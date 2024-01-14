/*!
 * ブロック崩しステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_BREAKOUT_STAGE_H_
#define _INCLUDED_BREAKOUT_STAGE_H_


#include <script/lua/tb_script_lua.h>
#include <base/math/tb_geometry.h>

#include <crayon/stage/cr_action_stage.h>
#include <crayon/map/cr_action_map.h>
#include <crayon/collision/cr_action_collision_scene.h>

class BreakoutStage : public CrActionStage
{
    typedef BreakoutStage Self;
    typedef CrActionStage Super;
public:

    // コンストラクタ
    BreakoutStage( TbScriptLuaNode* node );

    // デストラクタ
    virtual ~BreakoutStage();

protected:

    // 更新
    virtual void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

    // アクター生成
    virtual CrActor* OnCreatedActor( const CrActorFactory::CreateParam& param ) TB_OVERRIDE;

private:

    // ステージクリアしたか
    TbBool isCleared() const;

    // ゲームオーバー
    TbBool isGameOver() const;

private:

    TbUint32 m_BarCount;
    
};

#endif
