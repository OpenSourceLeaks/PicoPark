/*!
 * シンプルな矩形
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_RECT_H_
#define _INCLUDED_SIDE_ACTION_RECT_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionRect : public CrActionActor
{
public:
    
    typedef SideActionRect Self;
    typedef CrActionActor Super;

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_SWITCH ,
        TYPE_DAMAGE ,
        TYPE_MAX
    };

public:

    // コンストラクタ
    SideActionRect( const TbRectF32& rect );

    // デストラクタ
    virtual ~SideActionRect();

public:

    // シーンに入った
    virtual void OnEnter(void* userData);

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

public:

    // タイプ設定
    void SetType( Type type );

private:

    Type                        m_Type;
    TbUint32                    m_RefCounter;
    CrActionActorElasticSprite  m_Sprite;
    TbRectF32                   m_Rect;

};

#endif
