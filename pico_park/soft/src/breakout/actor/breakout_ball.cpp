/*!
 * ブロック崩し用ボール
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "breakout_ball.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

static TbFloat32 RADIUS = 12.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -RADIUS , -RADIUS , RADIUS*2.0f , RADIUS*2.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(256,512,12,12),
};
static const TbRectF32Pod SPRITE_UV_RECT_BATTLE =
{
    CMN_TEX_UV_RECT(272,496,12,12),
};

static const TbCircleF32 COLLISION_CIRCLE( 0.0f , 0.0f , RADIUS );

static TB_NDEBUG_CONST TbFloat32 APPROACH_RATE = 0.1f;
static TB_NDEBUG_CONST TbFloat32 SPEED_MAX = 4.0f;
static TB_NDEBUG_CONST TbUint32  DEAD_COUNT = 180;
static TB_NDEBUG_CONST TbUint32  ANIM_COUNT = 10;
static TB_NDEBUG_CONST TbFloat32 SCALE = 1.05f;
static const TbUint32 SE_TIMER = 3;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
BreakoutBall::BreakoutBall(TbBool isBattle)
    : Super()
    , m_Friction(0.0f)
    , m_Speed(SPEED_MAX)
    , m_SpeedScale(1.0f)
    , m_IsEnableDead(TB_TRUE)
    , m_DeadCount(0)
    , m_SoundTimer(SE_TIMER)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    if (isBattle)
    {
        initParam.param.uvRect = SPRITE_UV_RECT_BATTLE; //
    }
    else
    {
        initParam.param.uvRect = SPRITE_UV_RECT; //
    }
    m_SpriteNode.Initialize(initParam);
    sprite.Initialize(&m_SpriteNode);

    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
                              CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
    collision->SetContactedCallback( TbCreateMemFunc(this,&Self::onContacted) );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
BreakoutBall::~BreakoutBall()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void BreakoutBall::OnUpdateAction( TbFloat32 deltatime )
{
    if (0 < m_SoundTimer) {
        --m_SoundTimer;
    }
    // 速度更新
    TbVector2 v1 = GetVelo();
    v1.Normalize();
    v1 *= m_Speed * m_SpeedScale;
    SetVelo(v1);

    if( m_DeadCount > 0 ) {

        TbSint32 animCount = m_DeadCount - (DEAD_COUNT-ANIM_COUNT);
        if( animCount >= 0 )
        {
            // 消えるアニメーション
            TbSprite& sprite = GetMainSprite();
            TbVector2 scale = sprite.GetScale();
            scale *= SCALE;
            sprite.SetScale( scale );

            TbFloat32 alpha = static_cast<TbFloat32>(animCount) / static_cast<TbFloat32>(ANIM_COUNT);
            sprite.SetAlpha(alpha);
        }

        --m_DeadCount;
        if( 0 == m_DeadCount ) {
            if( m_DeadCallback.IsValid() ){
                m_DeadCallback();
            }
            Exit();
        }
    }else{
        if( m_Friction > 0.0f )
        {
            TbFloat32 attenuation = 1.0f - m_Friction;
            TbVector2 velo = GetVelo();
            velo *= attenuation;
            if( velo.LengthSq() < 0.01f ){
                velo.Clear();
            }
            SetVelo(velo);
        }
    }
}

/*!
 * 接触開始コールバック
 * @author teco
 * @since 2014.07.27
 */
void BreakoutBall::onContacted(const TbVector2& normal, CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info)
{
    // コリジョン同士の衝突
    static const TbFloat32 E = 1.0f; // 反発係数
    if( info.collision && info.collision->GetActor() ) {

        CrActionActor* target = info.collision->GetActor();
        TbVector2 v1 = GetCollisionVelo();
        TbVector2 v2 = target->GetCollisionVelo();
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
        if( m1 >= WEIGHT_MAX && m2 < WEIGHT_MAX ) {
            m2 = 0.0f;
        }else if( m2 >= WEIGHT_MAX && m1 < WEIGHT_MAX ) {
            m1 = 0.0f;
        }

        // 反発計算
        v1 -= projV1;
        TbFloat32 after = ((m1-m2*E)*len1+(1+E)*m2*len2)/(m1+m2);
        projV1 = normal * after;
        v1 += projV1;

        playSE(v1);

#if 1   // 速度固定
        v1.Normalize();
        v1 *= m_Speed * m_SpeedScale;
#endif

        TbFloat32 len = normal.Length();

        SetVelo(v1);

        // コリジョンヒット
        OnHitCollision( info.collision );

    }else{
        // マップとの衝突
        TbVector2 velo = GetVelo();
        TbVector2 dir = normal * velo.Dot(normal);
        velo -= dir;
        velo += -dir * E;
        SetVelo(velo);

        playSE(velo);

        TbFloat32 len = normal.Length();
        if (len > 100.0f ) {
            int a = 0;
        }

        TbUint32 chip = info.map->GetMapChip(info.point.x,info.point.y);
        if( CR_ACTION_MAP_CHIP_BREAK_BEGIN <= chip && chip < CR_ACTION_MAP_CHIP_BREAK_END ){
            if (chip == CR_ACTION_MAP_CHIP_BREAK5) {
                TbFloat32 scale = 2.0f;
                GetStage()->NotifyEvent(nullptr, SIDE_ACTION_EVENT_SPEED_UP, &scale);
                // 他のはもとに戻す
                info.map->ReplaceMapChip(CR_ACTION_MAP_CHIP_BREAK5,CR_ACTION_MAP_CHIP_BREAK2);
            }
            info.map->SetMapChip(info.point.x,info.point.y,CR_TILE_MAP_CHIP_NONE);
            OnHitMapChip(info.point.x,info.point.y);
        }else if( 0 == m_DeadCount ){
            // 地面に衝突すると消える
            if( CR_TILE_MAP_CHIP_BLOCK != chip && !(CR_TILE_MAP_CHIP_BLOCK_WL <= chip && chip <= CR_TILE_MAP_CHIP_BLOCK_WR) )
            {
                if( TB_FLOAT32_EQUAL( normal.GetY() , 1.0f ) && m_IsEnableDead ){
                    m_DeadCount = DEAD_COUNT;
                    ResetVelo();
                    GetCollision()->SetActive(TB_FALSE);
                }
            }
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 BreakoutBall::NotifyEvent(TbUint32 event, void* param)
{
    switch (event)
    {
    case SIDE_ACTION_EVENT_SPEED_UP:
        if (param) {
            TbFloat32 scale = *reinterpret_cast<TbFloat32*>(param);
            m_SpeedScale *= scale;
            m_SpeedScale = TbMin(m_SpeedScale,2.0f);
        }
        break;
    }
    return 0;
}

// SE再生
void BreakoutBall::playSE(TbVector2 velo)
{
    if (0 == m_SoundTimer && 0.1f <= velo.LengthSq())
    {
        TbPlaySound("bound");
        m_SoundTimer = SE_TIMER;
    }
}
