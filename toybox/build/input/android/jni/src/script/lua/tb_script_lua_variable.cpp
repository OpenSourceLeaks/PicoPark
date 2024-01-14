/*!
 * Luaスクリプト用変数
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */

#include "tb_fwd.h"
#include "script/lua/tb_script_lua_variable.h"
#include "script/lua/tb_script_lua_node.h"

namespace toybox
{

/*!
 * デフォルトコンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVariable::TbScriptLuaVariable()
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
}

/*!
 * Sint16型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVariable::TbScriptLuaVariable( TbSint16 value )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( value );
}

/*!
 * Uint16型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVariable::TbScriptLuaVariable( TbUint16 value )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( value );
}

/*!
 * Sint32型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVariable::TbScriptLuaVariable( TbSint32 value )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( value );
}

/*!
 * Sint16型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVariable::TbScriptLuaVariable( TbUint32 value )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( value );
}

/*!
 * Sint32型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVariable::TbScriptLuaVariable( TbFloat32 value )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( value );
}

/*!
 * Sint16型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVariable::TbScriptLuaVariable( TbFloat64 value )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( value );
}

/*!
 * 文字列型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
TbScriptLuaVariable::TbScriptLuaVariable( const char* string )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( string );
}

/*!
 * 文字列型コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
TbScriptLuaVariable::TbScriptLuaVariable( const TbString& string )
    : m_Type( TYPE_NULL )
    , m_ValueStr()
{
    Set( string );
}

/*!
 * Sint16型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( TbSint16 value )
{
    m_Type = TYPE_SINT16;
    m_Value.valueS16 = value;
}

/*!
 * Uint16型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( TbUint16 value )
{
    m_Type = TYPE_UINT16;
    m_Value.valueU16 = value;
}

/*!
 * Sint32型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( TbSint32 value )
{
    m_Type = TYPE_SINT32;
    m_Value.valueS32 = value;
}

/*!
 * Uint32型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( TbUint32 value )
{
    m_Type = TYPE_UINT32;
    m_Value.valueU32 = value;
}

/*!
 * Float32型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( TbFloat32 value )
{
    m_Type = TYPE_FLOAT32;
    m_Value.valueF32 = value;
}

/*!
 * Float64型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( TbFloat64 value )
{
    m_Type = TYPE_FLOAT64;
    m_Value.valueF64 = value;
}

/*!
 * 文字列型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( const char* value )
{
    if( value ){
        m_Type = TYPE_STRING;
        m_ValueStr = value;
    }
}

/*!
 * 文字列型設定
 * @author Miyake Shunsuke
 * @since 2011.01.03
 */
void TbScriptLuaVariable::Set( const TbString& value )
{
    if( value.GetBuffer() ){
        m_Type = TYPE_STRING;
        m_ValueStr = value;
    }
}

/*!
 * 代入演算子
 * @author Miyake Shunsuke
 * @since 2011.01.02
 */
TbScriptLuaVariable& TbScriptLuaVariable::operator=( const TbScriptLuaVariable& var )
{
    m_Type = var.m_Type;
    m_Value = var.m_Value;
    return (*this);
}

/*!
 * Luaスタックに値を積む
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbBool TbScriptLuaVariable::push( TbScriptLuaNode* node )
{
    if( !node ){
        return TB_FALSE;
    }
    switch( m_Type )
    {
    case TYPE_NULL:
        return TB_FALSE;
        break;
#if 0
    case TYPE_BOOL:
        {
            ::lua_pushboolean( node->m_State , m_Value.valueBool );
        }
        break;
#endif
    case TYPE_SINT16:
        {
            ::lua_pushinteger( node->m_State , m_Value.valueS16 );
        }
        break;
    case TYPE_UINT16:
        {
            ::lua_pushinteger( node->m_State , m_Value.valueS16 );
        }
        break;
    case TYPE_SINT32:
        {
            ::lua_pushinteger( node->m_State , m_Value.valueS32 );
        }
        break;
    case TYPE_UINT32:
        {
            ::lua_pushinteger( node->m_State , m_Value.valueU32 );
        }
        break;
    case TYPE_FLOAT32:
        {
            ::lua_pushnumber( node->m_State , m_Value.valueF32 );
        }
        break;
    case TYPE_FLOAT64:
        {
            ::lua_pushnumber( node->m_State , m_Value.valueF64 );
        }
        break;
    case TYPE_STRING:
        {
            ::lua_pushstring( node->m_State , m_ValueStr.GetBuffer() );
        }
        break;
    }
    return TB_TRUE;
}

}
