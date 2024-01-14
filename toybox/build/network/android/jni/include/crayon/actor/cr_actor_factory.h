/*! 
 * ファクトリークラス
 * @author teco
 * @since 2013.10.18
 */

#ifndef _INCLUDED_CR_ACTOR_FACTORY_H_
#define _INCLUDED_CR_ACTOR_FACTORY_H_

#include <base/util/tb_function.h>
#include <base/math/tb_vector2.h>
#include <script/lua/tb_script_lua.h>

namespace crayon
{

class CrActor;

class CrActorFactory
{
public:

    class UserDataUnit
    {
    public:

        enum Type
        {
            TYPE_NONE ,
            TYPE_F32 ,
            TYPE_STRING ,
        };

    public:

        // コンストラクタ
        UserDataUnit()
            : m_Type(TYPE_NONE)
            , m_Float(0.0f)
        {
        }

        // 小数設定
        void SetF32( TbFloat32 value ) {
            m_Type = TYPE_F32;
            m_Float = value;
        }

        // 文字列設定
        void SetString( const char* string ) {
            m_Type = TYPE_STRING;
            m_String = string;
        }

    public:

        // 型取得
        Type GetType() const {
            return m_Type;
        }

        // 整数として
        TbSint32    AsSint32() const {
            if( TYPE_NONE == m_Type ) {
                return 0;
            }
            TB_RESQUE_RET(TYPE_F32==m_Type,0);
            return static_cast<TbSint32>(m_Float);
        }
        // 小数として
        TbFloat32   AsFloat32() const {
            if( TYPE_NONE == m_Type ) {
                return 0.0f;
            }
            TB_RESQUE_RET(TYPE_F32==m_Type,0.0f);
            return m_Float;
        }
        // 文字列として
        const char* AsString() const {
            if( TYPE_NONE == m_Type ) {
                return "";
            }
            TB_RESQUE_RET(TYPE_STRING==m_Type,nullptr);
            return m_String.GetBuffer();
        }

    private:
        Type                        m_Type;
        TbFloat32                   m_Float;
        toybox::TbStaticString64    m_String;
    };

    class UserData
    {
    public:

        static const TbUint32 USERDATA_MAX = 32;

    public:

        // コンストラクタ
        UserData() 
            : m_Count(0)
        {}

    public:

        // 個数設定
        void SetCount( TbUint32 count ) {
            m_Count = count;
        }

        // 個数取得
        TbUint32 GetCount() const {
            return m_Count;
        }

        // 小数設定
        void SetS32(TbUint32 index, TbSint32 value) {
            if (index < TB_ARRAY_COUNT_OF(m_Unit)) {
                m_Unit[index].SetF32(static_cast<TbFloat32>(value));
            }
        }

        // 小数設定
        void SetF32( TbUint32 index , TbFloat32 value ) {
            if( index < TB_ARRAY_COUNT_OF(m_Unit) ){
                m_Unit[index].SetF32(value);
            }
        }

        // 文字列設定
        void SetString( TbUint32 index , const char* string ) {
            if( index < TB_ARRAY_COUNT_OF(m_Unit) ){
                m_Unit[index].SetString(string);
            }
        }

    public:

        // 型取得
        UserDataUnit::Type GetType(TbUint32 index) const {
            if (index < TB_ARRAY_COUNT_OF(m_Unit)) {
                return m_Unit[index].GetType();
            }
            return UserDataUnit::TYPE_NONE;
        }

        // 整数として
        TbSint32    AsSint32(TbUint32 index) const {
            if( index < TB_ARRAY_COUNT_OF(m_Unit) ){
                return m_Unit[index].AsSint32();
            }
            return static_cast<TbSint32>(0);
        }
        // 小数として
        TbFloat32   AsFloat32(TbUint32 index) const {
            if( index < TB_ARRAY_COUNT_OF(m_Unit) ){
                return m_Unit[index].AsFloat32();
            }
            return static_cast<TbFloat32>(0.0f);
        }
        // 文字列として
        const char* AsString(TbUint32 index) const {
            if( index < TB_ARRAY_COUNT_OF(m_Unit) ){
                return m_Unit[index].AsString();
            }
            return nullptr;
        }

    private:
        TbUint32        m_Count;
        UserDataUnit    m_Unit[USERDATA_MAX];
    };

    struct CreateParam
    {
        CreateParam() 
            : createThreshold(0.0f) , pos() , playerCount(0)
        {
        }
        TbFloat32                createThreshold;
        TbSint32                 playerCount;
        toybox::TbVector2        pos;
        UserData                 userData;
        toybox::TbStaticString32 typeName;
        toybox::TbStaticString32 name;
    };

    typedef toybox::TbStaticFunction32<CrActor* (const CreateParam&)> CreateFunc;

public:

    // コンストラクタ
    CrActorFactory();

    // デストラクタ
    ~CrActorFactory();

public:
    
    // 初期化
    void Initialize( toybox::TbScriptLuaNode* node );

    // 始めから再開始
    void Restart() {
        m_CreateIndex = 0;
    }

    // 更新
    void Update( TbFloat32 threshold );

    // スキップ
    void Skip( TbFloat32 threshold );
    
    // 生成関数を設定
    void SetCreateFunc( const CreateFunc& func ) {
        m_CreateFunc = func;
    }

    // プレイヤー数設定
    void SetPlayerCount( TbSint32 playerCount ) {
        m_PlayerCount = playerCount;
    }        

private:

    CreateFunc           m_CreateFunc;
    toybox::TbArray<CreateParam> m_CreateParam;
    TbUint32             m_CreateIndex;
    TbUint32             m_SkipCount;
    TbSint32             m_PlayerCount;

};

}

#endif
