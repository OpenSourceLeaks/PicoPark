/*!
 * バトル用ストップウォッチ判定
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_JUDGE_STOP_WATCH_H_
#define _INCLUDED_SIDE_ACTION_JUDGE_STOP_WATCH_H_

#include "side_action_judge_base.h"

class SideActionJudgeStopWatch : public SideActionJudgeBase
{
public:
    
    typedef SideActionJudgeStopWatch Self;
    typedef SideActionJudgeBase Super;

public:

    // コンストラクタ
    SideActionJudgeStopWatch();

    // デストラクタ
    virtual ~SideActionJudgeStopWatch();

public:

    // セットアップ
    virtual void Setup( const CrActorFactory::UserData& userData ) TB_OVERRIDE;

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param ) TB_OVERRIDE;

private:

    struct RankInfo
    {
        RankInfo()
            : playerIndex(-1)
            , time(TB_UINT32_MAX)
            , finishTimer(0.0f)
        {
        }
        TbSint32  playerIndex;
        TbUint32  time;
        TbFloat32 finishTimer;
    };

private:

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    CrActionActorSpriteHooker m_SpriteHooker;
    TbStaticString32          m_ChildName;
    TbFloat32                 m_TargetSec;
    TbFloat32                 m_FinishTimer;
    TbSint32                  m_BestPlayer;
    TbUint32                  m_BestTime;

    TbUint32                  m_RankCount;
    RankInfo                  m_RankInfo[CMN_PLAYER_MAX];

};

#endif
