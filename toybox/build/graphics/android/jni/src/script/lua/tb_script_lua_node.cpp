/*!
 * Luaスクリプト操作用ノード
 * @author Miyake Shunsuke
 * @since 2010.12.25
 */
#include "tb_fwd.h"
#include "script/lua/tb_script_lua_node.h"
#include "base/string/tb_string_converter.h"
#include "base/string/tb_static_string.h"
#include "base/io/tb_file_all.h"
#include "base/io/tb_print.h"

namespace toybox
{

/*! 
 * コンストラクタ( スレッド作成用 )
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::TbScriptLuaNode( TbSint32 index , TbSint32 topStackSize )
    : m_Index( index )
    , m_BitArray()
    , m_State( ::luaL_newstate() )
    , m_StackTop( 0 )
    , m_TopStack( topStackSize )
{
    ::luaL_openlibs( m_State );
}

/*! 
 * コンストラクタ( スレッド作成用 )
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::TbScriptLuaNode( TbSint32 index , TbSint32 topStackSize , TbScriptLuaNode* node )
    : m_Index( index )
    , m_BitArray()
    , m_State( ::lua_newthread( node->m_State ) )
    , m_StackTop( 0 )
    , m_TopStack( topStackSize )
{
    m_BitArray.SetBit( FLAG_NEED_CLOSE , TB_TRUE );
}

/*! 
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::~TbScriptLuaNode()
{
    if( m_BitArray.Test( FLAG_NEED_CLOSE ) ){
        ::lua_close( m_State );
    }
}

/*!
 * バッファ実行
 * @param buf バッファ
 * @param size バッファサイズ
 * @author Miyake Shunsuke
 * @since 2011.08.25
 */
TbScriptLuaNode::Result TbScriptLuaNode::DoBuffer( const void* buf , TbUint32 size )
{
    const int ret = ::luaL_loadbuffer( m_State , static_cast<const TbChar8*>(buf) , size , "buf" );
    if(ret!=0){
        // エラーメッセージ取得。
        const char* str = ::lua_tostring( m_State , -1);
        TB_WARN_MSG(0,str);
        // エラーメッセージを削除
        ::lua_pop( m_State , 1 );
        return RESULT_FAIL;
    }
    TbScriptLuaReturnParam retParam;
    retParam.returnCount = LUA_MULTRET;
    TbScriptLuaNode::Result result = Call( retParam );
    return result;
}

/*! 
 * ファイル実行(完了復帰)
 * @author Miyake Shunsuke
 * @since 2013.09.21
 */
TbScriptLuaNode::Result TbScriptLuaNode::DoFileSync( const char* fpath , TbFilePathId pathId )
{
    TbFile file( fpath , TB_FILE_OPERATE_READ , TB_FILE_SYNC ,  pathId );
    if( TB_FAILED(file.Open()) ) {
        return RESULT_FAIL;
    }
    if( TB_FAILED( file.Read() ) ){
        return RESULT_FAIL;
    }
    TbScriptLuaNode::Result res = DoBuffer(file.GetReadBuffer(),file.GetReadBufferSize());
    return res;
}

/*!
 * タイプ取得
 * @author Miyake Shunsuke
 * @since 2010.12.29
 */
TbScriptLuaNode::Type TbScriptLuaNode::GetType()
{
    Type type = static_cast<Type>(::lua_type( m_State , -1 ));
    return type;
}

/*!
 * 相対パスでノード移動
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::GoToRelatively( const char* name )
{
    TbStaticString512 str = name;
    TbSint32 strSize = str.GetBufferSize();
    char* buffer = str.GetBuffer();
    char* token = buffer;
    TbBool isArray = TB_FALSE;
    for( TbSint32 i = 0; i < strSize; ++i ){
        if( !isArray ){
            // 非配列要素
            if( buffer[i] == '.' || buffer[i] == '[' || i == strSize-1 ){
                isArray = (buffer[i] == '[');
                buffer[i] = '\0';
                if( TbStrCharCount( token ) > 0 ){
                    if( 0 == TbStrCmp( token , "_parent" ) ){
                        if( GoToParent( 1 ) == RESULT_NOT_FOUND ){
                            return RESULT_NOT_FOUND;
                        }
                    }else{
                        if( GoToChild( token ) == RESULT_NOT_FOUND ){
                            return RESULT_NOT_FOUND;
                        }
                    }
                }
                token = &buffer[ i + 1 ];
            }
        }else{
            // 配列要素
            if( buffer[i] == ']' ){
                isArray = TB_FALSE;
                buffer[i] = '\0';
                if( GoToChild( TbStringConverter::ParseSint32( token ) ) == RESULT_NOT_FOUND ){
                    return RESULT_NOT_FOUND;
                }
                token = &buffer[ i + 1 ];
            }
        }
    }
    return RESULT_OK;
}

/*!
 * 絶対パスでノード移動
 * @author Miyake Shunsuke
 * @since 2011.08.26
 */
TbScriptLuaNode::Result TbScriptLuaNode::GoToAbsolutely( const char* name )
{
    // スタックをリセット
    ::lua_settop( m_State , m_StackTop );
    return GoToRelatively( name );
}

/*!
 * 先頭に移動
 * @author Miyake Shunsuke
 * @since 2011.08.26
 */
TbScriptLuaNode::Result TbScriptLuaNode::GoToTop()
{
    // スタックをリセット
    ::lua_settop( m_State , m_StackTop );
    return RESULT_OK;
}

/*!
 * 親ノードに移動
 * @author Miyake Shunsuke
 * @since 2010.12.29
 */
TbScriptLuaNode::Result TbScriptLuaNode::GoToParent( TbSint32 num )
{
    TbSint32 count = num;
    while( count > 0 ){
        // 親がテーブルかグローバルであればOK
        if( ( lua_istable( m_State , -2 ) && lua_gettop( m_State ) > m_StackTop ) || 
            ( lua_gettop(m_State) == ( m_StackTop + 1 ) ) )
        { 
            ::lua_pop( m_State , 1 );
        }else{
            return RESULT_NOT_FOUND;
        }
        --count;
    }
    return RESULT_OK;
}

/*!
 * 子ノードに移動
 * @author Miyake Shunsuke
 * @since 2010.12.29
 */
TbScriptLuaNode::Result TbScriptLuaNode::GoToChild( const char* name )
{
    if( lua_gettop( m_State ) == m_StackTop ){
        ::lua_getglobal( m_State , name );
    }else if( TYPE_TABLE == GetType() ){
        ::lua_getfield( m_State , -1 , name );
    }
    if( TYPE_NIL == GetType() ){
        ::lua_pop( m_State , 1 );
        return RESULT_NOT_FOUND;
    }
    return RESULT_OK;
}

/*!
 * 子ノードに移動(配列として)
 + 0始まりでアクセスできるようにしておく
 * @author Miyake Shunsuke
 * @since 2010.12.29
 */
TbScriptLuaNode::Result TbScriptLuaNode::GoToChild( TbSint32 index )
{
    if( lua_gettop( m_State ) == m_StackTop ){
        return RESULT_NOT_FOUND;
    }
    ::lua_pushinteger( m_State , index+1 );
    ::lua_gettable( m_State , -2 );
    if( TYPE_NIL == GetType() ){
        ::lua_pop( m_State , 1 );
        return RESULT_NOT_FOUND;
    }
    return RESULT_OK;
}

/*!
 * デバッグ情報出力
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
void TbScriptLuaNode::PrintDebugInfo()
{
    //スタックに積まれている数を取得する
    TbSint32 stackSize = ::lua_gettop(m_State);
    for( TbSint32 i = stackSize; i >= 1; i-- ) {
        TbSint32 type = ::lua_type(m_State, i);
        TbPrintf("Stack[%2d-%10s] : ", i, ::lua_typename(m_State,type) );
        switch( type ) {
        case LUA_TNUMBER:
            {
                //number型
                TbPrintf("%f", ::lua_tonumber(m_State, i) );
            }
            break;
        case LUA_TBOOLEAN:
            {
                //boolean型
                if( ::lua_toboolean(m_State, i) ) {
                    TbPrintf("true");
                }else{
                    TbPrintf("false");
                }
            }
            break;
        case LUA_TSTRING:
            {
                //string型
                TbPrintf("%s", lua_tostring(m_State, i) );
            }
            break;
        case LUA_TNIL:
            {
                //nil
            }
            break;
        default:
            {
                //その他の型
                TbPrintf("%s", lua_typename(m_State, type));
            }
            break;
        }
        TbPrintf("\n");
    }
}

/*!
 * 現在のノードにすぐ戻れるように保存用スタックにノード情報を積んでおく
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
void TbScriptLuaNode::PushStackTop()
{
    m_StackTop = ::lua_gettop( m_State );
    m_TopStack.PushBack( m_StackTop );
}

/*! 
 * 現在のノードを破棄して保存用スタックの先頭をカレントノードにする
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
void TbScriptLuaNode::PopStackTop()
{
    m_TopStack.PopBack();
    if( m_TopStack.GetCount() > 0 ){
        m_StackTop = m_TopStack.GetAt( m_TopStack.GetCount() -1 );
    }else{
        m_StackTop = 0;
    }
}

/*!
 * Sint32型に変換してみる
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::TryToSint32( TbSint32& dst )
{
    if( TYPE_NUMBER == GetType() ){
        dst = static_cast<TbSint32>(::lua_tonumber( m_State , -1 ));
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * Uint32型に変換してみる
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::TryToUint32( TbUint32& dst )
{
    if( TYPE_NUMBER == GetType() ){
        dst = static_cast<TbUint32>(::lua_tonumber( m_State , -1 ));
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * Float32型に変換してみる
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::TryToFloat32( TbFloat32& dst )
{
    if( TYPE_NUMBER == GetType() ){
        dst = static_cast<TbFloat32>(::lua_tonumber( m_State , -1 ));
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * 文字列型に変換してみる
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::TryToString( const char** dst )
{
    if( TYPE_STRING == GetType() && dst ){
        *dst = ::lua_tostring( m_State , -1 );
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE;
}

/*!
 * 長さを取得(テーブルなら数、文字列なら文字数)
 * @author Miyake_Shunsuke
 * @since 2014.09.08
 */
TbScriptLuaNode::Result TbScriptLuaNode::TryToTableLength( TbUint32& dst )
{
    dst = 0;
    Type type = GetType();
    if( TYPE_TABLE == type ){
        lua_pushnil( m_State );
        while( lua_next( m_State , -2 ) != 0 )
        {
            ++dst;
            lua_pop(m_State,1);
        }
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * テーブルに値設定
 * テーブルノード以外の場合は失敗
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::SetValue( const char* name , TbScriptLuaVariable& value )
{
    if( TYPE_TABLE == GetType() ){
        value.push( this );
        ::lua_setfield( m_State , -2 , name );
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE; // ノードが正しくない
}

/*! 
 * テーブルに値設定
 * テーブルノード以外の場合は失敗
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::SetValue( TbSint32 index , TbScriptLuaVariable& value )
{
    if( TYPE_TABLE == GetType() ){
        ::lua_pushinteger( m_State , index );
        value.push( this );
        ::lua_settable( m_State , -3 );
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE; // ノードが正しくない
}

/*!
 * 呼び出し
 * @author Miyake Shunsuke
 * @since 2011.01.02
 */
TbScriptLuaNode::Result TbScriptLuaNode::call( TbSint32 argNum , TbScriptLuaReturnParam& retParam )
{
    // 関数ノードであれば呼ぶ
    if( lua_isfunction( m_State , -1 - argNum ) ){
        TbSint32 top = ::lua_gettop( m_State );
        // 返り値数は限界値を超えていない
        TB_ASSERT( retParam.returnCount == LUA_MULTRET || retParam.returnCount <= TbScriptLuaReturnParam::RETURN_COUNT_MAX );
        // 呼び出し
        if( 0 != ::lua_pcall( m_State , argNum , retParam.returnCount , 0 ) ){
            ::lua_settop( m_State , top - 1 - argNum );
            return RESULT_FAIL;
        }
        TbSint32 returnCount = ( ::lua_gettop( m_State ) - ( top - 1 - argNum ) );
        // 設定していた返り値と実際の返り値を比較
        if( retParam.returnCount == LUA_MULTRET ){
            TbSint32 nowTop = ::lua_gettop( m_State );
            ::lua_settop( m_State , top - 1 - argNum );
        }else{
            TB_ASSERT( retParam.returnCount == returnCount );
            TbScriptLuaVariable var;
            for( TbSint32 i = 0; i < returnCount; ++i ){
                if( toVariable( var , TB_TRUE ) == RESULT_OK ){
                    if( !retParam.returnList.IsFull() ){
                        retParam.returnList.PushBack( var );
                    }
                }else{
                    // 返り値変換不能な値がスタックにある場合は失敗
                    ::lua_settop( m_State , top - 1 - argNum );
                    return RESULT_FAIL;
                }
            }
        }
        return RESULT_OK;
    }
    return RESULT_INVALID_NODE;
}

/*!
 * コルーチン呼び出し
 * @author Miyake Shunsuke
 * @since 2011.01.02
 */
TbScriptLuaNode::Result TbScriptLuaNode::resume( TbSint32 argNum , TbScriptLuaReturnParam& retParam )
{
    // 関数ノードであれば呼ぶ
    if( lua_isfunction( m_State , -1 - argNum ) ){
        TbSint32 top = ::lua_gettop( m_State );
        // 返り値数は限界値を超えていない
        TB_ASSERT( retParam.returnCount <= TbScriptLuaReturnParam::RETURN_COUNT_MAX );
        // 呼び出し
        TbSint32 retval = ::lua_resume( m_State , nullptr , argNum );
        // 正常に終了しなかったら失敗
        if( 0 != retval && LUA_YIELD != retval ){
            ::lua_settop( m_State , top - 1 - argNum );
            return RESULT_FAIL;
        }
        TbSint32 returnCount = ( ::lua_gettop( m_State ) - ( top - 1 - argNum ) );
        // 設定していた返り値と実際の返り値を比較
        TB_ASSERT( retParam.returnCount == returnCount );
        TbScriptLuaVariable var;
        for( TbSint32 i = 0; i < returnCount; ++i ){
            if( toVariable( var , TB_TRUE ) == RESULT_OK ){
                if( !retParam.returnList.IsFull() ){
                    retParam.returnList.PushBack( var );
                }
            }else{
                // 返り値変換不能な値がスタックにある場合は失敗
                ::lua_settop( m_State , top - 1 - argNum );
                return RESULT_FAIL;
            }
        }
        return ( LUA_YIELD == retval ) ? RESULT_YIELD : RESULT_OK;
    }
    return RESULT_INVALID_NODE;
}

/*!
 * 変数変換
 * @author Miyake Shunsuke
 * @since 2011.01.02
 */
TbScriptLuaNode::Result TbScriptLuaNode::toVariable( TbScriptLuaVariable& var , TbBool canPop )
{
    switch( GetType() )
    {
    case TYPE_NUMBER:
        var.Set( ::lua_tonumber( m_State , -1 ) );
        break;
    case TYPE_STRING:
        var.Set( ::lua_tostring( m_State , -1 ) );
        break;
    case TYPE_BOOL:
        var.Set( static_cast<TbSint32>( ::lua_toboolean( m_State , -1 ) ) );
        break;
    default:
        return RESULT_INVALID_NODE;
        break;
    }
    return RESULT_OK;
}

/*!
 * 引数変換
 * @author Miyake Shunsuke
 * @since 2011.01.05
 */
TbScriptLuaNode::Result TbScriptLuaNode::toArg( TbScriptLuaVariable& var )
{
    if( var.push( this ) ){
        return RESULT_OK;
    }
    return RESULT_INVALID_ARG;
}

/*!
 * 関数呼び出し
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Call( TbScriptLuaReturnParam& retParam )
{
    if( TYPE_FUNCTION == GetType() ){
        return call( 0 , retParam );
    }
    return RESULT_INVALID_NODE;
}

/*!
 * 関数呼び出し
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK ){
            return call( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*!
 * 関数呼び出し
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK )
        {
            return call( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*!
 * 関数呼び出し
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK && 
            toArg( arg2 ) == RESULT_OK )
        {
            return call( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*!
 * 関数呼び出し
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK && 
            toArg( arg2 ) == RESULT_OK &&
            toArg( arg3 ) == RESULT_OK )
        {
            return call( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*!
 * 関数呼び出し
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 , TbScriptLuaVariable& arg4 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK && 
            toArg( arg2 ) == RESULT_OK &&
            toArg( arg3 ) == RESULT_OK &&
            toArg( arg4 ) == RESULT_OK )
        {
            return call( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * 関数再開
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Resume( TbScriptLuaReturnParam& retParam )
{
    if( TYPE_FUNCTION == GetType() ){
        return resume( 0 , retParam );
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * 関数再開
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK ){
            return resume( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * 関数再開
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK )
        {
            return resume( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * 関数再開
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK && 
            toArg( arg2 ) == RESULT_OK )
        {
            return resume( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * 関数再開
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK && 
            toArg( arg2 ) == RESULT_OK &&
            toArg( arg3 ) == RESULT_OK )
        {
            return resume( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

/*! 
 * 関数再開
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode::Result TbScriptLuaNode::Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 , TbScriptLuaVariable& arg4 )
{
    if( TYPE_FUNCTION == GetType() ){
        TbSint32 top = ::lua_gettop( m_State );
        if( toArg( arg0 ) == RESULT_OK && 
            toArg( arg1 ) == RESULT_OK && 
            toArg( arg2 ) == RESULT_OK &&
            toArg( arg3 ) == RESULT_OK &&
            toArg( arg4 ) == RESULT_OK )
        {
            return resume( 0 , retParam );
        }else{
            ::lua_settop( m_State , top );
            return RESULT_INVALID_ARG;
        }
    }
    return RESULT_INVALID_NODE;
}

}
