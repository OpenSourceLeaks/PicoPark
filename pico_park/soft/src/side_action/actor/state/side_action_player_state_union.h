/*!
 * プレイヤーアクター
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_H_
#define _INCLUDED_ACTION_PLAYER_H_

#include <graphics/sprite/tb_sprite.h>
#include "side_action/actor/side_action_actor.h"

class SideActionPlayer : public SideActionActor
{
public:

    typedef SideActionActor Super;

public:

    // コンストラクタ
    SideActionPlayer();

    // デストラクタ
    virtual ~SideActionPlayer();

public:

    // プレイヤーインデックス設定
    void SetPlayerIndex( TbUint32 index )
    {
        m_PlayerIndex = index;
    }

    // プレイヤー
    TbUint32 GetPlayerIndex() const {
        return m_PlayerIndex;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 描画更新
    virtual void OnDraw( TbFloat32 deltatime );

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

private:

    TbUint32            m_PlayerIndex;
    TbFloat32           m_JumpVelo;     // ジャンプ初速度

private:

    TbSprite            m_Sprite;
    TbSpriteNodeTree    m_SpriteNode;

};

#endif
