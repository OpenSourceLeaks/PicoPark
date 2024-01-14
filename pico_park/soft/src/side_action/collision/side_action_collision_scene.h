/*!
 * アクション用コリジョンシーン
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_COLLISION_SCENE_H_
#define _INCLUDED_SIDE_ACTION_COLLISION_SCENE_H_

#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_set.h>
#include <crayon/collision/cr_action_collision_scene.h>

class SideActionCollisionScene : public CrActionCollisionScene
{
public:
    typedef SideActionCollisionScene Self;
    typedef CrActionCollisionScene Super;
public:
    
    // コンストラクタ
    SideActionCollisionScene();

    // デストラクタ
    virtual ~SideActionCollisionScene();

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // 慣性有効か
    void SetEnableInertia(TbBool isEnable) {
        m_IsEnableInertia = isEnable;
    }

    // 慣性有効か
    TbBool IsEnableInertia() const {
        return m_IsEnableInertia;
    }

protected:

    // 押し戻し後処理
    virtual void OnPostPushBack();

private:

    typedef toybox::TbStaticSet<CrActionCollision*, 100> CollisionSet;

private:

    // 慣性移動更新
    void updateInertia( CrActionCollision* co , TbBool isEnableCheckDown , CollisionSet* set );

    // プレイヤー位置を確定
    void fixPlayerPos();

private:

    TbBool m_IsEnableInertia;

};

#endif
