/*!
 * バウンドボール
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_SWITCH_H_
#define _INCLUDED_SIDE_ACTION_SWITCH_H_

#include "common/actor/action_actor.h"

// スイッチ
class SideActionSwitch : public ActionActor
{
public:
    
    typedef SideActionSwitch Self;
    typedef ActionActor Super;

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_REVERSE , // 逆さ
        TYPE_PHYSICS ,
        TYPE_MAX
    };

public:

    // コンストラクタ
    SideActionSwitch( Type type );

    // デストラクタ
    virtual ~SideActionSwitch();

public:

    // ID設定
    void SetId( TbSint32 id ) {
        m_Id = id;
    }

    // 繰り返し処理可能かどうか
    void SetEnableRepeat( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_REPEAT,isEnable);
    }

    // 睡眠リクエスト可能
    void SetEnableRequestSleep() {
        m_BitArray.SetBit(FLAG_ENABLE_SLEEP);
    }

    // オン
    TbBool IsOn() const {
        return m_BitArray.Test(FLAG_ON);
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

protected:

    // スイッチ状態変化
    virtual void OnChangeSwitchState( TbBool isOn ) {}

private:

    enum Flag
    {
        FLAG_ON_PRE ,       // 前フレーム押している
        FLAG_ON ,           // 押している
        FLAG_REQ_REBOUND ,  // 跳ね返りリクエスト
        FLAG_REBOUND ,      // 跳ね返る
        FLAG_ENABLE_REPEAT ,// 繰り返し押せる
        FLAG_ENABLE_SLEEP , // 押したキャラが寝る
        FLAG_MAX
    };

private:

    // 衝突時コールバック
    void onCollided( ActionCollision* );

    // イベント通知
    void changeSwitchState( TbBool isOn );

private:

    Type            m_Type;
    TbSint32        m_Id;
    TbBitArray32    m_BitArray;

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


#endif
