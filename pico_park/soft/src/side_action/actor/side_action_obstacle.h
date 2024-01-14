/*!
 * 障害物
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_OBJSTACLE_H_
#define _INCLUDED_SIDE_ACTION_OBJSTACLE_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "side_action_rect.h"

class SideActionObstacle : public CrActionActor
{
public:
    
    typedef SideActionObstacle Self;
    typedef CrActionActor Super;
    
public:

    // コンストラクタ
    SideActionObstacle();

    // デストラクタ
    virtual ~SideActionObstacle();

public:

    // ロジック更新
    virtual void OnPostAction(TbFloat32 deltatime);


    // イベント通知
    virtual  TbSint32 NotifyEvent(TbUint32 event, void* param);

protected:

    // 矩形生成
    CrActionActor* GenerateRect(const TbRectF32& rect );

    // 矩形生成
    CrActionActor* GenerateRect(const TbRectF32& rect , TbFloat32 moveSpeed);

    // 天井
    CrActionActor* GenerateCeilRect( TbFloat32 x ,TbFloat32 width , TbFloat32 height );

    // 床
    CrActionActor* GenerateFloorRect(TbFloat32 x, TbFloat32 width, TbFloat32 height);

    // 床
    CrActionActor* GenerateBox( TbFloat32 x , TbFloat32 y , TbFloat32 size );

    // 振幅設定
    void SetActorAmplitude( CrActionActor*  actor , TbFloat32 veloAngle , TbFloat32 height );

private:

    struct RectInfo
    {
        RectInfo()
            : isEnableVelo(TB_FALSE)
        {
        }
        CrActionActor* actor;
        TbBool         isEnableVelo;
        TbVector2      velo;
    };

private:

    // 矩形生成
    CrActionActor* generateRect(const TbRectF32& rect);

private:

    TbBitArray32       m_BitArray;
    TbArray<RectInfo>  m_ObstacleList;

};

#endif
