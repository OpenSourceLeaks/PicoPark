/*!
 * パズルのブロック
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "puzzle_block_dr_free.h"
#include "puzzle_block_dr.h"

#include "puzzle/map/puzzle_dr_map.h"
#include "system/sys_player_input.h"
#include "system/sys_giant_input.h"
#include "root.h"
#include <base/math/tb_random.h>

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleBlockDrFree::PuzzleBlockDrFree( const PuzzleBlockDr* parent )
    : Super(0)
{
    SetFallTimeScale(0.5f);
    SetStage(parent->GetStage());
    PuzzleDrMap* map = static_cast<PuzzleDrMap*>(parent->GetMap());
    map->AddFreeBlock(this);
    SetMap(parent->GetMap());
    SetBlock(*parent);
    SetEnd(TB_TRUE);
    SetEnableControl(TB_FALSE);
}

/*!
 * デストラクタ
 * @author teco
 */
PuzzleBlockDrFree::~PuzzleBlockDrFree()
{
}

/*!
 * 終了更新
 * @author teco
 */
void PuzzleBlockDrFree::OnUpdateEnd()
{
#if 0
    TbBool isActive = IsActive();
    TB_ASSERT(GetBlockCount()>0);
    TbPointS32 down(0,1);
    if( IsMovable(down) ){
        SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
        SetEnd(TB_FALSE);
    }
#endif
}

/*!
 * 床に到着
 * @author teco
 */
void PuzzleBlockDrFree::OnHitFloor()
{
    SetEnd(TB_TRUE);
}

/*!
 * マップ上のブロックの何かが削除された場合
 * @author teco
 */
void PuzzleBlockDrFree::OnEraseMapBlock()
{
    // 一旦マップチップ解除
    for (size_t i = 0; i < GetBlockCount(); ++i) {
        TbPointS32 point = GetBlockPos(i) + GetPoint();
        if( TILE_MAP_CHIP_NONE == GetMap()->GetMapChip(point.x,point.y) ){
            EraseBlock(i);
            --i;
        }
    }
    if( 0 == GetBlockCount() ){
        SetActive(TB_FALSE);
        Release();
        Exit();
    }else{
        SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
        SetEnd(TB_FALSE);
    }
}
