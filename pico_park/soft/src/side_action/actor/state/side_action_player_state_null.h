/*!
 * クリア用プレイヤーステート
 * @author teco
 */

#ifndef _INCLUDED_ACTION_PLAYER_STATE_NULL_H_
#define _INCLUDED_ACTION_PLAYER_STATE_NULL_H_

#include "side_action_player_state.h"

class SideActionPlayerStateNull : public SideActionPlayerState
{
public:

    typedef SideActionPlayerState Super;

public:

    // コンストラクタ
    SideActionPlayerStateNull();

    // デストラクタ
    virtual ~SideActionPlayerStateNull();

public:
    
    // 初期化
    virtual void Initialize(SideActionPlayer& player) TB_OVERRIDE;

    // 後始末
    virtual void Finalize(SideActionPlayer& player) TB_OVERRIDE;

    // 更新
    virtual void Update(SideActionPlayer& player, TbFloat32 deltatime) TB_OVERRIDE
    {
    }

};

#endif
