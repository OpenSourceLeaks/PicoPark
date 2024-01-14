/*!
 * テコリス
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TETRIS_H_
#define _INCLUDED_SIDE_ACTION_TETRIS_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"
#include "side_action/actor/side_action_player_input.h"
#include "puzzle/stage/puzzle_tetris_stage.h"

class SideActionTetris : public ActionActor
{
public:
    
    typedef SideActionTetris Self;
    typedef ActionActor      Super;

public:

    // コンストラクタ
    SideActionTetris();

    // デストラクタ
    virtual ~SideActionTetris();

public:

    // 初期化
    TbResult InitializeByScript( const char* nodePath );

    // ノルマ設定
    void SetNorma( TbSint32 count ) {
        m_Norma = count;
    }

    // スイッチターゲット設定
    void SetSwitchTarget( const char* target ) {
        if( target ){
            m_SwitchTarget = target;
        }
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // 描画更新
    virtual void OnDraw( TbFloat32 deltatime ) TB_OVERRIDE;

private:

    // シーンに入った
    virtual void OnEnter( void* userData ) TB_OVERRIDE;

    // シーンに入った
    virtual void OnExit() TB_OVERRIDE;

private:

    enum
    {
        FLAG_CLEAR , 
    };

private:

    TbBitArray32        m_BitArray;
    TbSint32            m_Norma;
    PuzzleTetrisStage*  m_PuzzleStage;
    TbStaticString64    m_NodePath;
    TbStaticString64    m_SwitchTarget;

};

#endif
