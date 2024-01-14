/*!
 * パズルのブロック
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTOR_PUZZLE_BLOCK_DR_H_
#define _INCLUDED_ACTOR_PUZZLE_BLOCK_DR_H_

#include "puzzle/actor/puzzle_block_base.h"

class PuzzleBlockDr : public PuzzleBlockBase
{
public:

    typedef PuzzleBlockBase Super;
    typedef PuzzleBlockDr Self;

public:

    // コンストラクタ
    PuzzleBlockDr( TbSint32 playerIndex );

    // デストラクタ
    virtual ~PuzzleBlockDr();

public:

    // 再生成
    virtual void OnHitFloor();

    // 終了更新
    virtual void OnUpdateEnd();

private:

    // ブロック更新
    void updateNewBlock();

};

#endif
