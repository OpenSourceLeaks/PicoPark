/*!
 * Luaスクリプト操作用ノード
 * @author Miyake Shunsuke
 * @since 2010.12.25
 */
#ifndef _INCLUDED_TB_SCRIPT_LUA_NODE_H_
#define _INCLUDED_TB_SCRIPT_LUA_NODE_H_

#include <script/lua/tb_script_lua_fwd.h>
#include <script/lua/tb_script_lua_variable.h>
#include <base/container/tb_array.h>
#include <base/io/tb_file_types.h>

namespace toybox
{

/*!
 * 
 * @author Miyake Shunsuke
 * @since 2010.12.25
 */
class TbScriptLuaNode
{
public:

    enum Type
    {
        TYPE_INVALID = -1 , //!< 不正値
        TYPE_NIL,           //!< NIL値
        TYPE_BOOL,          //!< Bool値
        TYPE_LIGHTUSERDATA, //!< ライトユーザーデータ
        TYPE_NUMBER,        //!< 数値
        TYPE_STRING,        //!< 文字列
        TYPE_TABLE,         //!< テーブル
        TYPE_FUNCTION,      //!< 関数
        TYPE_USERDATA,      //!< ユーザーデータ
        TYPE_THREAD,        //!< スレッド
    };

    enum Result
    {
        RESULT_OK,      // 成功
        RESULT_FAIL,         // 失敗
        RESULT_NOT_FOUND,    // 発見できず
        RESULT_INVALID_NODE, // 不正ノードである
        RESULT_INVALID_ARG,  // 引数が不正がある
        RESULT_YIELD,        // 中断
    };
    
private:

    // コンストラクタ
    TbScriptLuaNode( TbSint32 index , TbSint32 topStackSize );
    // コンストラクタ( スレッド作成用 )
    TbScriptLuaNode( TbSint32 index , TbSint32 topStackSize , TbScriptLuaNode* node );
    // デストラクタ
    ~TbScriptLuaNode();
    
public: // 共通

    // インデックス番号取得
    TbSint32 GetIndex(){ return m_Index; }

    // タイプ取得
    Type GetType();

public: // スクリプトコード実行

    // バッファ実行
    Result DoBuffer( const void* buf , TbUint32 size );

