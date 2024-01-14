/*!
 * パズル用マップ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "puzzle/map/puzzle_map.h"
#include "common/actor/actor.h"
#include "root.h"
#include <graphics/render/tb_draw_2d.h>
#include <base/math/tb_random.h>

const TbUint32 MAP_CHIP_ATTR[] = 
{
    // PRE
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 

    // 設置確定後のパズルブロック
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 

    // 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION , 
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(MAP_CHIP_ATTR)==PUZZLE_MAP_CHIP_MAX-TILE_MAP_CHIP_USER_BEGIN);

static TbFloat32 UV_ERROR = 0.0001f;
#if !CMN_ENABLE_BUILD_100P
#define PUZ_TEX_UV_RECT(x,y,w,h) CMN_TEX_UV_RECT_2(x,y,w,h,UV_ERROR)
#else // 100人テトリス用
#define PUZ_TEX_UV_RECT(x,y,w,h) CMN_TEX_UV_RECT_2(x,y,6,6,UV_ERROR)
#endif

// アクションパート用マップチップのUV矩形座標
static TB_NDEBUG_CONST TbRectF32 MAP_CHIP_UV_RECT[] = 
{
    TbRectF32() , // TILE_MAP_CHIP_INVALID , 
    TbRectF32() , // TILE_MAP_CHIP_NONE , 
    TbRectF32(PUZ_TEX_UV_RECT(0,336,12,12)) , // TILE_MAP_CHIP_BLOCK ,
    TbRectF32(PUZ_TEX_UV_RECT(16,416,12,12)) , // 床
    TbRectF32(PUZ_TEX_UV_RECT(0,416,12,12)) , // 床+左壁
    TbRectF32(PUZ_TEX_UV_RECT(36,416,12,12)) , // 床+右壁 
    TbRectF32(PUZ_TEX_UV_RECT(16,452,12,12)) , // 天井
    TbRectF32(PUZ_TEX_UV_RECT(0,452,12,12)) , // 天井+左壁
    TbRectF32(PUZ_TEX_UV_RECT(36,452,12,12)) , // 天井+右壁
    TbRectF32(PUZ_TEX_UV_RECT(0,432,12,12)) , // 左壁
    TbRectF32(PUZ_TEX_UV_RECT(36,432,12,12)) , // 右壁
    TbRectF32(PUZ_TEX_UV_RECT(16,432,12,12)) , // 内側
    TbRectF32(PUZ_TEX_UV_RECT(0,464,12,12)) , // 内側(右壁と床に挟まれ)
    TbRectF32(PUZ_TEX_UV_RECT(20,464,12,12)) , // 内側(左壁と床に挟まれ)
    TbRectF32(PUZ_TEX_UV_RECT(0,484,12,12)) , // 内側(右壁と天井に挟まれ)
    TbRectF32(PUZ_TEX_UV_RECT(20,484,12,12)) , // 内側(左壁と天井に挟まれ)
    TbRectF32(PUZ_TEX_UV_RECT(32,464,16,16)) , // 内側(上)
    TbRectF32(PUZ_TEX_UV_RECT(32,480,16,16)) , // 内側(下)
    TbRectF32(PUZ_TEX_UV_RECT(48,464,16,16)) , // 内側(左)
    TbRectF32(PUZ_TEX_UV_RECT(48,480,16,16)) , // 内側(右)

    TbRectF32(PUZ_TEX_UV_RECT(64,416,12,12)) , // 長ブロック上
    TbRectF32(PUZ_TEX_UV_RECT(64,432,12,12)) ,  // 長ブロック中
    TbRectF32(PUZ_TEX_UV_RECT(64,448,12,12)) , // 長ブロック下
    TbRectF32(PUZ_TEX_UV_RECT(0,496,16,16)) , // 長ブロック左
    TbRectF32(PUZ_TEX_UV_RECT(16,496,16,16)) ,  // 長ブロック中
    TbRectF32(PUZ_TEX_UV_RECT(32,496,16,16)) , // 長ブロック右

    // ブロック移動用
    TbRectF32(PUZ_TEX_UV_RECT(0,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(16,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(32,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(48,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(64,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(80,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(96,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(112,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(128,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(144,352,12,12)) , // パズルブロック ,

    // マップ設置用
    TbRectF32(PUZ_TEX_UV_RECT(0,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(16,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(32,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(48,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(64,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(80,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(96,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(112,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(128,352,12,12)) , // パズルブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(144,352,12,12)) , // パズルブロック ,

    // マップ設置用
    TbRectF32(PUZ_TEX_UV_RECT(160,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(176,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(192,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(208,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(224,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(240,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(256,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(272,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(288,352,12,12)) , // ターゲットブロック ,
    TbRectF32(PUZ_TEX_UV_RECT(304,352,12,12)) , // ターゲットブロック ,
};

// マップチップのUV情報取得
TbRectF32 PuzzleMap::GetMapChipUV( TbUint32 mapChip )
{
    if( mapChip < TB_ARRAY_COUNT_OF(MAP_CHIP_UV_RECT) ){
        return MAP_CHIP_UV_RECT[mapChip];
    }
    return TbRectF32();
}

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleMap::PuzzleMap()
    : Super()
    , m_FallTime(1.0f)
{
    SetUserMapChipAttrTable(MAP_CHIP_ATTR,TB_ARRAY_COUNT_OF(MAP_CHIP_ATTR));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.18
 */
