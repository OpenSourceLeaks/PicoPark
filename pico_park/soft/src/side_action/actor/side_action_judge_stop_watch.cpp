/*!
 * 判定型ストップウォッチ
 * @author teco
 */

#include "fwd.h"
#include "side_action_judge_stop_watch.h"

#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionJudgeStopWatch::SideActionJudgeStopWatch()
    : Super()
    , m_TargetSec(0.01f)
    , m_FinishTimer(0.0f)
    , m_BestPlayer(-1)
    , m_BestTime(0)
{
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionJudgeStopWatch::~SideActionJudgeStopWatch()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionJudgeStopWatch::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_CHILD_NAME
    };
    if (PARAM_CHILD_NAME < userData.GetCount())
    {
        m_ChildName = userData.AsString(PARAM_CHILD_NAME);
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionJudgeStopWatch::OnUpdateAction(TbFloat32 deltatime)
{
    struct CompareFunc
    {
        bool operator()(const RankInfo& left , const RankInfo& right )
        {
            return left.time < right.time;
        }
    };
    m_FinishTimer += deltatime;
    if (0.1f <= m_FinishTimer)
    {
        if (!IsActive()) {
            TbBool isOn = TB_TRUE;
            NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON,&isOn);
            for (TbUint32 i = 0; i < Root::GetInstance().GetPlayerCount(); ++i)
            {
                TbBool find = TB_FALSE;
                for (TbSint32 j = 0; j < m_RankCount; ++j)
                {
                    if (m_RankInfo[j].playerIndex == i) {
                        find = TB_TRUE;
                        break;
                    }
                }
                if (!find)
                {
                    m_RankInfo[m_RankCount].playerIndex = i;
                    m_RankInfo[m_RankCount].time = TB_UINT32_MAX;
                    ++m_RankCount;
                }
            }
        }else if(IsWaitingResult()){
            TbInsertionSort(m_RankInfo, m_RankInfo + m_RankCount, CompareFunc());
            TbFloat32 WAIT_SEC = 1.5f;
            WAIT_SEC -= 0.1f * static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount()-CMN_PLAYER_MIN);
            TbBool isWait = TB_FALSE;
            for (TbSint32 i = m_RankCount - 1; 0 < i; --i)
            {
                if ( m_RankInfo[i].finishTimer < WAIT_SEC && 
                     m_RankInfo[i].time != m_RankInfo[0].time )
                {
                    m_RankInfo[i].finishTimer += deltatime;
                    isWait = TB_TRUE;
                    TbStaticString64 childName = TbStaticString64::Format("%s%d" , m_ChildName.GetBuffer() , m_RankInfo[i].playerIndex+1);
                    TbColor color = TbColor::FromRgbaF32(0.5f,0.5f,0.5f,0.5f);
                    GetActionStage()->NotifyEvent(childName.GetBuffer(), SIDE_ACTION_EVENT_SET_COLOR, &color);
                    if (m_RankInfo[i].time == m_RankInfo[i - 1].time){
                        continue;
                    }else {
                        break;
                    }
                }
            }
            // 
            if (!isWait)
            {
                SetPrizePlayerIndex(m_BestPlayer);
                TbSint32 sec = TbClamp<TbSint32>(m_BestTime/100, 0, 99);
                TbSint32 msec = static_cast<TbSint32>(m_BestTime) % 100;
                TbStaticString64 message = TbStaticString64::Format("WINNER\n(%02d:%02d)",sec,msec);
                SetResultMessage(message.GetBuffer());
            }
        }
    }
    if (!IsActive())
    {
        m_BestPlayer = -1;
        m_BestTime = TB_UINT32_MAX;
        m_RankCount = 0;
    }
    Super::OnUpdateAction(deltatime);
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionJudgeStopWatch::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_PLAYER_PARAM:
        {
            if(!IsActive())
            {
                SideActionEventPlayerParam* playerParam = reinterpret_cast<SideActionEventPlayerParam*>(param);
                if (!TB_FLOAT32_EQUAL_ZERO(playerParam->paramF32)) {
                    TbUint32 time = static_cast<TbUint32>(playerParam->paramF32*100.0f);
                    if (time == m_BestTime)
                    {
                        // 引き分け
                        m_BestPlayer = -1;
                    }
                    else if( time < m_BestTime )
                    {
                        m_BestPlayer = playerParam->playerIndex;
                        m_BestTime = time;
                    }
                    if (m_RankCount < TB_ARRAY_COUNT_OF(m_RankInfo))
                    {
                        m_RankInfo[m_RankCount].playerIndex = playerParam->playerIndex;
                        m_RankInfo[m_RankCount].time = time;
                        ++m_RankCount;
                    }
    //                TbPrintf("time=%u,m_BestTime=%u,m_BestPlayer=%u\n", time, m_BestTime,m_BestPlayer);
                }
            }
        }
        break;
    case SIDE_ACTION_EVENT_ALIVE:
        {
            m_FinishTimer = 0.0f;
        }
        break;
    }
    return Super::NotifyEvent(event,param);
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionJudgeStopWatch::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetFontSize(48.0f);
    fontBuffer->SetPoseMatrix(poseMatrix);
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    // 数
    TbSint32 sec = TbClamp<TbSint32>(static_cast<TbSint32>(m_TargetSec), 0, 99);
    TbSint32 msec = static_cast<TbSint32>(m_TargetSec*100.0f) % 100;

    TbFloat32 x = static_cast<TbSint32>(drawPos.GetX());
    TbFloat32 y = static_cast<TbSint32>(drawPos.GetY());
    fontBuffer->Printf(x, y,
        "TARGET TIME %02d:%02d", sec, msec);

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
