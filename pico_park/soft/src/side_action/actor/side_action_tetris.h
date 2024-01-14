/*!
 * テコリス
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TETRIS_H_
#define _INCLUDED_SIDE_ACTION_TETRIS_H_

#include "side_action_tetris_base.h"

class SideActionTetris : public SideActionTetrisBase
{
public:
    
    typedef SideActionTetris        Self;
    typedef SideActionTetrisBase    Super;

public:

    // コンストラクタ
    SideActionTetris();

    // デストラクタ
    virtual ~SideActionTetris();

public:

    // スイッチターゲット設定
    void SetSwitchTarget( const char* target ) {
        if( target ){
            m_SwitchTarget = target;
        }
    }

protected:

    // ゲームオーバー処理
    virtual void OnGameOver() TB_OVERRIDE;

    // クリアー処理
    virtual void OnClear() TB_OVERRIDE;

private:

    TbStaticString64    m_SwitchTarget;

};

#endif
