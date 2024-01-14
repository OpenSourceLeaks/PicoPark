/*!
 * アクション用コリジョン
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_ACTION_COLLISION_H_
#define _INCLUDED_CR_ACTION_COLLISION_H_

#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector3.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_array.h>
#include <base/util/tb_function.h>
#include <crayon/cr_util.h>

namespace crayon
{

class CrActionCollisionScene;
class CrActionActor;
class CrActionMap;
class CrActionCollision;

struct CrActionCollisionInfo
{
    CrActionCollisionInfo() : collision(NULL) , map(NULL) , point() {}
    CrActionCollision* collision;
    CrActionMap*       map;
    toybox::TbPointS32       point;
};

class CrActionCollision
{
    friend class CrActionCollisionScene;

public:

    // 接触対象
    enum ContactTarget
    {
        CONTACT_TARGET_MAP ,
        CONTACT_TARGET_COLLISION , 
    };

    // 押し戻しレイヤー
    enum PushBackTarget
    {
        PUSH_BACK_TARGET_MAP    = TB_BIT(0) , // マップとの押し戻し
        PUSH_BACK_TARGET_COLLISION = TB_BIT(1) , // コリジョン同士の押し戻し

        PUSH_BACK_TARGET_ALL = PUSH_BACK_TARGET_COLLISION | PUSH_BACK_TARGET_MAP
    };

    enum ShapeType
    {
        SHAPE_TYPE_RECT     , // 矩形
        SHAPE_TYPE_CIRCLE   , // 円形
        SHAPE_TYPE_SEGMENT  , // 線分
        SHAPE_TYPE_POLYGON  , // ポリゴン
    };

    static const TbUint32 CONTACT_GET_MAX = 16;
    static const TbUint32 LAYER_DEFAULT = 0;
    static const TbUint32 LAYER_INVALID = static_cast<TbUint32>(-1);

    typedef toybox::TbStaticArray<CrActionCollision*, CONTACT_GET_MAX> ContactList;

public:

    // 衝突コールバック(押し戻しは無しバージョン)
    typedef toybox::TbStaticFunction32<void (CrActionCollision*)> CollidedCallback;

    // 接触開始コールバック
    typedef toybox::TbStaticFunction32<void (const toybox::TbVector2&,ContactTarget,const CrActionCollisionInfo&)> ContactAddedCallback;

    // 接触コールバック
    typedef toybox::TbStaticFunction32<void (const toybox::TbVector2&,ContactTarget,const CrActionCollisionInfo&)> ContactedCallback;

    // 接触終了コールバック
    typedef toybox::TbStaticFunction32<TbBool (const toybox::TbVector2&,ContactTarget,const CrActionCollisionInfo&)> ContactRemovedCallback;

    // 押し戻されたコールバック
    typedef toybox::TbStaticFunction32<void (const toybox::TbVector2& pos,const toybox::TbVector2& pushBackVec)> ChangedPosCallback;

    // スケール変更コールバック
    typedef toybox::TbStaticFunction32<void( TbFloat32 scaleX , TbFloat32 scaleY )> ChangedScaleCallback;

    // 衝突しているか
    static TbBool IsCollided( const     CrActionCollision& target1 , 
                              const     CrActionCollision& target2 , 
                              const     toybox::TbVector2* ofs1 = NULL ,
                              const     toybox::TbVector2* ofs2 = NULL , 
                              TbBool    force = TB_FALSE );

    // 前回衝突しているか
    static TbBool IsCollidedPre( const CrActionCollision& target1 , 
                                 const CrActionCollision& target2 , 
                                 const toybox::TbVector2* ofs1 = NULL ,
                                 const toybox::TbVector2* ofs2 = NULL ,
                                 TbBool    force = TB_FALSE);

public:
    
    // コンストラクタ
    CrActionCollision();

    // デストラクタ
    ~CrActionCollision();

public:

    // 初期化
    void Initialize( const toybox::TbRectF32& rect , TbUint32 pushBackTargetBit );

    // 初期化
    void Initialize( const toybox::TbCircleF32& circle , TbUint32 pushBackTargetBit );

    // アクティブ状態か設定
    void SetActive( TbBool isActive ) {
        m_BitArray.SetBit(FLAG_ACTIVE,isActive);
    }

    // アクティブ状態か取得
    TbBool IsActive() const {
        return m_BitArray.Test(FLAG_ACTIVE);
    }

    // 位置固定か(コリジョンから押し戻されない)
    void SetFixedPos( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_FIXED_POS,isEnable);
    }

    // 位置固定か
    TbBool IsFixedPos() const {
        return m_BitArray.Test(FLAG_FIXED_POS) && !m_BitArray.Test(FLAG_DISABLE_FIXED_POS);
    }

    // 位置固定コリジョンから押されることを許容するか
    void SetEnablePushedFromFixedPosCollision( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_DISABLE_PUSHED_FROM_FIXED_POS,!isEnable);
    }

    // 位置固定コリジョンから押されることを許容するか
    TbBool IsEnablePushedFromFixedPosCollision() const {
        return !m_BitArray.Test(FLAG_DISABLE_PUSHED_FROM_FIXED_POS);
    }

    // 押し戻しターゲット設定
    void SetPushBackTarget( TbUint32 target ) {
        m_PushBackTarget = target;
    }

    // 押し戻しターゲット取得
    TbUint32 GetPushBackTarget() const {
        return m_PushBackTarget;
    }

    // 押し戻しターゲットは有効か
    TbBool IsActivePushBackTarget( PushBackTarget target ) const {
        return ( m_PushBackTarget & target ) == target;
    }

    // レイヤー設定
    void SetLayer( TbUint32 layer ) {
        m_Layer = layer;
    }

    // レイヤー取得
    TbUint32 GetLayer() const {
        return m_Layer;
    }

    // 優先度取得
    TbSint32 GetPriority() const {
        return m_Priority;
    }

    // 優先度設定
    void SetPriority( TbSint32 prio ) {
        TB_RESQUE(!m_Scene);
        m_Priority = prio;
    }

    // 形状取得
    ShapeType GetShapeType() const {
        return m_ShapeType;
    }

    // 属性設定
    void SetAttribute(TbUint32 attribute) {
        m_Attribute = attribute;
    }

    // 属性取得
    TbUint32 GetAttribute() const {
        return m_Attribute;
    }

    // 角度利用設定
    void SetEnableAngle(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_ANGLE, isEnable);
    }

    // 角度利用取得
    TbBool IsEnableAngle() const {
        return m_BitArray.Test(FLAG_ENABLE_ANGLE);
    }

    // 頭上接触オブジェクト強制検索(複数キャラの頭上に乗っかっていると片方だけ検知できなかったので)
    void SetEnableForceSearchUpContact(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_SEARCH_FIND_UP_CONTACT,isEnable);
    }

    // 頭上接触オブジェクト強制検索(複数キャラの頭上に乗っかっていると片方だけ検知できなかったので)
    TbBool IsEnableForceSearchUpContact() const {
        return m_BitArray.Test(FLAG_ENABLE_SEARCH_FIND_UP_CONTACT);
    }

public:

    // アクターの関連付け
    void SetActor( CrActionActor* actor ) {
        m_Actor = actor;
    }

    // 関連付けられているアクター取得
    CrActionActor* GetActor() {
        return m_Actor;
    }

    // 所属シーン取得
    const CrActionCollisionScene* GetScene() const {
        return m_Scene;
    }

    // 衝突コールバック
    void SetCollidedCallback( const CollidedCallback& callback ) {
        m_CollidedCallback = callback;
    }

    // 接触追加コールバック
    void SetContactAddedCallback( const ContactAddedCallback& callback ) {
        m_ContactAddedCallback = callback;
    }

    // 接触コールバック
    void SetContactedCallback( const ContactedCallback& callback ) {
        m_ContactedCallback = callback;
    }

    // 接触除去コールバック
    void SetContactRemovedCallback( const ContactRemovedCallback& callback ) {
        m_ContactRemovedCallback = callback;
    }

    // 押し戻しコールバック
    void SetChangedPosCallback( const ChangedPosCallback& callback ) {
        m_ChangedPosCallback = callback;
    }

    // スケール値変更
    void SetChangedScaleCallback(const ChangedScaleCallback& callback) {
        m_ChangedScaleCallback = callback;
    }

public:
    
    // 初期値設定
    void SetInitPos( const toybox::TbVector2& pos ) {
        m_Pos = pos;
        m_PrePos = pos;
    }

    // 座標設定
    void SetPos( const toybox::TbVector2& pos ) {
        m_Pos = pos;
        TB_ASSERT(!m_Pos.HasNan());
    }
    
    // 強制的に座標移動
    void SetPosForce( const toybox::TbVector2& pos ) {
        m_Pos = pos;
        m_PrePos = pos;
    }

    // 座標加算
    void AddPos( const toybox::TbVector2& pos ) {
        m_Pos += pos;
        TB_ASSERT(!m_Pos.HasNan());
    }

    // 位置を前の位置に戻す
    void RevertPos() {
        m_Pos = m_PrePos;
    }
  
    // 座標取得
    const toybox::TbVector2& GetPos() const {
        return m_Pos;
    }

    // 前回の座標取得
    const toybox::TbVector2& GetPrePos() const {
        return m_PrePos;
    }

    // 移動量取得
    toybox::TbVector2 GetMov() const {
        return m_Pos - m_PrePos;
    }

    // ローカル軸での矩形領域取得
    const toybox::TbRectF32 GetRect() const {
        toybox::TbRectF32 rect = m_Rect;
        rect.x -= m_ScalePivot.GetX();
        rect.y -= m_ScalePivot.GetY();
        rect = rect * m_Scale;
        rect.x += m_ScalePivot.GetX();
        rect.y += m_ScalePivot.GetY();
        return rect;
    }

    // 矩形領域取得
    toybox::TbRectF32 GetGlobalRect() const {
        toybox::TbRectF32 result = GetRect();
        result.x += m_Pos.GetX();
        result.y += m_Pos.GetY();
        return result;
    }

    // 前回の矩形領域取得
    toybox::TbRectF32 GetPreGlobalRect() const {
        toybox::TbRectF32 result = GetRect();
        result.x += m_PrePos.GetX();
        result.y += m_PrePos.GetY();
        return result;
    }

    // 生の矩形情報取得
    toybox::TbRectF32 GetRectRaw() const {
        return m_Rect;
    }

    // 円形領域取得
    toybox::TbCircleF32 GetGlobalCircle() const {
        TB_ASSERT(SHAPE_TYPE_CIRCLE==m_ShapeType);
        toybox::TbCircleF32 result = m_Circle;
        result.x = m_Pos.GetX() * m_Scale.GetX();
        result.y = m_Pos.GetY() * m_Scale.GetY();
        return result;
    }

    // 前回の円形領域取得
    toybox::TbCircleF32 GetPreGlobalCircle() const {
        TB_ASSERT(SHAPE_TYPE_CIRCLE==m_ShapeType);
        toybox::TbCircleF32 result = m_Circle;
        result.x = m_PrePos.GetX() * m_Scale.GetX();
        result.y = m_PrePos.GetY() * m_Scale.GetY();
        result.radius *= toybox::TbMax( m_Scale.GetX() , m_Scale.GetY() ); // 楕円非対応
        return result;
    }

    // マップ内を移動
    TbBool   MoveMap( CrActionMap* map , 
                      const toybox::TbVector2& mov ,
                      toybox::TbVector2* movResult , 
                      TbBool isEnableSlide = TB_TRUE );

    // 指定コリジョンに接触せずに移動
    TbBool   Move( CrActionCollision* co , const toybox::TbVector2& mov, toybox::TbVector2* movResult );

    // スケール設定
    void SetScale(toybox::TbVector2 scale) {
        m_Scale = scale;
    }

    // スケール設定
    void SetScale(TbFloat32 scale) {
        m_Scale.Set(scale, scale);
    }

    // スケール設定(強制)
    void SetScaleImmediate(toybox::TbVector2 scale) {
        m_Scale = scale;
        m_PreScale = m_Scale;
        m_ExpectedScale = m_Scale;
    }


    // スケール設定(強制)
    void SetScaleImmediate(TbFloat32 scale) {
        m_Scale.Set(scale, scale);
        m_PreScale = m_Scale;
        m_ExpectedScale = m_Scale;
    }

    // スケール取得
    const toybox::TbVector2& GetScale() const {
        return m_Scale;
    }

    // スケール用ピボット設定
    void SetScalePivot(toybox::TbVector2 pivot) {
        m_ScalePivot = pivot;
    }

    // スケール用ピボット取得
    toybox::TbVector2 GetScalePivot() const {
        return m_ScalePivot;
    }

public:

    // 接触数取得
    TbUint32 GetContactCount() const {
        return m_ContactCollision.GetCount();
    }

    // 接触コリジョン取得
    TbUint32 GetContactCount( const toybox::TbVector2& contactNormal , 
                              TbBool isRecursive , 
                              ContactList* checkList = NULL ,
                              TbUint32 collisionLayerMask = 0xFFFFFFFF ) const;

    // 接触コリジョン取得
    TbUint32 GetContactCount( CrContactFace type , 
                              TbBool isRecursive , 
                              ContactList* checkList = NULL ,
                              TbUint32 collisionLayerMask = 0xFFFFFFFF ) const 
    {
        return GetContactCount( CrUtil::GetContactFaceNormal(type) , isRecursive , checkList , collisionLayerMask );
    }

    // 再帰最大数
    TbUint32 GetContactRecursiveMax( const toybox::TbVector2& contactNormal,
                                     TbUint32 collisionLayerMask = 0xFFFFFFFF );
    
    // 再帰最大数
    TbUint32 GetContactRecursiveMax(CrContactFace type, TbUint32 collisionLayerMask = 0xFFFFFFFF)
    {
        return GetContactRecursiveMax(CrUtil::GetContactFaceNormal(type),collisionLayerMask);
    }


    // 何かと接触している
    TbBool IsContacted() const {
        return !m_ContactCollision.IsEmpty() || !m_ContactMapArray.IsEmpty();
    }

    // 接触コリジョン取得
    CrActionCollision* GetContactCollision(TbUint32 index) {
        if( index < static_cast<TbUint32>(m_ContactCollision.GetCount()) )
        {
            return m_ContactCollision[index].collision;
        }
        return nullptr;
    }

    // 接触コリジョン取得
    const CrActionCollision* GetContactCollision(TbUint32 index) const {
        if (index < static_cast<TbUint32>(m_ContactCollision.GetCount()))
        {
            return m_ContactCollision[index].collision;
        }
        return nullptr;
    }

    // 接触コリジョン取得
    TbUint32 GetContactCollision( CrActionCollision* (&dst)[CONTACT_GET_MAX] , const toybox::TbVector2& contactNormal ) const;

    // 接触コリジョン取得
    TbUint32 GetContactCollision( CrActionCollision* (&dst)[CONTACT_GET_MAX] , CrContactFace type ) const {
        return GetContactCollision( dst , CrUtil::GetContactFaceNormal(type) );
    }

    // 接触コリジョン存在するか
    TbBool ExistsContactCollision(CrActionCollision* src, const toybox::TbVector2& contactNormal) const;

    // 接触コリジョン存在するか
    TbBool ExistsGetContactCollision(CrActionCollision* src, CrContactFace type) const {
        return ExistsContactCollision(src, CrUtil::GetContactFaceNormal(type));
    }

    // 指定の法線方向で接触しているか
    TbBool IsContacted( const toybox::TbVector2& contactNormal , TbBool isEnableMap = TB_TRUE , TbUint32* attribute = nullptr ) const;

    // 接触しているか
    TbBool IsContacted( CrContactFace type , TbBool isEnableMap = TB_TRUE , TbUint32* attribute = nullptr ) const {
        return IsContacted( CrUtil::GetContactFaceNormal(type) , isEnableMap , attribute);
    }

    // 指定の法線方向でマップと接触しているか
    TbBool IsContactedMap( const toybox::TbVector2& contactNormal ) const;

    // 指定の法線方向でマップと接触しているか
    TbBool IsContactedMap( CrContactFace type ) const {
        return IsContactedMap( CrUtil::GetContactFaceNormal(type) );
    }

    // マップと接触しているか
    TbBool IsContactedMap() const {
        return !m_ContactMapArray.IsEmpty();
    }

    // 接触コリジョン追加
    void AddContactCollision( CrActionCollision* collision , const toybox::TbVector2& contactNormal );

        // 接触コリジョン追加
    void AddContactCollision( CrActionCollision* collision , CrContactFace type ) {
        AddContactCollision( collision , CrUtil::GetContactFaceNormal(type) );
    }

    // 接触コリジョン除去
    void RemoveContactCollision( CrActionCollision* collision );

    // 接触コリジョンクリア
    void ClearContactCollision();

public:

    // ユーザーデータ設定
    void SetUserDataU32(TbUint32 index, TbUint32 value) {
        if (index < TB_ARRAY_COUNT_OF(m_UserDataU32)) {
            m_UserDataU32[index] = value;
        }
    }

    // ユーザーデータ設定
    TbUint32 GetUserDataU32(TbUint32 index) const {
        if (index < TB_ARRAY_COUNT_OF(m_UserDataU32)) {
            return m_UserDataU32[index];
        }
        return 0;
    }

public:

    // シーンに入る
    TbBool Enter( CrActionCollisionScene* scene );

    // シーンから出る
    TbBool Exit();

private:
    
    enum Flag
    {
        FLAG_ACTIVE                       , // 有効か
        FLAG_FIXED_POS                    , // 
        FLAG_ENABLE_ANGLE                 , // 角度利用
        FLAG_ENABLE_SEARCH_FIND_UP_CONTACT , // 頭上接触オブジェクト強制検索
        FLAG_DISABLE_FIXED_POS , 
        FLAG_DISABLE_PUSHED_FROM_FIXED_POS , 
    };
    static const TbSint32 CONTACT_ACTOR_MAX_DEFAULT = 8;
    
    struct ContactInfo
    {
        enum State {
            STATE_NEW , 
            STATE_DEFAULT ,
        };
        State            state;
        CrActionCollision* collision;
        toybox::TbVector2        normal;

        // コリジョンとの比較演算子(TbFind用)
        TbBool operator==( CrActionCollision* target ) {
            return collision == target;
        }
    };
    typedef toybox::TbArray<ContactInfo> ContactArray;

    struct ContactMapInfo
    {
        toybox::TbVector2        normal;    // 接触法線

        // 比較演算子(TbFind用)
        TbBool operator==( const toybox::TbVector2& src ) {
            return normal == src;
        }
    };
    static const TbUint32 CONTACT_MAP_MAX = 8;
    typedef toybox::TbStaticArray<ContactMapInfo,CONTACT_MAP_MAX> ContactMapArray;

    static const TbUint32  USER_DATA_U32_MAX = 8;
    static const TbUint32  USER_DATA_F32_MAX = 8;

private:

    // 初期化
    void initialize( TbUint32 pushBackTargetBit );

    // 衝突コールバック
    void onCollided( CrActionCollision* target ) {
        if( m_CollidedCallback.IsValid() ){
            m_CollidedCallback( target );
        }
    }

    // 接触しているコリジョンの状態更新
    void updateContactCollision();

    // 接触しているマップの状態更新
    void updateContactMap();

    // 接触解除
    void resetContact();

    // マップとの接触情報追加
    void addMapContact( const toybox::TbVector2& normal , CrActionMap* map );

private: // Sceneから呼ばれる

    // 前準備
    TbBool updatePre()
    {
        m_BitArray.ResetBit(FLAG_DISABLE_FIXED_POS);
        updateExpectedPos();
        // スケールを一旦前フレームに戻す
        if ( m_Scale != m_PreScale)
        {
            m_ExpectedScale = m_Scale;
            m_Scale = m_PreScale;
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // 状態更新
    void updateAfter();

    // 期待値更新
    void updateExpectedPos() {
        m_ExpectedPos = m_Pos;
    }

    // 位置固定フラグを一時解除
    void pauseFixedPos() {
        m_BitArray.SetBit(FLAG_DISABLE_FIXED_POS);
    }

private:
    
    TbUint32                  m_Layer;                              // このコリジョンが属するレイヤー
    TbSint32                  m_Priority;                           // 値が大きいと先に処理されます
    toybox::TbVector2         m_ExpectedPos;                        // 期待されている座標
    toybox::TbVector2         m_Pos;                                // 実際の座標
    toybox::TbVector2         m_PrePos;                             // 前回の座標
    TbAngle32                 m_Angle;                              // 角度
    TbAngle32                 m_PreAngle;                           // 前回の角度
    toybox::TbBitArray32      m_BitArray;                           // ビット配列
    TbUint32                  m_Attribute;                          // 属性
    TbUint32                  m_PushBackTarget;                     // コリジョンレイヤー
    CrActionActor*            m_Actor;                              // 関連付けアクター
    ContactArray              m_ContactCollision;                   // 接触コリジョン
    ContactMapArray           m_ContactMapArray;                    // 接触マップ配列
    ContactMapArray           m_ContactMapArrayRequest;             // 接触マップ配列(リクエスト用)
    CrActionMap*              m_ContactedMap;                       // 接触済みマップ
    toybox::TbVector2         m_DebugPos;                           // 実際の座標

private: // 形状

    ShapeType                 m_ShapeType;                          // 形状タイプ
    toybox::TbRectF32         m_Rect;                               // 矩形コリジョン領域
    toybox::TbCircleF32       m_Circle;                             // 円形コリジョン領域
    toybox::TbVector2         m_ExpectedScale;                      // 期待するスケール設定
    toybox::TbVector2         m_Scale;                              // スケール 
    toybox::TbVector2         m_PreScale;                           // 前回のスケール
    toybox::TbVector2         m_ScalePivot;

private: // ユーザーデータ

    TbUint32                  m_UserDataU32[USER_DATA_U32_MAX];
    TbFloat32                 m_UserDataF32[USER_DATA_F32_MAX];

private: // コールバック

    CollidedCallback          m_CollidedCallback;
    ContactAddedCallback      m_ContactAddedCallback;
    ContactedCallback         m_ContactedCallback;
    ContactRemovedCallback    m_ContactRemovedCallback;
    ChangedPosCallback        m_ChangedPosCallback;
    ChangedScaleCallback      m_ChangedScaleCallback;

private: // シーン管理

    CrActionCollisionScene*     m_Scene;
    CrActionCollision*          m_Next;
    CrActionCollision*          m_Prev;

};

}

#endif
