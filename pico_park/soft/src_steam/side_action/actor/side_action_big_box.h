/*!
 * 協力しないと押せない箱
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_BIG_BOX_H_
#define _INCLUDED_SIDE_ACTION_BIG_BOX_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"
#include "common/stage/action_stage.h"
#include <base/util/tb_dynamic_cast.h>

class SideActionBigBox : public ActionActor
{
public:

    typedef ActionActor Super;
    typedef SideActionBigBox Self;

public:

    enum Type
    {
        TYPE_TALL , 
        TYPE_BIG , 
        TYPE_NORMAL , 
        TYPE_SMALL ,
        TYPE_MAX
    };

public:

    // コンストラクタ
    SideActionBigBox( Type type = TYPE_NORMAL );

    // コンストラクタ
    SideActionBigBox( const TbRectF32Pod& rect );

    // デストラクタ
    virtual ~SideActionBigBox();

public:

    // 最大押し数
    void SetPushCountMax( TbUint32 parcent );

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);
    
private:

    // 初期化
    void initialize(const TbRectF32Pod& rect);

    // 任意の方向から押されているか
    TbBool isPushed( CmnContactFace type );

    // 押している数
    TbUint32 getPushCount( ActionActor* actor , CmnContactFace type );

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

public:
    
    TbUint32                 m_PushCountMax;
    TbUint32                 m_PushCount;
    ActionActorElasticSprite m_Sprite;
    ActionActorSpriteHooker  m_SpriteHooker;
    TbRectF32Pod             m_Rect;

};

#endif
