/*!
 * パズルのブロック
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "puzzle_block_base.h"
#include "puzzle/map/puzzle_map.h"
#include <crayon/input/cr_player_input.h>
#include <crayon/input/cr_giant_input.h>
#include "root.h"
#include <base/math/tb_random.h>
#include <base/algorithm/tb_algorithm_misc.h>
#include <crayon/stage/cr_stage.h>

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.26
 */
PuzzleBlockBase::PuzzleBlockBase()
    : m_PlayerIndex(0)
    , m_Map(NULL)
    , m_BlockCount(0)
    , m_FallTimer(0.0f)
    , m_FallFloorTimer(0.0f)
    , m_FallTimeScale(1.0f)
    , m_PadIndex(0)
    , m_BaseMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK)
{
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.26
 */
PuzzleBlockBase::~PuzzleBlockBase()
{
}

/*!
 * セットアップ
 * @author teco
 * @since 2013.09.26
 */
void PuzzleBlockBase::Setup(const CrActorFactory::UserData& userData)
{
    TbSint32 playerIndex = userData.AsSint32(0);
    SetEnableControl(playerIndex<CMN_PLAYER_MAX);
    SetEnableMove(TB_TRUE);
    m_PlayerIndex = playerIndex % CMN_PLAYER_MAX;
    if (m_PlayerIndex < static_cast<TbSint32>(Root::GetInstance().GetPlayerCount())) {
        m_BitArray.SetBit(FLAG_ACTIVE);
    }
    m_PadIndex = CrInput::GetPlayerPadIndex(m_PlayerIndex);
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.26
 */
void PuzzleBlockBase::OnUpdate( TbFloat32 deltatime )
{
    if( IsEnd() ){
        OnUpdateEnd();
        return;
    }
    const CrPlayerInput& input = GetStage()->GetPlayerInput();

    if( !m_BitArray.Test(FLAG_ACTIVE) ){
#if 0 // 途中参加機能
        if( SysPlayerInput::IsPress(SysPlayerInput::BUTTON_ROTATE_R,m_PlayerIndex) ||
            SysPlayerInput::IsPress(SysPlayerInput::BUTTON_ROTATE_L,m_PlayerIndex) )
        {
            m_BitArray.SetBit(FLAG_ACTIVE);
        }
#endif
        return;
    }

    if( !IsEnableMove() ) {
        return;
    }

    TbBool isOnFloor = TB_FALSE;
    {
        TbPointS32 point = m_Point;
        point.y += 1;
        TbBool isHitMap = TB_FALSE;
        if (!isMovable(point, m_Block, m_BlockCount, &isHitMap)) {
            if (isHitMap) {
                isOnFloor = TB_TRUE;
            }
        }
    }

    // 上キー回転
    TbBool isEnableRot = TB_FALSE;
    TbSint32 movX = 0;
    TbSint32 movY = 0;
    RotateType rot = ROTATE_NONE;
    TbBool   isDownByInput = TB_FALSE;
   
    if( IsEnableControl() )
    {
        if( m_BitArray.Test(FLAG_ENABLE_ROT) ){
            if( input.IsPress(CrInput::BUTTON_ROTATE_R,m_PlayerIndex) ){
                rot = ROTATE_R;
            }else if( input.IsPress(CrInput::BUTTON_ROTATE_L,m_PlayerIndex) ){
                rot = ROTATE_L;
            }
        }

        // 左右移動
        if( input.IsOnRepeat(CrInput::BUTTON_LEFT,m_PlayerIndex) ){
            movX = -1;
        }else if( input.IsOnRepeat(CrInput::BUTTON_RIGHT,m_PlayerIndex) ){
            movX = 1;
        }

        // 下キー落下速度アップ
        if( input.IsOnRepeat(CrInput::BUTTON_DOWN,m_PlayerIndex) ){
            if (isOnFloor) {
                if ( m_FallFloorTimer < (GetMap()->GetFallFloorTime() - 0.12f)) {
                    isDownByInput = TB_TRUE;
                }
            }else{
                isDownByInput = TB_TRUE;
            }
            if (isDownByInput) {
                movY = 1;
            }
        }
    }

    // 自然落下
    m_FallTimer -= deltatime;
    m_FallFloorTimer -= deltatime;
    if( m_FallTimer <= 0.0f ){
        movY = 1;
        m_FallTimer = GetMap()->GetFallTime() * m_FallTimeScale;
    }

    if ( !isDownByInput )
    {
        if (isOnFloor) {
            movY = 0;
            if (m_FallFloorTimer <= 0.0f) {
                movY = 1;
            }
        }
    }

    if (0 < movY)
    {
        m_FallTimer      = GetMap()->GetFallTime() * m_FallTimeScale;
        m_FallFloorTimer = GetMap()->GetFallFloorTime();
    }

    move( movX , movY , rot );
}

/*!
 * ポイント設定
 * @author teco
 */
void PuzzleBlockBase::SetPoint( TbSint32 x , TbSint32 y , TbBool isEnableMapChip )
{
    TbUint32 mapChip = m_BaseMapChip;
    if( m_BitArray.Test(FLAG_UPDATE_MAP_CHIP) && isEnableMapChip ) {
        SetMapChip(CR_TILE_MAP_CHIP_NONE);
    }
    m_Point.x = x;
    m_Point.y = y;
    if( isEnableMapChip ){
        SetMapChip(mapChip);
    }
}

/*!
 * スタートポイントに戻る
 * @author teco
 */
bool PuzzleBlockBase::ReturnStartPoint()
{
    // 終了
    if (!isMovable(m_StartPoint, m_Block, m_BlockCount)){
        return TB_FALSE;
    }
    if( m_BaseMapChip != PUZZLE_MAP_CHIP_POST_BLOCK ) {
        SetMapChip(CR_TILE_MAP_CHIP_NONE);
    }
    // 初期位置に戻る
    m_Point = m_StartPoint;
    SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
    return TB_TRUE;
}

/*!
 * シーンに入った
 * @author teco
 * @since 2013.09.26
 */
void PuzzleBlockBase::OnEnter( void* userData )
{
    PuzzleMap* map = GetMap();
    if( TB_VERIFY(map) ) {
        // マップチップ設定
        if( m_BitArray.Test(FLAG_ACTIVE) ){
            SetMapChip(m_BaseMapChip);
        }
        m_FallTimer = map->GetFallTime() * m_FallTimeScale;
    }
}
    
/*!
 * シーンから出た
 * @author teco
 * @since 2013.09.26
 */
void PuzzleBlockBase::OnExit()
{
}

/*!
 * 移動可能か
 * @author teco
 * @since 2013.09.28
 */
TbBool PuzzleBlockBase::isMovable(const TbPointS32& point, const BlockInfo* block, TbUint32 blockCount, TbBool* isHitMap)
{
    if( !m_Map ){
        return TB_FALSE;
    }
    TbBool isMapChipPre = m_BaseMapChip == PUZZLE_MAP_CHIP_PRE_BLOCK;
    if (isMapChipPre) {
        SetMapChip(CR_TILE_MAP_CHIP_NONE);
    }
	for (size_t i = 0; i < blockCount; ++i) {
        TbPointS32 newPoint = block[i].pos + point;
        if( !m_Map->IsMovable(newPoint.x,newPoint.y) ){
            if( isHitMap ){
                TbUint32 chip = m_Map->GetMapChip(newPoint.x,newPoint.y) ;
                if( PUZZLE_MAP_CHIP_PRE_BLOCK <= chip && chip < PUZZLE_MAP_CHIP_PRE_BLOCK_END ){
                    *isHitMap = TB_FALSE;
                }else{
                    *isHitMap = TB_TRUE;
                }
            }
            if (isMapChipPre) {
                SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
            }
            return TB_FALSE;
        }
    }
    if (isMapChipPre) {
        SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
    }
    return TB_TRUE;
}

/*!
 * 移動
 * @author teco
 * @since 2013.09.26
 */
void PuzzleBlockBase::move( TbSint32 x , TbSint32 y , RotateType rot )
{
    if( 0 == x && 0 == y && (rot==ROTATE_NONE) ){
        return;
    }
    SetMapChip(CR_TILE_MAP_CHIP_NONE);
    TbPointS32 opoint = m_Point;
    TbPointS32 point = m_Point;
    TbPointS32 newPoint = m_Point;
    if( rot != ROTATE_NONE ){
        BlockInfo newBlock[BLOCK_COUNT_MAX];
        // 回転
        if( ROTATE_R == rot ) {
            // 右回転
            for( size_t i = 0; i < m_BlockCount; ++i ) {
                newBlock[i] = m_Block[i];
                newBlock[i].pos.x = -m_Block[i].pos.y;
                newBlock[i].pos.y = m_Block[i].pos.x;
            }
        }else{
            // 左回転
            for (size_t i = 0; i < m_BlockCount; ++i) {
                newBlock[i] = m_Block[i];
                newBlock[i].pos.x = m_Block[i].pos.y;
                newBlock[i].pos.y = -m_Block[i].pos.x;
            }
        }
        if (isMovable(newPoint, newBlock, m_BlockCount)){
            for (size_t i = 0; i < m_BlockCount; ++i) {
                m_Block[i].pos = newBlock[i].pos;
            }
        }
    }

    if( x != 0 ){
        newPoint.x +=x;
        // X軸移動
        if (isMovable(newPoint, m_Block, m_BlockCount)){
            point = newPoint;
        }else{
            newPoint.x -= x;
        }
    }

    // Y軸移動
    if( y != 0 ){
        TbSint32 count = TbAbs(y);
        TbSint32 sign = TbSign(y);
        for( TbSint32 i = 0; i < count; ++i  )
        {
            newPoint.y += sign;
            TbBool isHitMap = TB_FALSE;
            if( isMovable(newPoint,m_Block,m_BlockCount,&isHitMap) ){
                point = newPoint;
            }else{
                if( y > 0 && isHitMap && 0 == i ) 
                {
                    // マップチップを消滅可能状態に変更
                    m_Point = point;
                    SetMapChip(PUZZLE_MAP_CHIP_POST_BLOCK);
                    OnHitFloor();
                    return;
                }
                break;
            }
        }
    }
    TB_ASSERT(m_Point == opoint);
    m_Point = point;
    SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
}

/*!
* ブロック設定
* @author teco
*/
void PuzzleBlockBase::SetBlock(const TbPointS32* blockPoint, TbUint32 blockCount, TbBool isEnableRot)
{
    for (TbSizeT i = 0; i < blockCount; ++i) {
        m_Block[i].pos = blockPoint[i];
    }
    m_BlockCount = blockCount;
    m_BitArray.SetBit(FLAG_ENABLE_ROT, isEnableRot);
}

/*!
* ブロック設定
* @author teco
*/
void PuzzleBlockBase::SetBlock( const PuzzleBlockBase& src )
{
    m_BlockCount = src.GetBlockCount();
    for (TbSizeT i = 0; i < m_BlockCount; ++i) {
        m_Block[i].pos = src.m_Block[i].pos;
        m_Block[i].color = src.m_Block[i].color;
    }
    m_BitArray.SetBit(FLAG_ENABLE_ROT, src.m_BitArray.Test(FLAG_ENABLE_ROT));
    m_BaseMapChip = src.GetBaseMapChip();
}

/*!
 * ブロック削除
 * @author teco
 */
void PuzzleBlockBase::EraseBlock( TbUint32 blockIndex )
{
    if( blockIndex < m_BlockCount ){
        for( TbUint32 i = blockIndex; i < m_BlockCount-1; ++i ){
            m_Block[i] = m_Block[i+1];
        }
        --m_BlockCount;
    }
}

/*!
 * ブロックカラー設定
 * @author teco
 */
void PuzzleBlockBase::SetBlockColor(TbUint32 blockIndex, TbUint32 colorIndex)
{
    if (blockIndex < TB_ARRAY_COUNT_OF(m_Block) ){
        m_Block[blockIndex].color = colorIndex;
    }
}

/*!
* ブロックカラー設定
* @author teco
*/
void PuzzleBlockBase::SetBlockColor(TbUint32 colorIndex)
{
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Block); ++i )
    {
        m_Block[i].color = colorIndex;
    }
}

/*!
 * マップチップ更新
 * @author teco
 */
void PuzzleBlockBase::SetMapChip( TbUint32 chip )
{
    if( !m_Map ) {
        return;
    }
    for (size_t i = 0; i < m_BlockCount; ++i) 
    {
        TbPointS32 point = m_Block[i].pos + m_Point;
        TbUint32 chipValue = chip;
        if (CR_TILE_MAP_CHIP_NONE != chip) {
            chipValue += m_Block[i].color;
        }
        m_Map->SetMapChip(point.x, point.y, chipValue);
    }
    m_BitArray.SetBit(FLAG_UPDATE_MAP_CHIP,chip != CR_TILE_MAP_CHIP_NONE);
    m_BaseMapChip = chip;
}
