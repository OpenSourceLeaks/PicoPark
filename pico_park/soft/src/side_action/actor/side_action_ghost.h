/*!
 * 幽霊
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_GHOST_H_
#define _INCLUDED_SIDE_ACTION_GHOST_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include <crayon/actor/cr_action_actor_child.h>
#include <crayon/stage/cr_action_stage.h>
#include <base/util/tb_dynamic_cast.h>

#include "side_action/side_action_types.h"
#include "side_action/actor/side_action_key.h"
#include "side_action/actor/side_action_lift.h"
#include "side_action/actor/side_action_switch.h"
#include "root.h"

class SideActionPlayer;

class SideActionGhost : public CrActionActor
{
public:

    typedef CrActionActor Super;
    typedef SideActionGhost Self;

public:

    // コンストラクタ
    SideActionGhost();

    // デストラクタ
    virtual ~SideActionGhost();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // スイッチターゲット設定
    void SetSwitchTarget( const char* targetName );

    // 速度設定
    void SetSpeed( TbFloat32 speed ) {
        m_Speed = speed;
    }

    // 左向き
    virtual void LookLeft() TB_OVERRIDE {
        GetMainSprite().SetScale(-1.0f,1.0f);
    }

    // 左向き
    virtual void LookRight() TB_OVERRIDE {
        GetMainSprite().SetScale(1.0f,1.0f);
    }

    // イベント通知
    TbSint32 NotifyEvent( TbUint32 event , void* param );

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    enum Flag
    {
        FLAG_ACTIVE ,
        FLAG_SWITCH ,
        FLAG_DEAD ,
        FLAG_END
    };
    
private:

    // 自分を見ているプレイヤー数取得
    TbUint32 getLookingPlayerCount( const SideActionPlayer** nearPlayer, 
                                    TbUint32* activePlayerCount ) const;

    // 近くにあるキーを探す
    const SideActionKey*  getNearKey();

    // 有効設定
    void setActive( TbBool isActive );

    // 透明度設定
    void setAlpha( TbFloat32 alpha );

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

private:

    TbBitArray32             m_BitArray;
    TbUint32                 m_LookingPlayerCount;
    TbFloat32                m_BorderRate;
    TbFloat32                m_Sec;
    TbFloat32                m_KeyRange;
    TbFloat32                m_Speed;
    TbFloat32                m_Alpha;                  // 透明度

};

#endif
