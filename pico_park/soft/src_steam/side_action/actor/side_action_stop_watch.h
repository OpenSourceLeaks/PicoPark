/*!
 * ストップウォッチ
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_STOP_WATCH_H_
#define _INCLUDED_SIDE_ACTION_STOP_WATCH_H_

#include "common/actor/action_actor.h"

class SideActionStopWatch : public ActionActor
{

public:
    
    typedef SideActionStopWatch Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionStopWatch();

    // デストラクタ
    virtual ~SideActionStopWatch();

public:

    // 時間を取得
    TbFloat32 GetSec() const {
        return m_Sec;
    }

    // 初期時間設定
    void SetInitSec( TbFloat32 sec ){
        m_InitSec = sec;
        m_Sec = sec;
    }

    // 親の名前設定
    void SetParentName( const char* name ) {
        m_ParentName = name;
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
        FLAG_DEAD ,
        FLAG_MAX
    };

private:

    // 前景描画
    void onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbFloat32               m_Sec;
    TbFloat32               m_InitSec;
    TbFloat32               m_TotalSec;
    TbBitArray32            m_BitArray;
    ActionActorSpriteHooker m_SpriteHooker;
    TbStaticString64        m_ParentName;

};

#endif
