/*!
 * テコリス
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ENDLESS_TETRIS_H_
#define _INCLUDED_SIDE_ENDLESS_TETRIS_H_

#include "side_action_tetris_base.h"
#include "side_action_score_base.h"

class SideActionEndlessTetris : public SideActionTetrisBase
{
public:
    
    typedef SideActionEndlessTetris Self;
    typedef SideActionTetrisBase    Super;

public:

    // コンストラクタ
    SideActionEndlessTetris();

    // デストラクタ
    virtual ~SideActionEndlessTetris();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

protected:

    // ゲームオーバー処理
    virtual void OnGameOver() TB_OVERRIDE;

    // クリアー処理
    virtual void OnClear() TB_OVERRIDE {};

    // シーンに入った
    virtual void OnEnter(void* userData) TB_OVERRIDE;

    // シーンに入った
    virtual void OnExit() TB_OVERRIDE;


private:

    SideActionScoreBase*    m_Score;

};

#endif