PuzzleMap::~PuzzleMap()
{
    for( BlockList::Iterator ite = m_PlayerBlock.Begin() , endIte = m_PlayerBlock.End();
            ite != endIte; ++ite ) 
    {
        (*ite)->Release();
    }
    m_PlayerBlock.Clear();
}

/*!
 * 初期化
 * @author teco
 * @since 2013.10.10
 */
void PuzzleMap::OnInitialize( TbScriptLuaNode* node )
{
    if( !node ) {
        return;
    }

    if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->GoToChild("judge")) ) {
        return;
    }

    TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToSint32(m_JudgeRect.x,"x"));
    TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToSint32(m_JudgeRect.y,"y"));
    TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToSint32(m_JudgeRect.width,"w"));
    TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToSint32(m_JudgeRect.height,"h"));
    // 可変ステージの場合判定領域が狭くなることがあるので調整
    TbBool firstNone = TB_FALSE;
    TbSint32 endX = m_JudgeRect.x + m_JudgeRect.width;
    for( TbSint32 x = m_JudgeRect.x; x < endX; ++x ){
        TbSint32 chip = GetMapChip(x,m_JudgeRect.y);
        if( !firstNone ){
            if( chip == TILE_MAP_CHIP_NONE ){
                m_JudgeRect.x = x;
                firstNone = TB_TRUE;
            }
        }else{
            if( chip != TILE_MAP_CHIP_NONE ){
                m_JudgeRect.width = x-m_JudgeRect.x;
                break;
            }
        }
    }
    node->GoToParent(1);

    // プロパティ取得
    {
        TbFloat32 val = 0.0f;
        if( TbScriptLuaNode::RESULT_OK == node->TryToFloat32(val,"infoX") ){
            m_InfoDispPos.SetX(val);
        }
        if( TbScriptLuaNode::RESULT_OK == node->TryToFloat32(val,"infoY") ){
            m_InfoDispPos.SetY(val);
        }
    }
}

/*! 
 * 背景描画
 * @author teco
 * @since 2013.10.24
 */
void PuzzleMap::OnDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbUint32 height = GetTileHeight();
    TbUint32 width = GetTileWidth();
    TbFloat32 chipSize = static_cast<TbFloat32>(GetChipSize());
    const TbPointS32& offset = GetOffset();
    TbVector3 posePos;
    poseMatrix.GetTranslation(posePos);

#if CMN_ENABLE_BUILD_100P
    chipSize += 0.1f;
#endif
    for( TbUint32 y = 0; y < height; ++y ) {
        for( TbUint32 x = 0; x < width; ++x ) {
            TbUint32 chip = GetMapChip(x,y);
            if( chip >= TILE_MAP_CHIP_BLOCK )
            {
                TbRectF32 rect( static_cast<TbFloat32>((x+offset.x)*chipSize) + posePos.GetX() , 
                                static_cast<TbFloat32>((y+offset.y)*chipSize) + posePos.GetY() , 
                                static_cast<TbFloat32>(chipSize) , 
                                static_cast<TbFloat32>(chipSize) );
                material->Draw(rect,MAP_CHIP_UV_RECT[chip],TbColor::White(),TbMatrix::IDENTITY);
            }
        }
    }
}

/*!
 * デバッグメニュー生成
 * @author teco
 * @since 2014.05.11
 */
void PuzzleMap::OnCreateDebugMenu( TbDebugMenu* menu )
{
    TbDebugMenuItemF32* debugFallTime = menu->CreateItemF32("FallTime");
    debugFallTime->SetUnit(0.1f);
    debugFallTime->Bind(&m_FallTime);
}
