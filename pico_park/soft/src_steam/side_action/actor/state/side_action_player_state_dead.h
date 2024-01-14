/*!
 * タイトル画面用プレイヤーステート
 * @author teco
 * @since 2013.10.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_STATE_DEAD_H_
#define _INCLUDED_ACTION_PLAYER_STATE_DEAD_H_

#include "side_action_player_state.h"

class SideActionPlayerStateDead : public SideActionPlayerState
{
public:

    typedef SideActionPlayerState Super;

public:

    // コンストラクタ
    SideActionPlayerStateDead();

    // デストラクタ
    virtual ~SideActionPlayerStateDead();

public:
    
    // 更新
    virtual void Update( SideActionPlayer& player , TbFloat32 deltatime );

private:

    enum State
    {
        STATE_INIT , 
        STATE_WAIT , 
        STATE_DEAD , 
    };

private:

    State       m_State;
    TbFloat32   m_WaitTimer;

};

#endif
