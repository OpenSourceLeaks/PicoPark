/*!
 * 上に載ると落下するブロック
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_FALL_BOX_H_
#define _INCLUDED_SIDE_ACTION_FALL_BOX_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include <crayon/stage/cr_action_stage.h>
#include <base/util/tb_dynamic_cast.h>

class SideActionFallBox : public CrActionActor
{
public:

    typedef CrActionActor      Super;
    typedef SideActionFallBox  Self;

public:

    // コンストラクタ
    SideActionFallBox();

    // デストラクタ
    virtual ~SideActionFallBox();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

private:

    enum
    {
        FLAG_FALL , 
        FLAG_MAX
    };
    
private:

    // 初期化
    void initialize(const TbRectF32& rect);

    // 接触開始コールバック
    void onContacted(const TbVector2& normal, CrActionCollision::ContactTarget target, const CrActionCollisionInfo& collision);

private:
    
    CrActionActorElasticSprite  m_Sprite;
    TbRectF32                   m_Rect;
    TbFloat32                   m_FallTimer;
    TbBitArray32                m_BitArray;

};

#endif
