/*!
 * ブロック崩しステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_BREAKOUT_STAGE_H_
#define _INCLUDED_BREAKOUT_STAGE_H_

#include "common/stage/action_stage.h"
#include "common/map/action_map.h"
#include "common/collision/action_collision_scene.h"
#include <script/lua/tb_script_lua.h>
#include <base/math/tb_geometry.h>

class Actor;

class BreakoutStage : public ActionStage
{
    typedef BreakoutStage Self;
    typedef ActionStage Super;
public:

    // コンストラクタ
    BreakoutStage( TbScriptLuaNode* node );

    // デストラクタ
    virtual ~BreakoutStage();

protected:

    // 更新
    virtual void OnUpdate( TbFloat32 deltatime );

    // アクター生成
    virtual void OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData& userData);

private:

    // ステージクリアしたか
    TbBool isCleared() const;

    // ゲームオーバー
    TbBool isGameOver() const;

private:

    TbUint32 m_BarCount;
    
};

#endif
