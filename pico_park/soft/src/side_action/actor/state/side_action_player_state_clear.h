/*!
 * クリア用プレイヤーステート
 * @author teco
 * @since 2013.10.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_STATE_CLEAR_H_
#define _INCLUDED_ACTION_PLAYER_STATE_CLEAR_H_

#include "side_action_player_state.h"

class SideActionPlayerStateClear : public SideActionPlayerState
{
public:

    typedef SideActionPlayerState Super;

public:

    // コンストラクタ
    SideActionPlayerStateClear();

    // デストラクタ
    virtual ~SideActionPlayerStateClear();

public:
    
    // 初期化
    virtual void Initialize(SideActionPlayer& player) TB_OVERRIDE;

    // 後始末
    virtual void Finalize(SideActionPlayer& player) TB_OVERRIDE;

    // 更新
    virtual void Update( SideActionPlayer& player , TbFloat32 deltatime ) TB_OVERRIDE;

    // 後処理
    virtual void Post( SideActionPlayer& player , TbFloat32 deltatime ) TB_OVERRIDE;

private:

    enum State
    {
        STATE_INIT , 
        STATE_WAIT , 
        STATE_IDLE , 
    };

private:

    State       m_State;
    TbFloat32   m_WaitTimer;

};

#endif
