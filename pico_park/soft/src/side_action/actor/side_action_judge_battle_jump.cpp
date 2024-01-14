/*!
 * エンドレス用のジャンプスコア
 * @author teco
 */

#include "fwd.h"
#include "root.h"
#include "side_action_judge_battle_jump.h"
#include "side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "common/cmn_util.h"

static const TbFloat32 WIDTH = 40.0f + 1.0f;
static const TbFloat32 HEIGHT = 15.0f + 1.0f;

static const TbRectF32Pod SPRITE_RECT =
{
    -WIDTH*2.0f , -HEIGHT*2.0f , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT =
{
    CMN_TEX_UV_RECT(320,432,WIDTH,HEIGHT)
};

static const TbFloat32 DEFAULT_FONT_SIZE = 32.0f;
static const TbUint32  READY_COUNT = 3;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionJudgeBattleJump::SideActionJudgeBattleJump()
    : Super()
    , m_Phase(PHASE_TUTORIAL)
    , m_Count(0)
    , m_ActiveRefCount(0)
    , m_BestPlayer(-1)
    , m_Level(0)
    , m_CountInLevel(0)
{
    AddComponent(&m_SpriteHooker);

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionJudgeBattleJump::~SideActionJudgeBattleJump()
{
}

/*!
 * イベント通知
 * @author teco
 */
void SideActionJudgeBattleJump::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_TARGET ,
        PARAM_HAS_TUTORIAL ,
        PARAM_FAST_SPEED_RATE ,
        PARAM_SPEED_RATE ,
    };
    m_TargetName = userData.AsString(PARAM_TARGET);
    if (PARAM_HAS_TUTORIAL < userData.GetCount()) {
        m_HasTutorial = userData.AsSint32(PARAM_HAS_TUTORIAL);
    }
    if (PARAM_FAST_SPEED_RATE < userData.GetCount())
    {
        TbFloat32 fastSpeedRate = userData.AsFloat32(PARAM_FAST_SPEED_RATE);
        m_SpeedController.SetFastSpeedCoef(fastSpeedRate);
    }
    m_SpeedController.Setup(userData,PARAM_SPEED_RATE);
}

/*!
 * シーンに入った
 * ロジック更新
 * @author teco
 */
void SideActionJudgeBattleJump::OnEnter(void* userData) 
{
    Super::OnEnter(userData);
    GetStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_IMMORTAL,nullptr);
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionJudgeBattleJump::OnUpdateAction(TbFloat32 deltatime)
{
    // シーケンス制御
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    switch (m_Phase)
    {
    case PHASE_TUTORIAL:
        // チュートリアル
        {
            // アクティブ
            TbUint32 playerCount = stage->GetPlayerCount();
            for (TbUint32 i = 0; i < playerCount; ++i)
            {
                TbStaticString64 targetName;
                targetName.SetFormatedString("%s%u", m_TargetName.GetBuffer(), i + 1);
                GetStage()->NotifyEvent(targetName.GetBuffer(), SIDE_ACTION_EVENT_SUSPEND, nullptr);
            }

            // 成功と失敗の一枚絵の後、カウントダウン
            if (m_HasTutorial) {
                ++m_Phase;
            }
            else {
                m_Phase = PHASE_ACTIVE;
            }
        }
        break;
    case PHASE_TUTORIAL + 1:
        {
            // Open
        }
        break;
    case PHASE_TUTORIAL + 2:
        {
            // 閉じる
            m_Phase = PHASE_ACTIVE;
        }
        break;
    case PHASE_ACTIVE:
        {
            static const TbSint32 LEVEL_1_BORDER = 5;
            static const TbSint32 LEVEL_2_BORDER = 10;
            static const TbSint32 LEVEL_3_BORDER = 15;
            static const TbSint32 LEVEL_4_BORDER = 30;
            TbUint32 playerCount = stage->GetPlayerCount();
            TbUint32 count = m_Count - READY_COUNT;
            if (count == LEVEL_1_BORDER ||
                count == LEVEL_2_BORDER ||
                count == LEVEL_3_BORDER || 
                count == LEVEL_4_BORDER )
            {
                ++m_Level;
                m_CountInLevel = 0;
            }
            TbFloat32 speedScale = m_SpeedController.RandBallSpeedScale(m_CountInLevel,m_Level);
            for (TbUint32 i = 0; i < playerCount; ++i)
            {
                TbStaticString64 targetName;
                targetName.SetFormatedString("%s%u", m_TargetName.GetBuffer(), i + 1);
                GetStage()->NotifyEvent(targetName.GetBuffer(), SIDE_ACTION_EVENT_LEVEL_DOWN, nullptr);
                GetStage()->NotifyEvent(targetName.GetBuffer(), SIDE_ACTION_EVENT_LEVEL_UP, &speedScale);
                if (count == LEVEL_3_BORDER)
                {
                    TbFloat32 scale = 2.0f;
                    GetStage()->NotifyEvent(targetName.GetBuffer(), SIDE_ACTION_EVENT_SCALE, &scale);
                }
            }
        }
        // break through
    case PHASE_ACTIVE + 1:
        {
            // アクティブ
            TbUint32 playerCount = stage->GetPlayerCount();
            void* myName = const_cast<char*>(GetName());
            TbUint32 aliveCount = 0;
            for (TbUint32 i = 0; i < playerCount; ++i)
            {
                if (stage->GetPlayer(i)->IsAlive())
                {
                    ++aliveCount;
                }
            }
            if (2 <= aliveCount)
            {
                // 二人以上いたら再転送
                for (TbUint32 i = 0; i < playerCount; ++i)
                {
                    if (stage->GetPlayer(i)->IsAlive())
                    {
                        TbStaticString64 targetName;
                        targetName.SetFormatedString("%s%u", m_TargetName.GetBuffer(), i + 1);
                        if (GetStage()->NotifyEvent(targetName.GetBuffer(), SIDE_ACTION_EVENT_ACTIVATE, myName)) {
                            ++m_ActiveRefCount;
                        }
                    }
                }
            }
            m_Phase = PHASE_SUSPEND;
        }
        break;
    case PHASE_SUSPEND:
        // 通知待ち
        {
        }
        break;
    }

    // 判定結果
    if (!IsActive()) {
        SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
        TbUint32 gameOverCount = 0;
        TbUint32 playerCount = stage->GetPlayerCount();
        for (TbUint32 i = 0; i < playerCount; ++i)
        {
            SideActionPlayer* player = stage->GetPlayer(i);
            if (player->IsGameOver()) {
                ++gameOverCount;
            }
        }
        if( playerCount-1 <= gameOverCount)
        {
            for (TbUint32 i = 0; i < playerCount; ++i)
            {
                SideActionPlayer* player = stage->GetPlayer(i);
                if( player->IsAlive() ){
                    m_BestPlayer = i;
                    break;
                }
            }
            TbBool isOn = TB_TRUE;
            NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON, &isOn);
        }
    }else if (IsWaitingResult()) {
        SetPrizePlayerIndex(m_BestPlayer);
    }
    Super::OnUpdateAction(deltatime);
}

