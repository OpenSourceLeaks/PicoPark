/*!
 * バトル用ストップウォッチ判定
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_SCORE_STOP_WATCH_H_
#define _INCLUDED_SIDE_ACTION_SCORe_STOP_WATCH_H_

#include "side_action_score_base.h"

class SideActionScoreStopWatch : public SideActionScoreBase
{
public:
    
    typedef SideActionScoreStopWatch Self;
    typedef SideActionScoreBase Super;

public:

    // コンストラクタ
    SideActionScoreStopWatch();

    // デストラクタ
    virtual ~SideActionScoreStopWatch();

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    CrActionActorSpriteHooker m_SpriteHooker;
    TbFloat32                 m_TotalSec;
    TbBool                    m_IsFailed;
    TbFloat32                 m_FinishTimer;

};

#endif
