/*!
 * アクタークラス
 * @author Miyake Shunsuke
 * @since 2013.03.20
 */

#ifndef _INCLUDED_TB_ACTOR_COMPONENT_H_
#define _INCLUDED_TB_ACTOR_COMPONENT_H_

#include <base/math/tb_vector4.h>
#include <base/math/tb_vector2.h>
#include <base/string/tb_static_string.h>

namespace toybox
{

class TbActorScene;

class TbActorComponent
{
public:

    // デストラクタ
    virtual ~TbActorComponent() {}

public:

    // ロジック更新
    virtual void OnUpdate( const TbActor* actor , TbFloat32 deltatime ) {}
    
    // 描画更新
    virtual void OnDraw( const TbActor* actor , TbFloat32 deltatime ) {}

    // シーンに登録された
    virtual void OnEnter( const TbActor* actor , void* userData ) {}

    // シーンから出た
    virtual void OnExit( const TbActor* actor ) {}

    // 処理中断
    virtual void OnSuspend( const TbActor* actor ) {}

    // 処理復帰
    virtual void OnResume( const TbActor* actor ) {}

    // 描画中断
    virtual void OnSuspendDraw( const TbActor* actor ) {}

    // 描画復帰
    virtual void OnResumeDraw( const TbActor* actor ) {}

};

}

#endif
