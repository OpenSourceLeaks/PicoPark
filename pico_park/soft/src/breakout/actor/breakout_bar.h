/*!
 * ブロック崩し用バー
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_BREAKOUT_BAR_H_
#define _INCLUDED_BREAKOUT_BAR_H_

#include <crayon/actor/cr_action_actor.h>
#include <crayon/input/cr_input.h>

class BreakoutBar : public CrActionActor
{
public:
    
    typedef BreakoutBar Self;
    typedef CrActionActor Super;

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
    TbBool isOnInput( CrInput::Button button ) const;

    // 押した
    TbBool isPressInput( CrInput::Button button ) const;

    // 離した
    TbBool isReleaseInput( CrInput::Button button ) const;

private:

    TbSpriteNodeTree m_SpriteNode;
    TbUint32         m_PlayerIndex;

};

#endif
