/*!
 * 上下リフト
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_UPDOWN_LIFT_H_
#define _INCLUDED_SIDE_ACTION_UPDOWN_LIFT_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionUpDownLift : public ActionActor
{
public:

    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionUpDownLift();

    // デストラクタ
    virtual ~SideActionUpDownLift();

public:

    // 高さ設定
    void SetHeight( TbFloat32 height ) {
        m_Height = height;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

protected:

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {
        m_OffsetPos = pos - GetInitPos();
        m_BitArray.SetBit(FLAG_PUSH_BACK);
    }

private:

    enum
    {
        FLAG_PUSH_BACK
    };

private:

    TbBitArray32        m_BitArray;
    TbVector2           m_OffsetPos;
    TbFloat32           m_OldTimer;
    TbFloat32           m_Timer;
    TbFloat32           m_PushBackTimer;
    TbFloat32           m_PushBackLimitTime;
    TbFloat32           m_Height;

};

#endif
