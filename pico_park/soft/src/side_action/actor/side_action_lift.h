/*!
 * リフト
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LIFT_H_
#define _INCLUDED_SIDE_ACTION_LIFT_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "root.h"

class SideActionLift : public CrActionActor
{
public:

    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionLift();

    // デストラクタ
    virtual ~SideActionLift();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param) TB_OVERRIDE;

protected:

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {
        m_OffsetPos = pos - GetInitPos();
        m_BitArray.SetBit(FLAG_PUSH_BACK);
    }

private:

    enum
    {
        FLAG_PUSH_BACK , 
        FLAG_ABS ,
        FLAG_ACTIVE ,
        FLAG_SWITCH
    };

private:

    TbBitArray32        m_BitArray;
    TbVector2           m_Move;
    TbVector2           m_OffsetPos;
    TbFloat32           m_OldTimer;
    TbFloat32           m_Timer;
    TbFloat32           m_PushBackTimer;
    TbFloat32           m_PushBackLimitTime;
    TbFloat32           m_Speed;

};

#endif
