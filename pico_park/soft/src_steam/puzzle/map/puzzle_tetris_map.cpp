/*!
 * パズル用マップ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "puzzle/map/puzzle_tetris_map.h"
#include "common/actor/actor.h"
#include "root.h"
#include <graphics/render/tb_draw_2d.h>
#include <base/math/tb_random.h>

static TbFloat32 FALL_TIME_NORMAL = 1.0f;

#if CMN_ENABLE_BUILD_GIANT_INPUT
#if 1
PuzzleMap::FallTimeParam FALL_TIME_PARAM[] = 
{
    { 1 , 0.5f } , 
    { 5 , 0.3f } ,
    { 10 , 0.1f } ,
};
#else
PuzzleMap::FallTimeParam FALL_TIME_PARAM[] = 
{
    { 1 , 0.9f } , 
    { 2 , 0.8f } , 
    { 3 , 0.7f } , 
    { 4 , 0.6f } , 
    { 5 , 0.5f } , 
    { 6 , 0.4f } ,
    { 7 , 0.3f } ,
    { 10 , 0.1f } ,
};
#endif
#elif 1
PuzzleTetrisMap::FallTimeParam FALL_TIME_PARAM[] = 
{
    { 10 , 0.8f } , 
    { 20 , 0.6f } ,
    { 30 , 0.4f } ,
    { 40 , 0.39f } ,
    { 50 , 0.37f } ,
	{ 60 , 0.35f },
	{ 70 , 0.33f } ,
};
#else
PuzzleMap::FallTimeParam FALL_TIME_PARAM[] = 
{
    { 10 , 0.8f } , 
    { 20 , 0.3f } ,
    { 30 , 0.2f } ,
};
#endif

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleTetrisMap::PuzzleTetrisMap()
    : Super()
    , m_NormaLineCount(0)
    , m_LineCount(0)
    , m_CountUpLineCount(0)
{
    SetFallTime(FALL_TIME_NORMAL);
    m_FallTimeCount = TB_ARRAY_COUNT_OF(FALL_TIME_PARAM);
    for( TbSizeT i = 0; i < m_FallTimeCount; ++i ) {
        m_FallTimeTable[i] = FALL_TIME_PARAM[i];
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.18
 */
PuzzleTetrisMap::~PuzzleTetrisMap()
{
}

/*!
 * 初期化
 * @author teco
 */
void PuzzleTetrisMap::OnInitialize( TbScriptLuaNode* node )
{
    Super::OnInitialize(node);
    TbFloat32 fallTimeDefault = 0.0f;
    if( node && TbScriptLuaNode::RESULT_OK == node->TryToFloat32(fallTimeDefault,"fallTimeDefault") ) {
        SetFallTime(fallTimeDefault);
    }
    
    if( node && TbScriptLuaNode::RESULT_OK == node->GoToChild("fallTimeTable") ) 
    {
        TbUint32 count = 0;
        if( TbScriptLuaNode::RESULT_OK == node->TryToTableLength(count) ){
            m_FallTimeCount = 0;
            for( TbUint32 i = 0; i < count; ++i ) {
                node->GoToChild(i);

                TbSint32 border = 0;
                node->TryToSint32ByIndex(border,0);

                TbFloat32 time = 0;
                node->TryToFloat32ByIndex(time,1);

                TbFloat32 offset = 0;
                node->TryToFloat32ByIndex(offset,2);
                time += Root::GetInstance().GetPlayerCount() * offset;

                m_FallTimeTable[m_FallTimeCount].border = border;
                m_FallTimeTable[m_FallTimeCount].time = time;
                
                node->GoToParent(1);
                ++m_FallTimeCount;
            }
        }
        node->GoToParent(1);
    }

}

/*!
 * マップチップ更新
 * @author teco
 */
void PuzzleTetrisMap::OnUpdate( TbFloat32 deltatime )
{
    m_CountUpLineCount = 0;
    updateTetrisLine();
}

/*!
 * 前景描画
 * @author teco
 */
void PuzzleTetrisMap::OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbFontBuffer2d* fontBuffer = Root::GetInstance().GetFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    TbVector3 posePos;
    poseMatrix.GetTranslation(posePos);
    TbSint32 offsetX = (GetOffset().x*GetChipSize()) + posePos.GetX();
    TbSint32 offsetY = (GetOffset().y*GetChipSize()) + posePos.GetY();
    TbSint32 value = GetLineCount();
    if( 0 < m_NormaLineCount ) {
        // ノルマが設定されている的は残り数
        value = TbMax<TbSint32>( 0 , static_cast<TbSint32>(m_NormaLineCount) - value );
    }
    fontBuffer->Printf( offsetX + static_cast<TbSint32>(GetInfoDispPos().GetX()) , 
                        offsetY + static_cast<TbSint32>(GetInfoDispPos().GetY()) ,
                        "%d", value );

    fontBuffer->PopSetting();

}