    // ファイル実行(完了復帰)
    Result DoFileSync( const char* fpath , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

public: // ノード移動

    // 相対パスでノード移動
    Result GoToRelatively( const char* name );
    // 相対パスでノード移動
    template <typename T>
    Result GoToRelatively( const T& name ) {
        return GoToAbsolutely(name.GetBuffer());
    }
    // 絶対パスでノード移動
    Result GoToAbsolutely( const char* name );
    // 絶対パスでノード移動
    template <typename T>
    Result GoToAbsolutely( const T& name ) {
        return GoToAbsolutely(name.GetBuffer());
    }
    // 親ノードに移動
    Result GoToParent( TbSint32 num );
    // 子ノードに移動
    Result GoToChild( const char* name );
    // 子ノードに移動（配列として）
    Result GoToChild( TbSint32 index );
    // 先頭に移動
    Result GoToTop();
    // デバッグ情報出力
    void PrintDebugInfo();
    // Luaのスタックトップを保存しておく
    void PushStackTop();
    // Luaのスタックトップを保存用スタックから取り出して変更する
    void PopStackTop();

public: // 数値ノードとして
    
    // Sint32型に変換してみる
    Result TryToSint32( TbSint32& dst );
    // Uint32型に変換してみる
    Result TryToUint32( TbUint32& dst );
    // Float32型に変換してみる
    Result TryToFloat32( TbFloat32& dst );
    // 文字列配列に変換してみる
    Result TryToString( const char** dst );
    // ユーザーデータに変換してみる
    Result TryToUserData( void** dst );
    // 長さを取得(テーブルなら数、文字列なら文字数)
    Result TryToTableLength( TbUint32& dst );

public: // テーブルノードとして子から数値を取得

    // Sint32型に変換してみる
    Result TryToSint32( TbSint32& dst , const char* childName )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(childName) ) ){
            res = TryToSint32(dst);
            GoToParent(1);
        }
        return res;
    }

    // Uint32型に変換してみる
    Result TryToUint32( TbUint32& dst , const char* childName )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK== ( res = GoToChild(childName) ) ){
            res = TryToUint32(dst);
            GoToParent(1);
        }
        return res;
    }
    
    // Float32型に変換してみる
    Result TryToFloat32( TbFloat32& dst , const char* childName )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(childName) ) ){
            res = TryToFloat32(dst);
            GoToParent(1);
        }
        return res;
    }

    // 文字列配列に変換してみる
    Result TryToString( const char** dst , const char* childName )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(childName) ) ){
            res = TryToString(dst);
            GoToParent(1);
        }
        return res;
    }
    // ユーザーデータに変換してみる
    Result TryToUserData( void** dst , const char* childName )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(childName) ) ){
            res = TryToUserData(dst);
            GoToParent(1);
        }
        return res;
    }

    // Sint32型に変換してみる
    Result TryToSint32ByIndex( TbSint32& dst , const TbSint32 index )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(index) ) ){
            res = TryToSint32(dst);
            GoToParent(1);
        }
        return res;
    }

    // Uint32型に変換してみる
    Result TryToUint32ByIndex( TbUint32& dst , const TbSint32 index )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK== ( res = GoToChild(index) ) ){
            res = TryToUint32(dst);
            GoToParent(1);
        }
        return res;
    }
    // Float32型に変換してみる
    Result TryToFloat32ByIndex( TbFloat32& dst , const TbSint32 index )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(index) ) ){
            res = TryToFloat32(dst);
            GoToParent(1);
        }
        return res;
    }
    // 文字列配列に変換してみる
    Result TryToStringByIndex( const char** dst , const TbSint32 index )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(index) ) ){
            res = TryToString(dst);
            GoToParent(1);
        }
        return res;
    }
    // ユーザーデータに変換してみる
    Result TryToUserDataByIndex( void** dst , const TbSint32 index )
    {
        Result res = RESULT_FAIL;
        if( RESULT_OK == ( res = GoToChild(index) ) ){
            res = TryToUserData(dst);
            GoToParent(1);
        }
        return res;
    }

public: // テーブルノードとして
    
    // テーブルに値設定

    Result SetValue( const char* name , TbScriptLuaVariable& value );
    // テーブルに値設定
    Result SetValue( TbSint32 index , TbScriptLuaVariable& value );
    
public: // 関数ノードとして
    
    // 関数呼び出し
    Result Call( TbScriptLuaReturnParam& retParam );
    Result Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 );
    Result Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 );
    Result Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 );
    Result Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 );
    Result Call( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 , TbScriptLuaVariable& arg4 );

    // 関数再開
    Result Resume( TbScriptLuaReturnParam& retParam );
    Result Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 );
    Result Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 );
    Result Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 );
    Result Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 );
    Result Resume( TbScriptLuaReturnParam& retParam , TbScriptLuaVariable& arg0 , TbScriptLuaVariable& arg1 , TbScriptLuaVariable& arg2 , TbScriptLuaVariable& arg3 , TbScriptLuaVariable& arg4 );
    
private:
    enum {
        FLAG_NEED_CLOSE,
    };
private:
    // 呼び出し
    Result call( TbSint32 argNum , TbScriptLuaReturnParam& retParam );
    // 呼び出し
    Result resume( TbSint32 argNum , TbScriptLuaReturnParam& retParam );
    // 変数変換
    TbScriptLuaNode::Result toVariable( TbScriptLuaVariable& var , TbBool canPop );
    // 引数変換
    TbScriptLuaNode::Result toArg( TbScriptLuaVariable& var );
private:
    TbBitArray32 m_BitArray;        //!< ビット配列
    TbSint32   m_Index;             //!< 番号
    lua_State* m_State;             //!< Luaステート
    TbSint32 m_StackTop;            //!< Lua内部スタックの基準位置
    TbArray<TbSint32> m_TopStack;   //!< スタックトップを保存しておくスタック
private:
    friend class TbScriptLuaVM;
    friend class TbScriptLuaVariable;
};

}

#endif
