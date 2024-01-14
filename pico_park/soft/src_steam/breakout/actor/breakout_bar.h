/*!
 * ブロック崩し用バー
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_BREAKOUT_BAR_H_
#define _INCLUDED_BREAKOUT_BAR_H_

#include "common/actor/action_actor.h"
#include "system/sys_input.h"

class BreakoutBar : public ActionActor
{
public:
    
    typedef BreakoutBar Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    BreakoutBar( TbUint32 playerIndex );

    // デストラクタ
    virtual ~BreakoutBar();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 入力し続けている
    TbBool isOnInput( SysInput::Button button ) const;

    // 押した
    TbBool isPressInput( SysInput::Button button ) const;

    // 離した
    TbBool isReleaseInput( SysInput::Button button ) const;

private:

    TbSpriteNodeTree m_SpriteNode;
    TbUint32         m_PlayerIndex;

};

#endif
