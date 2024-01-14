/*!
 * パズルのブロック
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "puzzle_block_dr.h"

#include "common/stage/stage.h"
#include "puzzle/actor/puzzle_block_dr_free.h"
#include "puzzle/map/puzzle_map.h"
#include "system/sys_player_input.h"
#include "system/sys_giant_input.h"
#include "root.h"
#include <base/math/tb_random.h>

namespace
{
    PuzzleBlockBase::BlockData BLOCK =
    {
        // □
        // □
        TB_TRUE,
        TB_TRUE,
        2,
        { TbPointS32(0, 0), TbPointS32(0, 1), TbPointS32(0, 0), TbPointS32(0, 0) },
    };
}

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleBlockDr::PuzzleBlockDr( TbSint32 playerIndex )
    : Super(playerIndex)
{
    updateNewBlock();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.26
 */
PuzzleBlockDr::~PuzzleBlockDr()
{
}

/*!
 * 再生成
 * シーンから出た
 * @author teco
 */
void PuzzleBlockDr::OnHitFloor()
{
    TbActorScene& scene = GetStage()->GetActorScene();
    
    PuzzleBlockDrFree* freeBlock = new PuzzleBlockDrFree(this);
    freeBlock->SetPoint(GetPoint());
    freeBlock->Enter(&scene,GetStage());

    if( !ReturnStartPoint() ){
        SetEnd(TB_TRUE);
        return;
    }
    SetMapChip(TILE_MAP_CHIP_NONE);
    updateNewBlock();
    SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
}

/*!
 * 終了更新
 * @author teco
 */
void PuzzleBlockDr::OnUpdateEnd()
{
    if( !ReturnStartPoint() ){
        return;
    }
    SetMapChip(TILE_MAP_CHIP_NONE);
    updateNewBlock();
    SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
    SetEnd(TB_FALSE);
}

/*!
 * ブロック更新
 * @author teco
 */
void PuzzleBlockDr::updateNewBlock()
{
    static const TbSint32 COLOR_MAX = 2;
    static const TbUint32 COLOR_TABLE[] =
    {
        CMN_PLAYER_COLOR_SKYBLUE , 
        CMN_PLAYER_COLOR_YELLOW ,
        CMN_PLAYER_COLOR_RED
    };
    SetBlock(BLOCK);

    TbUint32 color1 = COLOR_TABLE[TbRandom(2)];
    TbUint32 color2 = COLOR_TABLE[TbRandom(2)];
    SetBlockColor(0, color1);
    SetBlockColor(1, color2);
}
