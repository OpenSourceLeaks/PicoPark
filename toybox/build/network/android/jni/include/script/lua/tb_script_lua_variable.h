/*!
 * Luaスクリプト用変数
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */

#ifndef _INCLUDED_TB_SCRIPT_LUA_VARIABLE_H_
#define _INCLUDED_TB_SCRIPT_LUA_VARIABLE_H_

#include <script/lua/tb_script_lua_fwd.h>
#include <base/container/tb_static_array.h>
#include <base/string/tb_string.h>

namespace toybox
{

class TbScriptLuaNode;

/*!
 * 変数クラス
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */
class TbScriptLuaVariable
{
public:
    enum Type
    {
        TYPE_NULL,
        TYPE_SINT16,
        TYPE_UINT16,
        TYPE_SINT32,
        TYPE_UINT32,
        TYPE_FLOAT32,
        TYPE_FLOAT64,
        TYPE_STRING,
    };
public:
    // NULL
    TbScriptLuaVariable();
    // Sint16型
    explicit TbScriptLuaVariable( TbSint16 value );
    // Uint16型
    explicit TbScriptLuaVariable( TbUint16 value );
    // Sint32型
    explicit TbScriptLuaVariable( TbSint32 value );
    // Uint32型
    explicit TbScriptLuaVariable( TbUint32 value );
    // Float32型
    explicit TbScriptLuaVariable( TbFloat32 value );
    // Float64型
    explicit TbScriptLuaVariable( TbFloat64 value );
    // 文字列型
    explicit TbScriptLuaVariable( const char* string );
    explicit TbScriptLuaVariable( const TbString& string );
    // デストラクタ
    ~TbScriptLuaVariable(){}
public:
    // 代入演算子
    TbScriptLuaVariable& operator=( const TbScriptLuaVariable& var );
public:
    // Sint16型設定
    void Set( TbSint16 value );
    // Uint16型設定
    void Set( TbUint16 value );
    // Sint32型設定
    void Set( TbSint32 value );
    // Uint32型設定
    void Set( TbUint32 value );
    // Float32型設定
    void Set( TbFloat32 value );
    // Float64型設定
    void Set( TbFloat64 value );
    // 文字列型設定
    void Set( const char* value );
    void Set( const TbString& value );
    //! リセット
    void Reset(){ m_Type = TYPE_NULL; }
    //! 型情報取得
    Type GetType(){ return m_Type; }
    //! Sint16型として取得( 型チェックはしない )
    TbSint16 ToSint16(){ return m_Value.valueS16; }
    //! Uint16型として取得( 型チェックはしない )
    TbUint16 ToUint16(){ return m_Value.valueU16; }
    //! Sint32型として取得( 型チェックはしない )
    TbSint32 ToSint32(){ return m_Value.valueS32; }
    //! Uint32型として取得( 型チェックはしない )
    TbSint32 ToUint32(){ return m_Value.valueU32; }
    //! Float32型として取得( 型チェックはしない )
    TbFloat32 ToFloat32(){ return m_Value.valueF32; }
    //! Float64型として取得( 型チェックはしない )
    TbFloat64 ToFloat64(){ return m_Value.valueF64; }
    //! 文字列( 型チェックはしない )
    const TbString& ToString() { return m_ValueStr; }
private:
    // Luaスタックに値を積む
    TbBool push( TbScriptLuaNode* node );
private:
    Type m_Type;
    union Value {
        TbSint16 valueS16;
        TbSint16 valueU16;
        TbSint32 valueS32;
        TbSint32 valueU32;
        TbFloat32 valueF32;
        TbFloat64 valueF64;
    } m_Value;
    TbString m_ValueStr;
private:
    friend class TbScriptLuaNode;
};

/*! 
 * 返り値格納構造体
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */
struct TbScriptLuaReturnParam
{
    static const TbSint32 RETURN_COUNT_MAX = 5;
    TbScriptLuaReturnParam( TbSint32 retCount = 0 ) : returnCount(retCount) , returnList() {}
    TbSint32                                            returnCount; //!< 返り値数
    TbStaticArray<TbScriptLuaVariable,RETURN_COUNT_MAX> returnList;  //!< 返り値格納先
};

}

#endif
