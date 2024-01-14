/*!
 * タイマー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TIEMR_H_
#define _INCLUDED_SIDE_ACTION_TIMER_H_

#include "common/actor/action_actor.h"

class SideActionTimer : public ActionActor
{
public:

    enum Type
    {
        TYPE_UP       , // 時間増える
        TYPE_DOWN     , // 時間下がる
    };

    enum TimeUpAction
    {
        TIMEUP_ACTION_NONE ,
        TIMEUP_ACTION_RETRY ,
        TIMEUP_ACTION_SWITCH 
    };

    // 表示タイプ
    enum DispType
    {
        DISP_TYPE_SEC ,
        DISP_TYPE_MSEC ,
        DISP_TYPE_DEFAULT = DISP_TYPE_SEC
    };

public:
    
    typedef SideActionTimer Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionTimer( Type type = TYPE_UP );

    // デストラクタ
    virtual ~SideActionTimer();

public:

    // タイムアップ持のアクション
    void SetTimeUpAction( TimeUpAction action ){
        m_TimeUpAction = action;
    }

    // 時間を取得
    TbFloat32 GetSec() const {
        return m_Sec;
    }

    // 初期時間設定
    void SetInitSec( TbFloat32 sec ){
        m_InitSec = sec;
        SetSec( sec );
    }

    // 時間設定
    void SetSec( TbFloat32 sec ) {
        TB_RESQUE(sec>0);
        m_Sec = sec;
        m_BitArray.ResetBit(FLAG_TIMEUP);
    }

    // 表示タイプ設定
    void SetDispType( DispType type ) {
        m_DispType = type;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Flag
    {
        FLAG_PAUSE ,
        FLAG_TIMEUP , 
        FLAG_MAX
    };

private:

    // 前景描画
    void onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // タイムアップ
    void onTimeUp();

private:

    Type                    m_Type;
    TbFloat32               m_Sec;
    TbFloat32               m_InitSec;
    TbBitArray32            m_BitArray;
    ActionActorSpriteHooker m_SpriteHooker;
    TimeUpAction            m_TimeUpAction;
    DispType                m_DispType;

};

#endif
