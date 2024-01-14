/*!
 * リズムボール
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_RHYTHM_BALL_H_
#define _INCLUDED_SIDE_ACTION_RHYTHM_BALL_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/actor.h"

class SideActionRhythmBall : public Actor
{
public:

    typedef Actor Super;

public:

    // コンストラクタ
    SideActionRhythmBall();

    // デストラクタ
    virtual ~SideActionRhythmBall();

public:

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime );
    
    // 描画更新
    virtual void OnDraw( TbFloat32 deltatime );

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

private: 

    TbVector4 m_OffsetPos;
    TbFloat32 m_Timer;

private: // 表示系

    TbSprite         m_Sprite;
    TbSpriteNodeTree m_SpriteNode;

};

#endif
