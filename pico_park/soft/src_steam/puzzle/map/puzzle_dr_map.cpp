/*!
 * パズル用マップ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "puzzle/map/puzzle_dr_map.h"
#include "common/actor/actor.h"
#include "root.h"
#include <graphics/render/tb_draw_2d.h>
#include <base/math/tb_random.h>
/*!
 * コンストラクタ
 * @author teco
 */
PuzzleDrMap::PuzzleDrMap()
    : Super()
    , m_TargetCountDr(0)
{
    m_FreeBlockList.SetCapacity(100);
    m_FreeBlockList.SetEnableSetCapacityAuto(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 */
PuzzleDrMap::~PuzzleDrMap()
{
}

/*!
 * 自由ブロック追加
 * @author teco
 */
void PuzzleDrMap::AddFreeBlock( PuzzleBlockBase* block )
{
    block->Retain();
    m_FreeBlockList.PushBack(block);
    ++m_FreeBlockTotal;
#if 0
    if( m_FreeBlockTotal== Root::GetInstance().GetPlayerCount()*20 ){
        SetFallTime(0.8f);
    }else if( m_FreeBlockTotal == Root::GetInstance().GetPlayerCount()*50 ){
        SetFallTime(0.6f);
    }
#endif
}

/*!
 * 初期化
 * @author teco
 */
void PuzzleDrMap::OnInitialize( TbScriptLuaNode* node )
{
    if( !node ) {
        return;
    }
    Super::OnInitialize(node);
    {
        TbUint32 offset = 0;
        TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToUint32(m_TargetCountDr,"targetCount"));
        TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToUint32(offset,"targetCountOffset"));
        m_TargetCountDr += offset * Root::GetInstance().GetPlayerCount();
        setupTargetDr();
    }
}

/*!
 * マップチップ更新
 * @author teco
 */
void PuzzleDrMap::OnUpdate( TbFloat32 deltatime )
{
    updateDr();

    // ガベコレ
    for( SubBlockList::Iterator it = m_FreeBlockList.Begin(); it != m_FreeBlockList.End(); )
    {
        if( !(*it)->IsActive() ){
            (*it)->Release();
            it = m_FreeBlockList.Erase(it);
        }else{
            ++it;
        }
    }
}

/*!
 * 前景描画
 * @author teco
 */
void PuzzleDrMap::OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbFontBuffer2d* fontBuffer = Root::GetInstance().GetFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    TbVector3 posePos;
    poseMatrix.GetTranslation(posePos);
    TbSint32 offsetX = GetOffset().x*GetChipSize() + posePos.GetX();
    TbSint32 offsetY = GetOffset().y*GetChipSize() + posePos.GetY();
    poseMatrix.GetTranslation(posePos);
    poseMatrix.GetTranslation(posePos);
    fontBuffer->Printf( offsetX + static_cast<TbSint32>(GetInfoDispPos().GetX()) , 
                        offsetY + static_cast<TbSint32>(GetInfoDispPos().GetY()) ,
                        "%d",GetTargetCount());

    fontBuffer->PopSetting();
}


/*!
 * ターゲットセットアップ
 * @author teco
 */
