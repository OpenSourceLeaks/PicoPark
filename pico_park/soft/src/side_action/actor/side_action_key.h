/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_KEY_H_
#define _INCLUDED_SIDE_ACTION_KEY_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionKey : public CrActionActor
{
public:
    
    typedef SideActionKey Self;
    typedef CrActionActor Super;

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_BREAKOUT
    };

public:

    // コンストラクタ
    SideActionKey( Type type = TYPE_NORMAL );

    // デストラクタ
    virtual ~SideActionKey();

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        enum
        {
            PARAM_OFFSET_X ,
            PARAM_OFFSET_Y ,
            PARAM_INACTIVE ,
            PARAM_SWITCH_TARGET,
            PARAM_ATTACH_OFFST_Y
        };
        if (m_Type == TYPE_NORMAL)
        {
            if (userData.AsSint32(PARAM_INACTIVE) > 0) {
                SetActive(TB_FALSE);
            }
            if (userData.AsString(PARAM_SWITCH_TARGET)) {
                SetSwitchTarget(userData.AsString(3));
            }
            if (PARAM_ATTACH_OFFST_Y < userData.GetCount()) {
                m_AttachOffsetY = userData.AsFloat32(PARAM_ATTACH_OFFST_Y);
            }
        }
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

public:

    // 初期位置計算
    TbVector2 CalcInitPos( const TbVector2& pos , const TbVector2& offset);

    // アクティブ設定
    void SetActive( TbBool isActive );

    // アクティブ設定取得
    TbBool IsActive() const {
        return m_BitArray.Test(FLAG_ACTIVE);
    }

    // アタッチプレイヤー番号取得
    TbBool IsPlayerAttached() const {
        return m_AttachTarget != nullptr;
    }

    // スイッチターゲット
    void SetSwitchTarget( const char* target ) {
        m_SwitchTarget = target;
    }

private:

    enum Flag
    {
        FLAG_ACTIVE , 
        FLAG_MAX
    };

    enum State
    {
        STATE_READY , 
        STATE_RETURN,
        STATE_REQ_END ,
        STATE_END ,
    };

    struct AttachTargetExclude
    {
        void Retain()
        {
            refCount = 2;
        }
        void Release()
        {
            if (0 < refCount) {
                --refCount;
                if (0 == refCount)
                {
                    target = nullptr;
                }
            }else {
                target = nullptr;
            }
        }
        TbUint32        refCount;
        CrActionActor*  target;
    };

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(CrActionCollision* target);

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // 壊せるブロックが残っているか
    TbBool existsBreakBlock();

public:

    Type             m_Type;
    State            m_State;
    TbBitArray32     m_BitArray;
    CrActionActor*   m_AttachTarget;
    CrActionActor*   m_AttachTargetTrigger;
    AttachTargetExclude m_AttachTargetExclude;
    TbRectF32        m_AttachRect;
    TbFloat32        m_AttachOffsetY;
    TbStaticString64 m_SwitchTarget;

    CrActionActorSpriteHooker  m_SpriteHooker;

};

#endif
