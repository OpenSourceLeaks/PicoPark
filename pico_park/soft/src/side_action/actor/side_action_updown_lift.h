/*!
 * 上下リフト
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_UPDOWN_LIFT_H_
#define _INCLUDED_SIDE_ACTION_UPDOWN_LIFT_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "root.h"

class SideActionUpDownLift : public CrActionActor
{
public:

    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionUpDownLift();

    // デストラクタ
    virtual ~SideActionUpDownLift();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        TbSint32 height = userData.AsSint32(0);
        if (userData.AsSint32(1) != 0) {
            height += userData.AsSint32(1) * Root::GetInstance().GetPlayerCount();
        }
        if ( 0.0f < userData.AsFloat32(2)) {
            m_Speed = userData.AsFloat32(2);
        }
        if (height != 0) {
            SetHeight(static_cast<TbFloat32>(height));
        }
    }

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
    TbFloat32           m_Speed;

};

#endif