void PuzzleDrMap::setupTargetDr()
{
    static const TbUint32 TARGET_COUNT_HEIGHT_MIN = 8;
    static const TbUint32 COLOR_TABLE[] =
    {
        CMN_PLAYER_COLOR_SKYBLUE , 
        CMN_PLAYER_COLOR_YELLOW ,
        CMN_PLAYER_COLOR_RED
    };
    const TbRectS32& judgeRect = GetJudgeRect();
    TbSint32 endY = judgeRect.y + judgeRect.height;
    TbSint32 endX = judgeRect.x + judgeRect.width;
    TbBool isErased = TB_FALSE;

    TbUint16 widthTable[256];
    TB_RESQUE(judgeRect.width<TB_ARRAY_COUNT_OF(widthTable));
    TB_RESQUE(TARGET_COUNT_HEIGHT_MIN<judgeRect.height);
    TbMemClear(widthTable);

    TbUint32 targetCount = m_TargetCountDr;
    TbUint32 targetCountHeightMax = judgeRect.height-TARGET_COUNT_HEIGHT_MIN;

    TbUint32 activeWidthCount  = judgeRect.width;
    for( TbUint32 i = 0; i < targetCount; ++i ){
        TbUint32 index = TbRandom(activeWidthCount-1);
        TbUint32 realIndex = 0;
        for( TbSint32 x = 0; x < judgeRect.width; ++x ){
            if( widthTable[x] == targetCountHeightMax ) {
                continue;
            }
            if( realIndex == index ){
                ++widthTable[x];
                if( widthTable[x] == targetCountHeightMax ){
                    --activeWidthCount;
                }
                break;
            }
            ++realIndex;
        }
    }

    for (TbSint32 x = 0; x < judgeRect.width; ++x )
    {
        if( 0 == widthTable[x] ) {
            continue;
        }
        TbUint32 height = targetCountHeightMax;
        for (TbUint32 y = endY-1; judgeRect.y+TARGET_COUNT_HEIGHT_MIN <= y && 0 < widthTable[x]; --y , --height )
        {
            TbSint32 rate = TbMax<TbSint32>(widthTable[x],height/2);
            TbSint32 value1 = TbRandom(height-1);
            if( rate  <= value1) {
                continue;
            }

            TbUint32 mapChipTableCount = 0;
            TbUint32 mapChipTable[TB_ARRAY_COUNT_OF(COLOR_TABLE)];
            for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(COLOR_TABLE); ++i ) {
                TbUint32 mapChip = PUZZLE_MAP_CHIP_TARGET_BLOCK+COLOR_TABLE[i];
                if( isEnableSetTargetDr(x+judgeRect.x,y,mapChip) ){
                    mapChipTable[mapChipTableCount] = mapChip;
                    ++mapChipTableCount;
                }
            }
            TB_ASSERT(mapChipTableCount>0);
            {
                TbUint32 mapChip = mapChipTable[TbRandom(mapChipTableCount-1)];
                SetMapChip(x+judgeRect.x,y,mapChip);
                --widthTable[x];
            }
        }
    }

}

/*!
 * ターゲットを設置可能か
 * @author teco
 */
TbBool PuzzleDrMap::isEnableSetTargetDr( TbSint32 x , TbSint32 y , TbUint32 mapchip )
{
    TbUint32 count1 = 0;
    TbUint32 count2 = 0;
    TbUint32 count3 = 0;
    TbUint32 count4 = 0;
    for( TbSint32 i = 0; i < 3; ++i ){
        if( mapchip == GetMapChip(x,y-1-i) ){
            ++count1;
        }
        if( mapchip == GetMapChip(x,y+1+i) ){
            ++count2;
        }
        if( mapchip == GetMapChip(x-1-i,y) ){
            ++count3;
        }
        if( mapchip == GetMapChip(x+1+i,y) ){
            ++count4;
        }
    }
    return count1 < 3 && count2 < 3 && count3 < 3 && count4 < 3;
}

/*!
 * 更新
 * @author teco
 */