/*!
 * ライン設定
 * @author teco
 */
void PuzzleTetrisMap::updateTetrisLine()
{
    TbSint32 lineCount = 0;
    const TbRectS32& judgeRect = GetJudgeRect();
    TbSint32 endY = judgeRect.y + judgeRect.height;
    TbSint32 endX = judgeRect.x + judgeRect.width;
    for( TbSint32 y = endY-1; y >= 0; --y ){
        TbBool isEnableErase = isEnableEraseTetrisLine(y);
        if( isEnableErase ) {
            ++lineCount;
        }
        if( lineCount > 0 ) {
            do {
                // 一段ずらす
                TbBool isEnableCopy = 0 < (y-lineCount);
                for( TbSint32 copyX = judgeRect.x; copyX < endX; ++copyX ){
                    TbUint32 srcChip = GetMapChip(copyX,y-lineCount);
                    TbUint32 dstChip = GetMapChip(copyX,y);
                    if( srcChip != TILE_MAP_CHIP_NONE && srcChip < PUZZLE_MAP_CHIP_PRE_BLOCK ){
                        isEnableCopy = TB_FALSE;
                    }
                    if( dstChip != TILE_MAP_CHIP_NONE && dstChip < PUZZLE_MAP_CHIP_PRE_BLOCK  ){
                        continue;
                    }
                    if( isEnableCopy ) {
                        SetMapChip(copyX,y,srcChip);
                        SetMapChip(copyX,y-lineCount,TILE_MAP_CHIP_NONE);
                    }else{
                        SetMapChip(copyX,y,TILE_MAP_CHIP_NONE);
                    }
                }
                isEnableErase = isEnableEraseTetrisLine(y);
                if( isEnableErase ) {
                    ++lineCount;
                }
            }while( isEnableErase );
        }
    }
    if( lineCount > 0 ) {
        m_LineCount += lineCount;
        m_CountUpLineCount = lineCount;
        if( m_LineErasedCallback.IsValid() ){
            m_LineErasedCallback(lineCount);
        }
        TbUint32 count = GetPlayerBlockCount();
        for( TbUint32 i = 0; i < count; ++i  ){
            PuzzleBlockBase* block = GetPlayerBlock(i);
            block->OnEraseMapBlock();
        }
        // 10を越えたらスピードアップ
        for( size_t i = 0; i < m_FallTimeCount; ++i ) {
            if( m_FallTimeTable[i].border <= m_LineCount ) {
                SetFallTime( m_FallTimeTable[i].time );
            }
        }
    }
}

/*!
 * 特定の高さでラインが消せれるか
 * @author teco
 */
TbBool PuzzleTetrisMap::isEnableEraseTetrisLine(TbSint32 y)
{
    const TbRectS32& judgeRect = GetJudgeRect();
    for (TbSint32 x = 0; x < judgeRect.width; ++x){
        TbSint32 chip = GetMapChip(x+judgeRect.x,y);
        if( chip < PUZZLE_MAP_CHIP_POST_BLOCK || chip >= PUZZLE_MAP_CHIP_POST_BLOCK_END ) {
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}



/*!
 * デバッグメニュー構築
 * @author teco
 */
void PuzzleTetrisMap::OnCreateDebugMenu( TbDebugMenu* menu ) 
{
    Super::OnCreateDebugMenu(menu);

    TbDebugMenuItemButton* debugButton = menu->CreateItemButton("Set Debug Block 1");
    debugButton->SetClickedCallback(TbCreateMemFunc(this,&Self::setDebugBlock));

}

/*!
 * デバッグ用ブロック設定ボタンをクリック
 * @author teco
 */
void PuzzleTetrisMap::setDebugBlock()
{
    TbUint32 y = GetTileHeight();
    TbUint32 emptyX = GetTileWidth()/2;
    TbUint32 line = 0;
    for( ; y > 0 && line < 4; --y )
    {
        TbBool isEnableCountUpLine = TB_FALSE;
        for( TbUint32 x = 0; x < GetWidth(); ++x )
        {
            TbUint32 chip = GetMapChip(x,y);
            if( chip == TILE_MAP_CHIP_NONE ){
                isEnableCountUpLine = TB_TRUE;
                if( emptyX == x ) {
                    continue;
                }
                SetMapChip(x,y,PUZZLE_MAP_CHIP_POST_BLOCK);
            }else if( PUZZLE_MAP_CHIP_PRE_BLOCK <= chip && chip < PUZZLE_MAP_CHIP_POST_BLOCK_END ){
                isEnableCountUpLine = TB_TRUE;
            }
        }
        if( isEnableCountUpLine ) {
            ++line;
        }
    }
}