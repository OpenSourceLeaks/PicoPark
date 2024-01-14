/*!
 * アクション用コリジョン
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_COLLISION_H_
#define _INCLUDED_ACTION_COLLISION_H_

#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_array.h>
#include <base/util/tb_function.h>
#include "common/cmn_util.h"

class ActionCollisionScene;
class ActionActor;
class ActionMap;
class ActionCollision;

struct ActionCollisionInfo
{
    ActionCollisionInfo() : collision(NULL) , map(NULL) , point() {}
    ActionCollision* collision;
    ActionMap*       map;
    TbPointS32       point;
};

class ActionCollision
{
    friend class ActionCollisionScene;

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

public:

    // 衝突コールバック(押し戻しは無しバージョン)
    typedef TbStaticFunction32<void (ActionCollision*)> CollidedCallback;

    // 接触開始コールバック
    typedef TbStaticFunction32<void (const TbVector2&,ContactTarget,const ActionCollisionInfo&)> ContactAddedCallback;

    // 接触コールバック
    typedef TbStaticFunction32<void (const TbVector2&,ContactTarget,const ActionCollisionInfo&)> ContactedCallback;

    // 接触終了コールバック
    typedef TbStaticFunction32<TbBool (const TbVector2&,ContactTarget,const ActionCollisionInfo&)> ContactRemovedCallback;

    // 押し戻されたコールバック
    typedef TbStaticFunction32<void (const TbVector2& pos,const TbVector2& pushBackVec)> ChangedPosCallback;

    // 衝突しているか
    static TbBool IsCollided( const     ActionCollision& target1 , 
                              const     ActionCollision& target2 , 
                              const     TbVector2* ofs1 = NULL ,
                              const     TbVector2* ofs2 = NULL , 
                              TbBool    force = TB_FALSE );

    // 前回衝突しているか
    static TbBool IsCollidedPre( const ActionCollision& target1 , 
                                 const ActionCollision& target2 , 
                                 const TbVector2* ofs1 = NULL ,
                                 const TbVector2* ofs2 = NULL);

public:
    
    // コンストラクタ
    ActionCollision();

    // デストラクタ
    ~ActionCollision();

public:

    // 初期化
    void Initialize( const TbRectF32& rect , TbUint32 pushBackTargetBit );

    // 初期化
    void Initialize( const TbCircleF32& circle , TbUint32 pushBackTargetBit );

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

    // 角度利用取得
    TbBool IsEnableAngle() const {
        return m_BitArray.Test(FLAG_ENABLE_ANGLE);
    }

    // 角度利用設定
    void SetEnableAngle( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_ANGLE,isEnable);
    }

public:

    // アクターの関連付け
    void SetActor( ActionActor* actor ) {
        m_Actor = actor;
    }

    // 関連付けられているアクター取得
    ActionActor* GetActor() {
        return m_Actor;
    }

    // 所属シーン取得
    const ActionCollisionScene* GetScene() const {
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

public:
    
    // 初期値設定
    void SetInitPos( const TbVector2& pos ) {
        m_Pos = pos;
        m_PrePos = pos;
    }

    // 座標設定
    void SetPos( const TbVector2& pos ) {
        m_Pos = pos;
        TB_ASSERT(!m_Pos.HasNan());
    }
    
    // 強制的に座標移動
    void SetPosForce( const TbVector2& pos ) {
        m_Pos = pos;
        m_PrePos = pos;
    }

    // 座標加算
    void AddPos( const TbVector2& pos ) {
        m_Pos += pos;
        TB_ASSERT(!m_Pos.HasNan());
    }

    // 位置を前の位置に戻す
    void RevertPos() {
        m_Pos = m_PrePos;
    }
  
    // 座標取得
    const TbVector2& GetPos() const {
        return m_Pos;
    }

    // 前回の座標取得
    const TbVector2& GetPrePos() const {
        return m_PrePos;
    }

    // 移動量取得
    TbVector2 GetMov() const {
        return m_Pos - m_PrePos;
    }

    // ローカル軸での矩形領域取得
    const TbRectF32& GetRect() const {
        return m_Rect;
    }

    // 矩形領域取得
    TbRectF32 GetGlobalRect() const {
        TbRectF32 result = m_Rect;
        result.x += m_Pos.GetX();
        result.y += m_Pos.GetY();
        return result;
    }

    // 前回の矩形領域取得
    TbRectF32 GetPreGlobalRect() const {
        TbRectF32 result = m_Rect;
        result.x += m_PrePos.GetX();
        result.y += m_PrePos.GetY();
        return result;
    }

    // 円形領域取得
    TbCircleF32 GetGlobalCircle() const {
        TB_ASSERT(SHAPE_TYPE_CIRCLE==m_ShapeType);
        TbCircleF32 result = m_Circle;
        result.x = m_Pos.GetX();
        result.y = m_Pos.GetY();
        return result;
    }

    // 前回の円形領域取得
    TbCircleF32 GetPreGlobalCircle() const {
        TB_ASSERT(SHAPE_TYPE_CIRCLE==m_ShapeType);
        TbCircleF32 result = m_Circle;
        result.x = m_PrePos.GetX();
        result.y = m_PrePos.GetY();
        return result;
    }

    // マップ内を移動
    TbBool   MoveMap( ActionMap* map , const TbVector2& mov , TbVector2* movResult );

public:

    // 接触数取得
    TbUint32 GetContactCount() const {
        return m_ContactCollision.GetCount();
    }

    // 接触コリジョン取得
    TbUint32 GetContactCount( const TbVector2& contactNormal , 
                              TbBool isRecursive , 
                              TbStaticArray<ActionCollision*,CONTACT_GET_MAX>* checkList = NULL ,
                              TbUint32 collisionLayerMask = 0xFFFFFFFF ) const;

    // 接触コリジョン取得
    TbUint32 GetContactCount( CmnContactFace type , 
                              TbBool isRecursive , 
                              TbStaticArray<ActionCollision*,CONTACT_GET_MAX>* checkList = NULL , 
                              TbUint32 collisionLayerMask = 0xFFFFFFFF ) const {
        return GetContactCount( CmnUtil::GetContactFaceNormal(type) , isRecursive , checkList , collisionLayerMask );
    }


    // 何かと接触している
    TbBool IsContacted() const {
        return !m_ContactCollision.IsEmpty() || !m_ContactMapArray.IsEmpty();
    }

    // 接触コリジョン取得
    TbUint32 GetContactCollision( ActionCollision* (&dst)[CONTACT_GET_MAX] , const TbVector2& contactNormal ) const;

    // 接触コリジョン取得
    TbUint32 GetContactCollision( ActionCollision* (&dst)[CONTACT_GET_MAX] , CmnContactFace type ) const {
        return GetContactCollision( dst , CmnUtil::GetContactFaceNormal(type) );
    }

    // 指定の法線方向で接触しているか
    TbBool IsContacted( const TbVector2& contactNormal , TbBool isEnableMap = TB_TRUE ) const;

    // 接触しているか
    TbBool IsContacted( CmnContactFace type , TbBool isEnableMap = TB_TRUE ) const {
        return IsContacted( CmnUtil::GetContactFaceNormal(type) , isEnableMap );
    }

    // 指定の法線方向でマップと接触しているか
    TbBool IsContactedMap( const TbVector2& contactNormal ) const;

    // 指定の法線方向でマップと接触しているか
    TbBool IsContactedMap( CmnContactFace type ) const {
        return IsContactedMap( CmnUtil::GetContactFaceNormal(type) );
    }

    // 接触コリジョン追加
    void AddContactCollision( ActionCollision* collision , const TbVector2& contactNormal );

        // 接触コリジョン追加
    void AddContactCollision( ActionCollision* collision , CmnContactFace type ) {
        AddContactCollision( collision , CmnUtil::GetContactFaceNormal(type) );
    }

    // 接触コリジョン除去
    void RemoveContactCollision( ActionCollision* collision );

public:

    // シーンに入る
    TbBool Enter( ActionCollisionScene* scene );

    // シーンから出る
    TbBool Exit();

private:
    
    enum Flag
    {
        FLAG_ACTIVE             , // 有効か
        FLAG_FIXED_POS          , // 
        FLAG_ENABLE_ANGLE       , // 角度利用
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
        ActionCollision* collision;
        TbVector2        normal;

        // コリジョンとの比較演算子(TbFind用)
        TbBool operator==( ActionCollision* target ) {
            return collision == target;
        }
    };
    typedef TbArray<ContactInfo> ContactArray;

    struct ContactMapInfo
    {
        TbVector2        normal;    // 接触法線

        // 比較演算子(TbFind用)
        TbBool operator==( const TbVector2& src ) {
            return normal == src;
        }
    };
    static const TbUint32 CONTACT_MAP_MAX = 8;
    typedef TbStaticArray<ContactMapInfo,CONTACT_MAP_MAX> ContactMapArray;

private:

    // 初期化
    void initialize( TbUint32 pushBackTargetBit );

    // 衝突コールバック
    void onCollided( ActionCollision* target ) {
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
    void addMapContact( const TbVector2& normal , ActionMap* map );

private: // Sceneから呼ばれる

    // 前準備
    void updatePre()
    {
        m_BitArray.ResetBit(FLAG_DISABLE_FIXED_POS);
        updateExpectedPos();
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
    TbVector2                 m_ExpectedPos;                        // 期待されている座標
    TbVector2                 m_Pos;                                // 実際の座標
    TbVector2                 m_PrePos;                             // 前回の座標
    TbAngle32                 m_Angle;                              // 角度
    TbAngle32                 m_PreAngle;                           // 前回の角度
    TbBitArray32              m_BitArray;                           // ビット配列
    TbUint32                  m_PushBackTarget;                     // コリジョンレイヤー
    ActionActor*              m_Actor;                              // 関連付けアクター
    ContactArray              m_ContactCollision;                   // 接触コリジョン
    ContactMapArray           m_ContactMapArray;                    // 接触マップ配列
    ContactMapArray           m_ContactMapArrayRequest;             // 接触マップ配列(リクエスト用)
    ActionMap*                m_ContactedMap;                       // 接触済みマップ

    TbVector2                 m_DebugPos;                                // 実際の座標

private: // 形状

    ShapeType                 m_ShapeType;                          // 形状タイプ
    TbRectF32                 m_Rect;                               // 矩形コリジョン領域
    TbCircleF32               m_Circle;                             // 円形コリジョン領域

private: // コールバック

    CollidedCallback          m_CollidedCallback;
    ContactAddedCallback      m_ContactAddedCallback;
    ContactedCallback         m_ContactedCallback;
    ContactRemovedCallback    m_ContactRemovedCallback;
    ChangedPosCallback        m_ChangedPosCallback;

private: // シーン管理

    ActionCollisionScene*     m_Scene;
    ActionCollision*          m_Next;
    ActionCollision*          m_Prev;

};

#endif
