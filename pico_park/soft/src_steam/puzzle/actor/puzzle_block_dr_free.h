/*!
 * パズルのブロック
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTOR_PUZZLE_BLOCK_DR_FREE_H_
#define _INCLUDED_ACTOR_PUZZLE_BLOCK_DR_FREE_H_

#include "puzzle/actor/puzzle_block_base.h"

class PuzzleBlockDr;

class PuzzleBlockDrFree : public PuzzleBlockBase
{
public:

    typedef PuzzleBlockBase Super;
    typedef PuzzleBlockDrFree Self;

public:

    // コンストラクタ
    PuzzleBlockDrFree( const PuzzleBlockDr* parent );

    // デストラクタ
    virtual ~PuzzleBlockDrFree();

public:

    // 終了更新
    virtual void OnUpdateEnd();

    // 再生成
    virtual void OnHitFloor();

    // マップ上のブロックの何かが削除された場合
    virtual void OnEraseMapBlock();

};

#endif
