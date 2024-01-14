/*!
 * ヘディング用ボール
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_bound_ball.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action/side_action_types.h"

static TbFloat32 RADIUS = 12.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -RADIUS , -RADIUS , RADIUS*2.0f , RADIUS*2.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(272,496,12,12),
};

static const TbCircleF32 COLLISION_CIRCLE( 0.0f , 0.0f , RADIUS );

static TB_NDEBUG_CONST TbUint32  DEAD_COUNT = 30;
static TB_NDEBUG_CONST TbUint32  ANIM_COUNT = 10;
static TB_NDEBUG_CONST TbFloat32 SCALE = 1.05f;
static TB_NDEBUG_CONST TbFloat32 FRICTION = 0.95f;
static TB_NDEBUG_CONST TbFloat32 WEIGHT = 50.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionBoundBall::SideActionBoundBall()
    : Super()
    , m_DeadTime(0)
{
    // スプライト
    
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_2);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetName("ball");

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    collision->SetContactedCallback( TbCreateMemFunc(this,&Self::onContacted) );

    // 重力設定
    SetAccelY(SIDE_ACTION_GRAVITY);

    SetWeight(WEIGHT);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionBoundBall::~SideActionBoundBall()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionBoundBall::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_DeadTime > 0 ) {
        // 死亡時
        TbSint32 animCount = m_DeadTime - (DEAD_COUNT-ANIM_COUNT);
        if( animCount >= 0 )
        {
            // 消えるアニメーション
            TbSprite& sprite = GetMainSprite();
            TbVector2 scale = sprite.GetScale();
            if( !m_BitArray.Test(FLAG_DISABLE_DEAD_FADE) ){
                scale *= SCALE;
            }
            sprite.SetScale( scale );

            TbFloat32 alpha = static_cast<TbFloat32>(animCount) / static_cast<TbFloat32>(ANIM_COUNT);
            if( alpha <= 0.0f || !m_BitArray.Test(FLAG_DISABLE_DEAD_FADE) ){
                sprite.SetAlpha(alpha);
            }
        }

        --m_DeadTime;
        if( 0 == m_DeadTime ) {
            if( m_DeadCallback.IsValid() ){
                m_DeadCallback();
            }
            Exit();
        }
    }else{
        if( GetCollision()->IsContacted(CMN_CONTACT_DOWN) )
        {
            // 左右の速度を摩擦
            TbVector2 velo = GetVelo();
            velo.SetX( velo.GetX()*FRICTION );
            SetVelo( velo );
        }
    }
}

/*!
 * 接触開始コールバック
 * @author teco
 * @since 2014.07.27
 */
void SideActionBoundBall::onContacted(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& info)
{
    // コリジョン同士の衝突
    static const TbFloat32 E = 1.0f; // 反発係数
    static const TbFloat32 VELO_MIN = 1.2f;
    static const TbFloat32 SPEED_MAX = 12.0f;

    // 運動量保存則に基づいた速度の変更(Y軸のみ)
#if 1
    {
        // 初速度0.0前提
        const TbVector4& prePos = GetPrePos();
        const TbVector2& preVelo = GetPreVelo();
        TbFloat32        posY = GetPos().GetY();
        TbFloat32 energy = prePos.GetY() * SIDE_ACTION_GRAVITY - TbSquare(preVelo.GetY())*0.5f;
        TbFloat32 veloY = TbSqrt(2.0f*((posY*SIDE_ACTION_GRAVITY)-energy));
        veloY = TbSign(GetVelo().GetY()) * veloY;
        SetVeloY(veloY);
    }
#endif

    if( info.collision && info.collision->GetActor() ) {

        ActionActor* target = info.collision->GetActor();
        TbVector2 v1 = GetVelo();
        TbVector2 v2 = target->GetVelo();
        TbFloat32 m1 = GetWeight();
        TbFloat32 m2 = target->GetWeight();
        
        // 衝突直接に射影
        TbFloat32 len1 = v1.Dot(normal);
        TbFloat32 len2 = v2.Dot(normal);
        TbVector2 projV1 = normal * len1;

        // 反射する必要性がなさそうな組み合わせは何もしない
        if( len1 * len2 > 0.0f ) {
            // 方向が同じ場合
            if( len1 > 0.0f ) {
                if( len2 > len1 ) {
                    // 何もしない
                    return;
                }
            }else{
                if( len1 < len2 ) {
                    // 何もしない
                    return;
                }
            }
        }else{
            if( len1 < 0.0f ){
                return;
            }
        }

        // WEIGHT_MAXを越えている場合は、とても重いものと軽いものでの衝突と判断する
#if 0
        if( m1 >= WEIGHT_MAX && m2 < WEIGHT_MAX ) {
            m2 = 0.0f;
        }else if( m2 >= WEIGHT_MAX && m1 < WEIGHT_MAX ) {
            m1 = 0.0f;
        }
#endif

        // 反発計算
        v1 -= projV1;
        TbFloat32 after = ((m1-m2*E)*len1+(1+E)*m2*len2)/(m1+m2);
        if( TbAbs(after) < VELO_MIN ) {
            after = 0.0f;
        }
        projV1 = normal * after;
        v1 += projV1;

        SetVelo(v1);

        // プレイヤーの頭上でY軸速度がなくなったら消える
        if( 0 == m_DeadTime ){
            if( info.collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER &&
                TB_FLOAT32_EQUAL_ZERO(v1.GetY()) ) 
            {
                m_DeadTime = DEAD_COUNT;
                ResetVelo();
                GetCollision()->SetActive(TB_FALSE);
            }
        }
    }else{
        // マップとの衝突
        TbVector2 velo = GetVelo();
        TbVector2 dir = normal * velo.Dot(normal);
        if( dir.Length() < VELO_MIN ){
            dir.Clear();
        }
        velo -= dir;
        velo += -dir * E;
        SetVelo(velo);
        if( 0 == m_DeadTime ){
#if 1
            // 地面に衝突すると消える
            if( TB_FLOAT32_EQUAL( normal.GetY() , 1.0f ) ){
                m_DeadTime = DEAD_COUNT;
                ResetVelo();
                GetCollision()->SetActive(TB_FALSE);
            }
#endif
        }
    }
}

/*!
 * イベント通知
 * @author Miyake_Shunske
 * @since 2015.01.26
 */ 
TbSint32 SideActionBoundBall::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_KEYBOX_ON == event ) {
        m_DeadTime = DEAD_COUNT;
        SetVelo(TbVector2(0.0f,3.0f));
        GetCollision()->SetActive(TB_FALSE);
        m_BitArray.SetBit(FLAG_DISABLE_DEAD_FADE);
        if( param ){
            TbFloat32 x = *reinterpret_cast<TbFloat32*>(param);
            SetPos(x,GetPosY());
        }
        return 1;
    }
    return 0;
}

