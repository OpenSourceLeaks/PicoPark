/*!
 * パズルのブロック
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "puzzle_block_tetris.h"

#if 1
#include "puzzle/map/puzzle_tetris_map.h"
#include <crayon/input/cr_player_input.h>
#include <crayon/input/cr_giant_input.h>
#include "root.h"
#include <base/math/tb_random.h>
#include <crayon/stage/cr_stage.h>

namespace
{
    PuzzleBlockTetris::BlockData BLOCK_LIST[] =
    {
        // □□
        // □□
        {
            TB_FALSE ,
            TB_TRUE , 
            4 ,
            { TbPointS32(0,0) , TbPointS32(0,1) , TbPointS32(1,0) , TbPointS32(1,1) } ,
        },
        // □□
        //   □□
        {
            TB_TRUE , 
            TB_TRUE , 
            4,
            { TbPointS32(-1, -1), TbPointS32(0, -1), TbPointS32(0, 0), TbPointS32(1, 0) },
        },
        //   □□
        // □□
        {
            TB_TRUE , 
            TB_TRUE , 
            4,
            { TbPointS32(-1, 1), TbPointS32(0, 0), TbPointS32(0, 1), TbPointS32(1, 0) },
        },
        // □□□
        //   □  
        {
            TB_TRUE , 
            TB_TRUE , 
            4,
            { TbPointS32(-1, 0), TbPointS32(0, 0), TbPointS32(0, 1), TbPointS32(1, 0) },
        },
        // □□□
        //     □
        {
            TB_TRUE , 
            TB_TRUE , 
            4,
            { TbPointS32(-1, 0), TbPointS32(0, 0), TbPointS32(1, 0), TbPointS32(1, 1) },
        },
        // □□□
        // □
        {
            TB_TRUE , 
            TB_TRUE , 
            4,
            { TbPointS32(-1, 0), TbPointS32(-1, -1), TbPointS32(0, 0), TbPointS32(1, 0) },
        },
        // □□□□
        {
            TB_TRUE , 
            TB_TRUE , 
            4,
            { TbPointS32(-1, 0), TbPointS32(0, 0), TbPointS32(1, 0), TbPointS32(2, 0) },
        },
    };

    PuzzleBlockTetris::BlockData BLOCK_LIST_3[] =
    {
#if 1
        // □□
        // □
        {
            TB_TRUE ,
            TB_TRUE , 
            3 ,
            { TbPointS32(0,0) , TbPointS32(0,1) , TbPointS32(1,0) } ,
        },
#endif
        // □□□
        {
            TB_TRUE , 
            TB_TRUE , 
            3,
            { TbPointS32(-1, 0), TbPointS32(0,0), TbPointS32(1, 0) },
        },
#if 0
        //   □
        // □
        {
            TB_TRUE , 
            TB_TRUE , 
            2,
            { TbPointS32(-1, 0), TbPointS32(0,-1), TbPointS32(1, 0) },
        },
        // □
        {
            TB_FALSE , 
            TB_FALSE , 
            1,
            { TbPointS32(0, 0) },
        },
#endif
    };
}

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleBlockTetris::PuzzleBlockTetris( Type type )
    : Super()
    , m_CurrentBlockType(0)
    , m_BlockTypeTable(nullptr)
    , m_BlockTypeMax(0)
{
    switch( type )
    {
    case TYPE_DEFAULT:
        m_BlockTypeTable = BLOCK_LIST;
        m_BlockTypeMax = TB_ARRAY_COUNT_OF(BLOCK_LIST);
        break;
    case TYPE_3:
        m_BlockTypeTable = BLOCK_LIST_3;
        m_BlockTypeMax = TB_ARRAY_COUNT_OF(BLOCK_LIST_3);
        break;
    default:
        TB_ASSERT(0);
        break;
    }
    m_SpriteHooker.SetDrawFunc(TbCreateMemFunc(this, &Self::draw));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.26
 */
PuzzleBlockTetris::~PuzzleBlockTetris()
{
}

/*!
 * セットアップ
 * @author teco
 */
void PuzzleBlockTetris::Setup(const CrActorFactory::UserData& userData)
{
    Super::Setup(userData);
    m_NextBlockType = randBlockType();
    SetBlockColor(GetPadIndex());
    updateNewBlock();
}

/*!
 * シーンに入った
 * @author teco
 */
void PuzzleBlockTetris::OnEnter( void* userData )
{
    Super::OnEnter(userData);
    CrStage* stage = reinterpret_cast<CrStage*>(userData);
    if( TB_VERIFY(stage) ) {
        m_SpriteHooker.Enter( &stage->GetSpriteScene(0) );
    }
}
    
