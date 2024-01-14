/*!
 * 触れると何か起きる系
 * @author teco
 * @since 2015.01.25
 */

#ifndef _INCLUDED_SIDE_ACTION_COLLISION_SWITCH_H_
#define _INCLUDED_SIDE_ACTION_COLLISION_SWITCH_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

// 触れるとスイッチの役割を果たす
class SideActionCollisionSwitch : public CrActionActor
{
public:
    
    typedef SideActionCollisionSwitch Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionCollisionSwitch( TbFloat32 width , TbFloat32 height );

    // デストラクタ
    virtual ~SideActionCollisionSwitch();

public:

    // ターゲット設定
    void SetTagetName( const char* name ) {
        m_TargetName = name;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 描画
    virtual void OnDraw( TbFloat32 deltatime ) TB_OVERRIDE;

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

private:

    TbBool              m_IsEnd;
    TbStaticString64    m_TargetName;

};

// 触れるとプレイヤーが変化する
class SideActionCollisionChangePlayer : public CrActionActor
{
public:

    typedef SideActionCollisionChangePlayer Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionCollisionChangePlayer();

    // デストラクタ
    virtual ~SideActionCollisionChangePlayer();

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

private:

    TbBool              m_IsEnd;
    TbBool              m_IsEnableRepeat;
    TbBool              m_IsOn;

};

// 触れるとアクター生成
class SideActionCollisionActorCreator : public CrActionActor
{
public:

    typedef SideActionCollisionActorCreator Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionCollisionActorCreator();

    // デストラクタ
    virtual ~SideActionCollisionActorCreator();

public:
   
    // サイレントモード
    void SetSilent() {
        m_IsSilent = TB_TRUE;
    }

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

private:

    TbBool                          m_IsEnd;
    TbBool                          m_IsSilent;
    CrActorFactory::CreateParam     m_CreateParam;

};

// 触れるとアクター生成
class SideActionCollisionConstraintMove : public CrActionActor
{
public:

    typedef SideActionCollisionConstraintMove Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionCollisionConstraintMove();

    // デストラクタ
    virtual ~SideActionCollisionConstraintMove();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

private:

    TbStaticString64    m_Target;

};

#endif
