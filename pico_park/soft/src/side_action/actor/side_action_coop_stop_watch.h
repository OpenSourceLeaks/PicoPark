/*!
 * 協力型ストップウォッチ
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_COOP_STOP_WATCH_H_
#define _INCLUDED_SIDE_ACTION_COOP_STOP_WATCH_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionCoopStopWatch : public CrActionActor
{
public:
    
    typedef SideActionCoopStopWatch Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionCoopStopWatch();

    // デストラクタ
    virtual ~SideActionCoopStopWatch();

public:

    // 時間を取得
    TbFloat32 GetSec() const {
        return m_Sec;
    }

    // ジャッジ秒設定
    void SetJudgeSec( TbFloat32 min , TbFloat32 max ){
        m_JudgeSecMin = min;
        m_JudgeSecMax = max;
    }

public:

    // シーンに入った
    virtual void OnEnter( void* userData ) {
        Super::OnEnter(userData);
        m_BitArray.SetBit(FLAG_UPDATE);
    }

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // ロジック更新 (派生クラスはこちらを継承するように)
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum
    {
        FLAG_UPDATE ,
        FLAG_END , 
        FLAG_FAILED , 
        FLAG_MAX
    };

private:

    // 前景描画
    void onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // 達成
    TbBool isCleared() const {
        return m_JudgeSecMin < m_Sec && m_Sec < m_JudgeSecMax && !m_BitArray.Test(FLAG_FAILED);
    }

private:

    TbFloat32               m_Sec;
    TbFloat32               m_InitSec;
    TbFloat32               m_TotalSec;

    TbFloat32               m_JudgeSecMax;
    TbFloat32               m_JudgeSecMin;
    TbFloat32               m_RetryTimerSec;

    TbBitArray32            m_BitArray;
    CrActionActorSpriteHooker m_SpriteHooker;

};

#endif