void PuzzleDrMap::updateDr()
{
    const TbRectS32& judgeRect = GetJudgeRect();
    TbSint32 endY = judgeRect.y + judgeRect.height;
    TbSint32 endX = judgeRect.x + judgeRect.width;
    TbBool isErased = TB_FALSE;

    // 横
    for (TbSint32 y = judgeRect.y; y < endY; ++y)
    {
        TbUint32 count = 0;
        TbUint32 currentChip = TILE_MAP_CHIP_NONE;
        for (TbSint32 x = judgeRect.x; x < endX; ++x)
        {
            TbSint32 chip = GetMapChip(x,y);
            TbBool isValid = TB_FALSE;
            TbBool isActive = TB_FALSE;
            if( chip < PUZZLE_MAP_CHIP_POST_BLOCK || chip >= PUZZLE_MAP_CHIP_TARGET_BLOCK_END ) {
                isValid = TB_FALSE;
            }else{
                isActive = TB_TRUE;
                if( PUZZLE_MAP_CHIP_TARGET_BLOCK <= chip && chip < PUZZLE_MAP_CHIP_TARGET_BLOCK_END ) {
                    chip -= PUZZLE_MAP_CHIP_TARGET_BLOCK;
                }else if( PUZZLE_MAP_CHIP_POST_BLOCK <= chip && chip < PUZZLE_MAP_CHIP_POST_BLOCK_END ) {
                    chip -= PUZZLE_MAP_CHIP_POST_BLOCK;
                }
                isValid = (count == 0) || (currentChip==chip);
            }
            if( isValid ){
                ++count;
                currentChip = chip;
            }else{
                if( count >= 4 ){
                    // ボーダー超えていたので消す
                    for (TbUint32 eraseX = 0; eraseX < count; ++eraseX){
                        SetMapChip(x-1-eraseX,y,TILE_MAP_CHIP_NONE);
                    }
                    isErased = TB_TRUE;
                }
                if( isActive ){
                    count = 1;
                    currentChip = chip;
                }else{
                    count = 0;
                    currentChip = TILE_MAP_CHIP_NONE;
                }
            }
        }
        if( count >= 4 ){
            // ボーダー超えていたので消す
            for (TbUint32 eraseX = 0; eraseX < count; ++eraseX){
                SetMapChip(endX-1-eraseX,y,TILE_MAP_CHIP_NONE);
            }
            isErased = TB_TRUE;
        }else{
            count = 0;
            currentChip = TILE_MAP_CHIP_NONE;
        }
    }

    // 縦
    for (TbSint32 x = judgeRect.x; x < endX; ++x)
    {
        TbUint32 count = 0;
        TbUint32 currentChip = TILE_MAP_CHIP_NONE;
        for (TbSint32 y = judgeRect.y; y < endY; ++y)
        {
            TbSint32 chip = GetMapChip(x,y);
            TbBool isValid = TB_FALSE;
            TbBool isActive = TB_FALSE;
            if( chip < PUZZLE_MAP_CHIP_POST_BLOCK || chip >= PUZZLE_MAP_CHIP_TARGET_BLOCK_END ) {
                isValid = TB_FALSE;
            }else{
                isActive = TB_TRUE;
                if( PUZZLE_MAP_CHIP_TARGET_BLOCK <= chip && chip < PUZZLE_MAP_CHIP_TARGET_BLOCK_END ) {
                    chip -= PUZZLE_MAP_CHIP_TARGET_BLOCK;
                }else if( PUZZLE_MAP_CHIP_POST_BLOCK <= chip && chip < PUZZLE_MAP_CHIP_POST_BLOCK_END ) {
                    chip -= PUZZLE_MAP_CHIP_POST_BLOCK;
                }
                isValid = (count == 0) || (currentChip==chip);
            }
            if( isValid ){
                ++count;
                currentChip = chip;
            }else{
                if( count >= 4 ){
                    // ボーダー超えていたので消す
                    for (TbUint32 eraseY = 0; eraseY < count; ++eraseY){
                        SetMapChip(x,y-1-eraseY,TILE_MAP_CHIP_NONE);
                    }
                    isErased = TB_TRUE;
                }
                if( isActive ){
                    count = 1;
                    currentChip = chip;
                }else{
                    count = 0;
                    currentChip = TILE_MAP_CHIP_NONE;
                }
            }
        }
        if( count >= 4 ){
            // ボーダー超えていたので消す
            for (TbUint32 eraseY = 0; eraseY < count; ++eraseY){
                SetMapChip(x,endY-1-eraseY,TILE_MAP_CHIP_NONE);
            }
            isErased = TB_TRUE;
        }else{
            count = 0;
            currentChip = TILE_MAP_CHIP_NONE;
        }
    }

    TbUint32 targetCount = 0;
    for (TbSint32 x = judgeRect.x; x < endX; ++x)
    {
        for (TbSint32 y = judgeRect.y; y < endY; ++y)
        {
            TbSint32 chip = GetMapChip(x,y);
            if( PUZZLE_MAP_CHIP_TARGET_BLOCK <= chip && chip < PUZZLE_MAP_CHIP_TARGET_BLOCK_END ) {
                ++targetCount;
            }
        }
    }
    m_TargetCountDr = targetCount;
    if( isErased ){
        TbUint32 count = GetPlayerBlockCount();
        for( TbUint32 i = 0; i < count; ++i  ){
            PuzzleBlockBase* block = GetPlayerBlock(i);
            block->OnEraseMapBlock();
        }
        TbForIterator( ite , m_FreeBlockList )
        {
            (*ite)->OnEraseMapBlock();
        }
        if( m_BlockErasedCallbackDr.IsValid() ){
            m_BlockErasedCallbackDr();
        }
    }
}

