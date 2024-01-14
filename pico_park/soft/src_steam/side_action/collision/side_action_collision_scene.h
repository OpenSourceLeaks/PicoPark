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
#include "common/collision/action_collision_scene.h"

class SideActionCollisionScene : public ActionCollisionScene
{
public:
    typedef SideActionCollisionScene Self;
    typedef ActionCollisionScene Super;
public:
    
    // コンストラクタ
    SideActionCollisionScene();

    // デストラクタ
    virtual ~SideActionCollisionScene();

protected:

    // 押し戻し後処理
    virtual void OnPostPushBack();

private:

    // 慣性移動更新
    void updateInertia( ActionCollision* co , TbBool isEnableCheckDown );

};

#endif
