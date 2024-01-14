/*!
 * テコリス
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TETRIS_BASE_H_
#define _INCLUDED_SIDE_ACTION_TETRIS_BASE_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "side_action/actor/side_action_player_input.h"
#include "puzzle/stage/puzzle_tetris_stage.h"

class SideActionTetrisBase : public CrActionActor
{
public:
    
    typedef SideActionTetrisBase    Self;
    typedef CrActionActor           Super;

public:

    // コンストラクタ
    SideActionTetrisBase();

    // デストラクタ
    virtual ~SideActionTetrisBase();

public:

    // 初期化
    TbResult InitializeByScript( const char* nodePath );

    // 再初期化
    TbResult ReinitializeByScript() {
        return InitializeByScript(m_NodePath.GetBuffer());
    }

    // ライン数取得
    TbUint32 GetLineCount() const {
        if(m_PuzzleStage){
            return m_PuzzleStage->GetLineCount();
        }
        return 0;
    }

    // ノルマ設定
    void SetNorma(TbSint32 count) {
        m_Norma = count;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // 描画更新
    virtual void OnDraw( TbFloat32 deltatime ) TB_OVERRIDE;

protected:

    // ゲームオーバー処理
    virtual void OnGameOver() = 0;

    // クリアー処理
    virtual void OnClear() = 0;

protected:

    // シーンに入った
    virtual void OnEnter( void* userData ) TB_OVERRIDE;

    // シーンに入った
    virtual void OnExit() TB_OVERRIDE;

private:

    enum
    {
        FLAG_END,
    };

private:

    TbBitArray32        m_BitArray;
    TbSint32            m_Norma;
    PuzzleTetrisStage*  m_PuzzleStage;
    TbStaticString64    m_NodePath;

};

#endif