/*! 
 * イベント通知
 * @author teco
 */
TbSint32 SideActionJudgeBattleJump::NotifyEvent( TbUint32 event , void* param )
{
    switch (event)
    {
    case SIDE_ACTION_EVENT_ACTIVATE:
        {
            if (TB_VERIFY(PHASE_SUSPEND == m_Phase && 0 < m_ActiveRefCount))
            {
                --m_ActiveRefCount;
                if (0 == m_ActiveRefCount)
                {
                    // 全員が停止したら、再度有効
                    ++m_Count;
                    ++m_CountInLevel;
                    m_Phase = PHASE_ACTIVE;
                }
            }
        }
        break;
    case SIDE_ACTION_EVENT_GET_COUNT:
        return m_Count;
    }

    return Super::NotifyEvent(event,param);
}

/*!
 * 文字描画
 * @author teco
 */
void SideActionJudgeBattleJump::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());

    TbVector2 drawPos = GetDrawPos();
    drawPos.Add(SPRITE_RECT.x + SPRITE_RECT.width / 2, SPRITE_RECT.y + SPRITE_RECT.height / 2);
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    TbSint32 count = m_Count;
    TbSint32 centerX = static_cast<TbSint32>(stage->GetWindowWidth()/2.0f);
    TbSint32 centerY = static_cast<TbSint32>(stage->GetWindowHeight()/2.0f);
    if ( 5 <= stage->GetPlayerCount())
    {
        centerY += 30.0f;
    }
    fontBuffer->SetEnableBorder(TB_TRUE);
    if (count < READY_COUNT)
    {
        fontBuffer->Printf(centerX,centerY,"READY?");
    }
    else if (count == READY_COUNT)
    {
        TbBool param =TB_FALSE;
        GetStage()->NotifyEvent(nullptr, SIDE_ACTION_EVENT_IMMORTAL, &param);
        fontBuffer->Printf(centerX, centerY, "GO!");
    }

    TbFloat32 scale = 1.5f/stage->GetScale();
    fontBuffer->SetFontSize(DEFAULT_FONT_SIZE*scale);
    fontBuffer->SetEnableBorder(TB_FALSE);
    // 数
    TbSint32 dispCount = count - READY_COUNT;
    if (count < READY_COUNT)
    {
        dispCount = READY_COUNT - count;
    }

    {
        fontBuffer->SetFontSize(DEFAULT_FONT_SIZE);
        fontBuffer->Printf(
            static_cast<TbSint32>(drawPos.GetX()),
            static_cast<TbSint32>(drawPos.GetY()),
            "%d" , dispCount);
    }

    fontBuffer->PopSetting();
    fontBuffer->Flush();

}

