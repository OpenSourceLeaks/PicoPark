/*!
 * バウワウ(早く動いているやつを襲う)
 * @author teco
 */

#include "fwd.h"
#include "side_action_bowwow_enemy.h"
#include "side_action/actor/side_action_anim_data.h"
#include <crayon/map/cr_action_map.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"
#include <sound/tb_sound.h>


#define ANIM_INTERVAL_W (64.0f/CMN_TEX_SIZE)

static const TbRectF32Pod SPRITE_RECT = 
{
    -64.0f , -64.0f , 128.0f , 128.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    -30.0f , SPRITE_RECT.y+25.0f , 60.0f , SPRITE_RECT.height-50.0f 
};

const TbRectF32Pod UV_RECT =
{
    CMN_TEX_UV_RECT(768,128,64,64)
};

// 歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_SLEEP_KEY[] = {
    { 0.0f            , TbRectF32() },
    { CMN_ANIM_FRAME_STEP , TbRectF32(0.0f,ANIM_INTERVAL_W,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*2.0f , TbRectF32(0.0f,ANIM_INTERVAL_W *2,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*3.0f , TbRectF32(0.0f,ANIM_INTERVAL_W,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*4.0f , TbRectF32() },
};
const TbAnimationKeyFrameDataRectF32 ANIM_SLEEP(ANIM_SLEEP_KEY, TB_ARRAY_COUNT_OF(ANIM_SLEEP_KEY), TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT, TB_FALSE);

// 睡眠警告アニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_SLEEP_WARN_KEY[] = {
    { 0.0f            , TbRectF32(CMN_TEX_POS(96.0f),0.0f,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP , TbRectF32(CMN_TEX_POS(96.0f),ANIM_INTERVAL_W,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*2.0f , TbRectF32(CMN_TEX_POS(96.0f),ANIM_INTERVAL_W * 2,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*3.0f , TbRectF32(CMN_TEX_POS(96.0f),ANIM_INTERVAL_W,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*4.0f , TbRectF32(CMN_TEX_POS(96.0f),0.0f,0.0f,0.0f) },
};
const TbAnimationKeyFrameDataRectF32 ANIM_SLEEP_WARN(ANIM_SLEEP_WARN_KEY, TB_ARRAY_COUNT_OF(ANIM_SLEEP_WARN_KEY), TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT, TB_FALSE);

// 歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame ANIM_WAKE_KEY[] = {
    { 0.0f            , TbRectF32(-CMN_TEX_POS(96.0f),0.0f,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP , TbRectF32(-CMN_TEX_POS(96.0f),ANIM_INTERVAL_W,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*2.0f , TbRectF32(-CMN_TEX_POS(96.0f),0.0,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*3.0f , TbRectF32(-CMN_TEX_POS(96.0f),ANIM_INTERVAL_W,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*4.0f , TbRectF32(-CMN_TEX_POS(96.0f),0.0,0.0f,0.0f) },
};
const TbAnimationKeyFrameDataRectF32 ANIM_WAKE(ANIM_WAKE_KEY, TB_ARRAY_COUNT_OF(ANIM_WAKE_KEY), TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT, TB_FALSE);

struct AnimData
{
    const char* name;
    const TbAnimationKeyFrameDataRectF32* data;
};

AnimData ANIM_TABLE[] =
{
    { "sleep" , &ANIM_SLEEP },
    { "warn"  , &ANIM_SLEEP_WARN },
    { "wake"  , &ANIM_WAKE }
};

static TbFloat32 SPEED = 1.0f;


// 睡眠記号アニメーション

#define SLEEP_ANIM_INTERVAL_W (32.0f/CMN_TEX_SIZE)

static const TbRectF32Pod SLEEP_SPRITE_RECT =
{
    56.0f , -120.0f , 64.0f , 64.0f
};

const TbRectF32Pod SLEEP_UV_RECT =
{
    CMN_TEX_UV_RECT(544,80,32,32)
};

// 歩きアニメーション
static TbAnimationKeyFrameDataRectF32::KeyFrame SLEEP_ANIM_SLEEP_KEY[] = {
    { CMN_ANIM_FRAME_STEP*0.0f  , TbRectF32(-SLEEP_ANIM_INTERVAL_W,0.0f,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*2.0f  , TbRectF32() },
    { CMN_ANIM_FRAME_STEP*4.0f  , TbRectF32(SLEEP_ANIM_INTERVAL_W,0.0f,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*6.0f  , TbRectF32(SLEEP_ANIM_INTERVAL_W * 2,0.0f,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*8.0f  , TbRectF32(SLEEP_ANIM_INTERVAL_W,0.0f,0.0f,0.0f) },
    { CMN_ANIM_FRAME_STEP*10.0f  , TbRectF32() },
    { CMN_ANIM_FRAME_STEP*12.0f  , TbRectF32(-SLEEP_ANIM_INTERVAL_W,0.0f,0.0f,0.0f) },
};
const TbAnimationKeyFrameDataRectF32 SLEEP_ANIM_SLEEP(SLEEP_ANIM_SLEEP_KEY, TB_ARRAY_COUNT_OF(SLEEP_ANIM_SLEEP_KEY), TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT, TB_FALSE);


/*!
 * コンストラクタ
 * @author teco
 */
SideActionBowwowEnemy::SideActionBowwowEnemy()
    : Super()
    , m_State(STATE_SLEEP)
    , m_StateSec(0.0f)
    , m_Sec(0.0f)
    , m_AttackSpeed(1.0f)
{
    // スプライト設定
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect   = SPRITE_RECT; //
    initParam.param.uvRect = UV_RECT;
    m_SpriteNode.Initialize(initParam);
    {
        TbSpriteNodeAnimTree::InitNodeParam animInitParam;
        animInitParam.name = "root";
        for ( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(ANIM_TABLE); ++i )
        {
            animInitParam.param.name = ANIM_TABLE[i].name;
            animInitParam.param.uvOffsetAnim = ANIM_TABLE[i].data;
            m_SpriteAnim[i].Initialize(1, &animInitParam);
        }
    }
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(&m_SpriteNode,&m_SpriteAnim[ANIM_SLEEP]);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    // ZZZ
    {
        initParam.name = "root";
        initParam.param.color.SetColorU8(0xFF, 0xFF, 0xFF, 0xFF);
        initParam.param.rect = SLEEP_SPRITE_RECT; //
        initParam.param.uvRect = SLEEP_UV_RECT;
        m_SleepSpriteNode.Initialize(initParam);
        {
            TbSpriteNodeAnimTree::InitNodeParam animInitParam;
            animInitParam.name = "root";
            animInitParam.param.name = "sleep";
            animInitParam.param.uvOffsetAnim = &SLEEP_ANIM_SLEEP;
            m_SleepSpriteAnim.Initialize(1, &animInitParam);
        }
        m_SleepSprite.GetSprite().Initialize(&m_SleepSpriteNode,&m_SleepSpriteAnim);
        m_SleepSprite.GetSprite().SetEnableLoop(TB_TRUE);
        m_SleepSprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_1);
        AddComponent(&m_SleepSprite);
    }

    // コリジョン
    TbUint32 pushBackTarget = 0;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_ENEMY);

    CrActionCollision* collisionAttack = CreateCollision(COLLISION_RECT, 0, TB_TRUE);
    collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided) );
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionBowwowEnemy::~SideActionBowwowEnemy()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionBowwowEnemy::Setup( const CrActorFactory::UserData& userData )
{
    if( 0 < userData.GetCount() ){
        m_AttackSpeed = userData.AsFloat32(0);
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBowwowEnemy::OnUpdateAction( TbFloat32 deltaSec)
{
    static TB_NDEBUG_CONST TbUint32 FRAME_COUNT = 30;
    static const TbFloat32 WARNING_DISTANCE_FIRST = 300.0f;
    static const TbFloat32 WARNING_DISTANCE = 400.0f;
    static const TbFloat32 WAKEUP_PLAYER_VELO = 1.5f;
    static const TbFloat32 ATTACK_PLAYER_VELO = 2.5f;

    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    if (!stage)  {
        return;
    }
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    TbUint32 playerCount = stage->GetPlayerCount();
    TbFloat32 movMax = 0.0f;
    TbSint32 targetIndex = -1;
    TbFloat32 warninDistance = (m_State == STATE_SLEEP) ? WARNING_DISTANCE_FIRST : WARNING_DISTANCE;
    for (TbUint32 i = 0; i < playerCount; ++i)
    {
        const SideActionPlayer* player = stage->GetPlayer(i);
        TbFloat32 distance = TbAbs(player->GetPosX() - GetPosX());
        if( distance < warninDistance)
        {
            TbVector2 mov = player->GetAverageMov(FRAME_COUNT);
            TbFloat32 movLen = mov.Length();
            if (movMax < movLen) {
                movMax = movLen;
                targetIndex = i;
            }
        }
    }

//    font->Printf(100,200,"%0.2f", movMax);
    switch (m_State)
    {
    case STATE_SLEEP:
        updateOffset(deltaSec);
        if (0 <= targetIndex) {
            setState(STATE_SLEEP_WARN);
        }
        break;
    case STATE_SLEEP_WARN:
        updateOffset(deltaSec);
        if (WAKEUP_PLAYER_VELO < movMax) {
            setState(STATE_WAKE);
        }
        break;
    case STATE_WAKE:
        if ( targetIndex < 0 ) {
            setState(STATE_SLEEP);
        }else if (ATTACK_PLAYER_VELO < movMax) {
            setState(STATE_ATTACK);
        }
        break;
    case STATE_ATTACK:
        {
            // ターゲットに向かって
            if (0 <= targetIndex)
            {
                TbVector2  basePos = GetPosXY();
                basePos.Add(0.0f, SPRITE_RECT.x + SPRITE_RECT.height);
                m_AttackVelo = stage->GetPlayer(targetIndex)->GetPosXY() - basePos;
                m_AttackVelo.Normalize();
                m_AttackVelo *= m_AttackSpeed;
                // テクスチャの向きが逆
                if (0.0f < m_AttackVelo.GetX())
                {
                    LookLeft();
                }
                else
                {
                    LookRight();
                }
            }
            AddPos(m_AttackVelo);
        }
        break;
    case STATE_ATTACK_AFTER:
        AddPos(m_AttackVelo);
        break;
    }

    if (m_StateSec < 1000.0f) {
        m_StateSec += deltaSec;
    }
}

/*! 
 * 接触開始コールバック
 * @author teco
 */
void SideActionBowwowEnemy::onCollided(CrActionCollision* target)
{
    if (target->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER)
    {
        // 死亡通知
        GetStage()->RequestScreenQuake();
        TbPlaySound("hit");
        target->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_DEAD,nullptr);
        if (STATE_ATTACK == m_State)
        {
            m_State = STATE_ATTACK_AFTER;
        }
    }
}

void SideActionBowwowEnemy::updateOffset(TbFloat32 deltaSec)
{
    static const TbFloat32 AMPTITUDE_SPEED = 2.0f;
    static const TbFloat32 AMPTITUDE = 8.0f;

    TbFloat32 oldY = TbSin(m_Sec*AMPTITUDE_SPEED)*AMPTITUDE;
    m_Sec += deltaSec;
    TbFloat32 rad = m_Sec*AMPTITUDE_SPEED;
    TbFloat32 y = TbSin(rad)*AMPTITUDE;
    if (TB_PI_RAD*2.0f <= rad) {
        m_Sec = (rad - TB_PI_RAD*2.0f) / AMPTITUDE_SPEED;
    }
    TbFloat32 mov = y - oldY;
    AddPos(0.0f,mov,0.0f);
}

// 状態設定
void SideActionBowwowEnemy::setState( State state )
{
    TbSprite& sprite = GetMainSprite();
    switch (state)
    {
    case STATE_SLEEP:
        sprite.SetNodeAnimation(&m_SpriteAnim[ANIM_SLEEP]);
        sprite.SetEnableStep(TB_TRUE);
        m_SleepSprite.GetSprite().SetVisible(TB_TRUE);
        m_SleepSprite.GetSprite().SetFrame(0.0f);
        break;
    case STATE_WAKE:
        if(m_State == STATE_SLEEP_WARN)
        {
            sprite.SetNodeAnimation(&m_SpriteAnim[ANIM_SLEEP_WARN]);
            sprite.SetEnableStep(TB_FALSE);
            m_SleepSprite.GetSprite().SetVisible(TB_FALSE);
        }
        break;
    case STATE_ATTACK:
        if (m_State == STATE_WAKE)
        {
            sprite.SetNodeAnimation(&m_SpriteAnim[ANIM_ANGRY]);
            sprite.SetEnableStep(TB_TRUE);
        }
        break;
    }
    m_State = state;
    m_StateSec = 0.0f;
}