/*!
 * シーンから出た
 * @author teco
 */
void PuzzleBlockTetris::OnExit()
{
    m_SpriteHooker.Exit();
}

/*!
 * 再生成
 * シーンから出た
 * @author teco
 */
void PuzzleBlockTetris::OnHitFloor()
{
    const BlockData& data = m_BlockTypeTable[m_CurrentBlockType];
    SetBlock(data);
    if( !ReturnStartPoint() ){
        SetEnd(TB_TRUE);
        return;
    }
    SetMapChip(CR_TILE_MAP_CHIP_NONE);
    updateNewBlock();
    SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
}

/*!
 * マップ上のブロックの何かが削除された場合
 * @author teco
 */
void PuzzleBlockTetris::OnEraseMapBlock()
{
    if( !IsActive() ){
        return;
    }
    PuzzleTetrisMap* map = static_cast<PuzzleTetrisMap*>(GetMap());
    
    TbPointS32 pos = GetPoint();
    TbUint32 lineCount = map->GetCountUpLineCount();
    // ラインが消えると、自動で位置が動くので同期させる
    pos.y += lineCount;
    SetPoint(pos.x,pos.y,TB_FALSE);
    
    // 一旦マップチップ解除
    SetMapChip(CR_TILE_MAP_CHIP_NONE);

    // 元の位置に戻れるだけ戻る
    TbPointS32 up(0,-1);
    for( TbUint32 i = 0; i < lineCount; ++i ){
        if( IsMovable(up) ){
            --pos.y;
        }else{
            break;
        }
    }

    SetPoint(pos);
    SetMapChip(PUZZLE_MAP_CHIP_PRE_BLOCK);
}

/*!
 * 終了更新
 * @author teco
 */
void PuzzleBlockTetris::OnUpdateEnd()
{
    if (ReturnStartPoint()) {
        SetEnd(TB_FALSE);
    }
}

/*!
 * ブロックタイプを乱数
 * @author teco
 */
TbUint32 PuzzleBlockTetris::randBlockType() const
{
    TbUint32 type = TbRandom(m_BlockTypeMax-1);
    if( m_BlockTypeTable[type].isEnableHigh ){
        return type;
    }
    return TbRandom(m_BlockTypeMax-1);
}

/*!
 * 描画
 * @author teco
 */
void PuzzleBlockTetris::draw( const TbMatrix& poseMatrix,TbSpriteMaterial* material )
{
    PuzzleMap* map = GetMap();
    if (!map) {
        return;
    }

#if 0
    auto font = Root::GetInstance().GetFont();
    font->PushSetting();
    font->SetFontSize(16);
    font->SetColor(TbColor::White());
    TbFloat32 y = 100 +30*GetPadIndex();
    font->Printf(30,y,"FT=%0.4f",GetFallFloorTimer());
    font->PopSetting();
#endif
#if CMN_ENABLE_BUILD_GIANT_INPUT
    static TbFloat32 NEXT_X = 28.0f;
    static TbFloat32 NEXT_Y = 4.0f;
    // Nextを表示
    const BlockData& data = m_BlockTypeTable[m_NextBlockType];
    TbFloat32 size = static_cast<TbFloat32>(map->GetChipSize());
    for( size_t i = 0; i < GetBlockCount(); ++i ) {
        TbVector2 pos( NEXT_X + data.point[i].x , NEXT_Y + data.point[i].y );
//        pos.SetX( static_cast<TbFloat32>(m_StartPoint.x + data.point[i].x) );
//        pos.SetY( static_cast<TbFloat32>(m_StartPoint.y + data.point[i].y) );
        TbRectF32 rect = TbRectF32( pos.GetX() * size , pos.GetY() * size , size , size );
        TbUint32 colorOffset = (GetBlockColor(0)+1)%CMN_PLAYER_MAX;
        TbRectF32 uvRect = PuzzleMap::GetMapChipUV(PUZZLE_MAP_CHIP_PRE_BLOCK+colorOffset);
        material->Draw( rect , uvRect , TbColor::White() , TbMatrix::IDENTITY );
    }
#endif
}

/*!
 * ブロック更新
 * @author teco
 */
void PuzzleBlockTetris::updateNewBlock()
{
    const BlockData& data = m_BlockTypeTable[m_NextBlockType];
    m_CurrentBlockType = m_NextBlockType;
    SetBlock(data);
    m_NextBlockType = randBlockType();

#if CMN_ENABLE_BUILD_GIANT_INPUT
    TbUint32 colorIndex = (GetBlockColor(0)+1)%CMN_PLAYER_MAX;
    SetBlockColor(colorIndex);
#endif
}

#endif