/*!
 * 飛行状態なプレイヤーステート
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_STATE_PLANE_H_
#define _INCLUDED_ACTION_PLAYER_STATE_PLANE_H_

#include "side_action_player_state.h"

class SideActionPlayerStatePlane : public SideActionPlayerState
{
public:

    typedef SideActionPlayerState Super;

public:

    // コンストラクタ
    SideActionPlayerStatePlane();

    // デストラクタ
    virtual ~SideActionPlayerStatePlane();

public:

    // 初期化
    virtual void Initialize(SideActionPlayer& player) TB_OVERRIDE;

    // 更新
    virtual void Update( SideActionPlayer& player , TbFloat32 deltatime );

    // 後処理
    virtual void Post( SideActionPlayer& player , TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(SideActionPlayer& player, TbUint32 event , void* param );

private:

    enum Flag
    {
        FLAG_REQUEST_VISIBLE,
        FLAG_MAX
    }; 

private:

    TbBitArray32        m_BitArray;
    TbVector2           m_RequestedPos;
};

#endif
