/*!
 * アクション用コンポーネント
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_ACTION_ACTOR_COMPONENT_H_
#define _INCLUDED_CR_ACTION_ACTOR_COMPONENT_H_

#include <base/math/tb_vector2.h>
#include <crayon//stage/cr_action_stage.h>

namespace crayon
{

class CrActionActorComponent
{
public:
    CrActionActorComponent() : m_Owner(nullptr) {}
    virtual ~CrActionActorComponent() {}
private:
    TB_DISALLOW_COPY_AND_ASSIGN(CrActionActorComponent);
public:
    // オーナー設定
    void SetOwner( CrActionActor* owner ) {
        m_Owner = owner;
    }
    // オーナー取得
    CrActionActor* GetOwner() {
        return m_Owner;
    }
    // オーナー取得
    const CrActionActor* GetOwner() const {
        return m_Owner;
    }
public:
    // 位置更新
    virtual void OnUpdatePos( const toybox::TbVector2& pos , const toybox::TbVector2& drawPos ) {}
    // スケール更新
    virtual void OnUpdateScale( TbFloat32 scaleX , TbFloat32 scaleY ) {}
    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {}
    // シーンから出た
    virtual void OnExit() {}
    // 速度が設定された
    virtual void OnSetVelo( toybox::TbVector2* velo ) {}
    // 表示変更
    virtual void OnChangedVisible( TbBool isVisible ) {}
private:
    CrActionActor*    m_Owner;    // オーナー
};

}

#endif
