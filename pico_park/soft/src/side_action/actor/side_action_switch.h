/*!
 * バウンドボール
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_SWITCH_H_
#define _INCLUDED_SIDE_ACTION_SWITCH_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

// スイッチ
class SideActionSwitch : public CrActionActor
{
public:
    
    typedef SideActionSwitch Self;
    typedef CrActionActor Super;

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_REVERSE , // 逆さ
        TYPE_PHYSICS ,
        TYPE_MAX
    };

    enum Color
    {
        COLOR_RED , 
        COLOR_BLUE , 
        COLOR_MAX
    };

public:

    // コンストラクタ
    SideActionSwitch( Type type , Color color = COLOR_RED );

    // デストラクタ
    virtual ~SideActionSwitch();

public:

    // ID設定
    void SetId( TbSint32 id ) {
        m_Id = id;
    }

    // COLOR設定
    void SetColor( Color color );

    // 繰り返し処理可能かどうか
    void SetEnableRepeat( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_REPEAT,isEnable);
    }

    // 睡眠リクエスト可能
    void SetEnableRequestSleep() {
        m_BitArray.SetBit(FLAG_ENABLE_SLEEP);
    }

    // オン
    TbBool IsPress() const {
        return m_BitArray.Test(FLAG_ON) && !m_BitArray.Test(FLAG_ON_PRE);
    }

    // オン
    TbBool IsOn() const {
        return m_BitArray.Test(FLAG_ON);
    }

    // オフリクエスト
    void RequestOff() {
        m_BitArray.SetBit(FLAG_REQ_OFF);
    }

    // サブターゲット設定
    void SetSubTarget(const char* target) {
        m_SubTarget = target;
    }

    // 対象への通知を手動にする
    void SetManualNotifyTarget() {
        m_BitArray.SetBit(FLAG_MANUAL_NOTIFY_TARGET);
    }

    // 対象へイベント通知
    void NotifyTarget( TbBool isOn );

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

protected:

    // スイッチ状態変化
    virtual void OnChangeSwitchState( TbBool isOn ) {}

protected:

    // 踏んでるコリジョン取得
    CrActionCollision* GetContactCollision() const {
        return m_ContactTarget;
    }

private:

    enum Flag
    {
        FLAG_ON_PRE ,       // 前フレーム押している
        FLAG_ON ,           // 押している
        FLAG_REQ_OFF     ,  // スイッチ押してるのリセット
        FLAG_TRIGGER_OFF    ,  // オフした
        FLAG_REQ_REBOUND ,  // 跳ね返りリクエスト
        FLAG_REBOUND ,      // 跳ね返る
        FLAG_ENABLE_REPEAT ,// 繰り返し押せる
        FLAG_ENABLE_SLEEP , // 押したキャラが寝る
        FLAG_MANUAL_NOTIFY_TARGET ,
        FLAG_MAX
    };

private:

    // 衝突時コールバック
    void onCollided( CrActionCollision* );

    // イベント通知
    void changeSwitchState( TbBool isOn );

private:

    Type                m_Type;
    TbSint32            m_Id;
    Color               m_Color;
    TbBitArray32        m_BitArray;
    TbStaticString32    m_SubTarget;
    CrActionCollision*  m_ContactTarget;

};

// スイッチ
class SideActionWakeSwitch : public SideActionSwitch
{
public:

    typedef SideActionWakeSwitch Self;
    typedef SideActionSwitch Super;

public:

    // コンストラクタ
    SideActionWakeSwitch()
        : Super(TYPE_NORMAL)
    {}

    // デストラクタ
    virtual ~SideActionWakeSwitch()
    {}

protected:

    // スイッチ状態変化
    virtual void OnChangeSwitchState(TbBool isOn) TB_OVERRIDE;

};

// ジャンプスイッチ
class SideActionJumpSwitch : public SideActionSwitch
{
public:

    typedef SideActionJumpSwitch Self;
    typedef SideActionSwitch Super;

public:

    // コンストラクタ
    SideActionJumpSwitch()
        : Super(TYPE_NORMAL)
    {
        SetEnableRepeat(TB_TRUE);
    }

    // デストラクタ
    virtual ~SideActionJumpSwitch()
    {}

protected:

    // サブターゲット設定
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        m_JumpVelo.Set( userData.AsFloat32(0) , userData.AsFloat32(1) );
    }

    // スイッチ状態変化
    virtual void OnChangeSwitchState(TbBool isOn) TB_OVERRIDE;

private:

    TbVector2   m_JumpVelo;

};

// 拡大縮小スイッチ
class SideActionScaleSwitch : public SideActionSwitch
{
public:

    typedef SideActionScaleSwitch Self;
    typedef SideActionSwitch Super;

public:

    // コンストラクタ
    SideActionScaleSwitch()
        : Super(TYPE_NORMAL)
    {
        SetEnableRepeat(TB_TRUE);
        SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);

        AddComponent(&m_SpriteHooker);
        m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this, &Self::onDrawText));
        m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_BACK_1);
    }

    // デストラクタ
    virtual ~SideActionScaleSwitch()
    {}

protected:

    // サブターゲット設定
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        m_ScaleVelo = userData.AsFloat32(0);
#if 0
        if (0.0f < m_ScaleVelo) {
            SetColor(COLOR_BLUE);
        }else {
            SetColor(COLOR_RED);
        }
#endif
    }

    // 定期更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // スイッチ状態変化
    virtual void OnChangeSwitchState(TbBool isOn) TB_OVERRIDE;

private:

    // 前景描画
    void onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

private:

    TbFloat32                   m_ScaleVelo;
    CrActionActorSpriteHooker   m_SpriteHooker;

};


// 効果発動が時間差スイッチ
class SideActionDelaySwitch : public SideActionSwitch
{
public:

    typedef SideActionDelaySwitch Self;
    typedef SideActionSwitch Super;

public:

    // コンストラクタ
    SideActionDelaySwitch();

    // デストラクタ
    virtual ~SideActionDelaySwitch()
    {}

protected:

    // サブターゲット設定
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

    // 定期更新
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

    // スイッチ状態変化
    virtual void OnChangeSwitchState(TbBool isOn) TB_OVERRIDE;

private:

    // 前景描画
    void onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

private:

    TbFloat32                   m_RestSec;
    TbFloat32                   m_DelaySec;
    CrActionActorSpriteHooker   m_SpriteHooker;

};

// スイッチ
class SideActionSwitchMediator : public CrActionActor
{
public:

    typedef SideActionSwitchMediator Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionSwitchMediator() : m_Counter(0){}

    // デストラクタ
    virtual ~SideActionSwitchMediator(){ }

public:

    // サブターゲット設定
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        m_Target = userData.AsString(0);
    }

public:

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param) TB_OVERRIDE;

private:

    TbStaticString32    m_Target;
    TbUint32            m_Counter;

};

#endif
