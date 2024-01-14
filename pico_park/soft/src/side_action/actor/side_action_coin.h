﻿/*!
 * コイン
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_COIN_H_
#define _INCLUDED_SIDE_ACTION_COIN_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionCoin : public CrActionActor
{
public:

    typedef SideActionCoin Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionCoin();

    // デストラクタ
    virtual ~SideActionCoin();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

private:

    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim;

};

class SideActionCoinObserver : public CrActionActor
{
public:

    typedef SideActionCoinObserver Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionCoinObserver()
        : Super()
    {
        m_Coins.SetEnableSetCapacityAuto(TB_TRUE);
        m_Coins.SetCapacity(10);
    }

    // デストラクタ
    virtual ~SideActionCoinObserver()
    {
        TbForIterator( ite , m_Coins ) {
            (*ite)->Release();
        }
    }

public:

    // コイン追加
    void AddCoin( SideActionCoin* coin ) {
        if( TB_VERIFY(m_Coins.PushBack(coin)) ){
            coin->Retain();
        }
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    TbArray<SideActionCoin*> m_Coins;

};

#endif