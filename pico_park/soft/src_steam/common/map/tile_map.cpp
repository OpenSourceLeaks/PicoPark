/*!
 * タイルマップ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "common/map/tile_map.h"
#include "common/actor/actor.h"
#include <graphics/render/tb_draw_2d.h>

namespace
{

const TbUint32 MAP_CHIP_DEFAULT_ATTR[] = 
{
    0 ,
    0 ,
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
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(MAP_CHIP_DEFAULT_ATTR)==TILE_MAP_CHIP_USER_BEGIN);

static const TbUint32 BLK = TILE_MAP_CHIP_BLOCK;
static const TbUint32 NON = TILE_MAP_CHIP_NONE;

static const TbUint32 TEST_WIDTH = 16;
static const TbUint32 TEST_SIZE = 32;
static TileMapChip TEST_STAGE[] = 
{
    BLK,NON,BLK,NON,BLK,NON,BLK,NON,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,NON,BLK,
    BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
};

}

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.11
 */
TileMap::TileMap()
    : m_Width(0)
    , m_Height(0)
    , m_ChipSize(0)
    , m_PlayerCount(0)
    , m_MapData(NULL)
    , m_UserMapChipAttrTable(NULL)
    , m_UserMapChipAttrCount(0)
    , m_Scroll(0.0f)
{
#if 1
    m_Width = TEST_WIDTH;
    m_Height = TB_ARRAY_COUNT_OF(TEST_STAGE)/m_Width;
    m_ChipSize = TEST_SIZE;
    m_MapData = TEST_STAGE;
    m_BitArray.SetBit(FLAG_STATIC_MAP);
#endif

    m_SpriteHookerBack.SetDrawFunc(TbCreateMemFunc(this,&Self::OnDrawBack));
    m_SpriteHookerBack.SetZ(CMN_SPRITE_Z_BACK_2);
    m_SpriteHookerFront.SetDrawFunc(TbCreateMemFunc(this,&Self::OnDrawFront));
    m_SpriteHookerFront.SetZ(CMN_SPRITE_Z_FRONT_3);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.11
 */
TileMap::~TileMap()
{
    release();
}

/*!
 * 初期化
 * @author teco
 * @since 2013.09.11
 */
void TileMap::Initialize( TbUint32 width , TbUint32 height , TbUint32 chipSize , TbUint32 playerCount )
{
    release();
    m_Width = width;
    m_Height = height;
    m_ChipSize = chipSize;
    m_PlayerCount = playerCount;
    m_MapData = new TileMapChip[m_Height*m_Width];
    m_BitArray.ResetBit(FLAG_STATIC_MAP);
    OnInitialize(NULL);
}

/*!
 * 初期化
 * @author teco
 * @since 2013.09.28
 */
void TileMap::Initialize( TbScriptLuaNode* node , TbUint32 playerCount)
{
    if( node )
    {
        m_PlayerCount = playerCount;
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->GoToChild("map")) ) {
            return;
        }
        TbUint32 width = 0;
        TbUint32 height = 0;
        TbUint32 size = 0;
        
        // width取得
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToUint32(width,"width")) ) {
            return;
        }

        // height取得
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToUint32(height,"height")) ) {
            return;
        }

        // offset取得
        node->TryToSint32(m_Offset.x,"offsetX");
        node->TryToSint32(m_Offset.y,"offsetY");

        // chipSize取得
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToUint32(size,"chipSize")) ) {
            return;
        }

        // 可変テーブルか取得
        TbSint32 variable = 0;
        if( m_PlayerCount > 0 ){
            node->TryToSint32(variable,"variable");
        }

        // テーブル取得
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->GoToChild("table")) ) {
            return;
        }

        // 可変テーブル参照
        if( variable ){
            if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->GoToChild(m_PlayerCount-1)) ) {
                return;
            }
        }

        // マップ初期化
        Initialize(width,height,size);

        // マップチップ設定
        TbUint32 value = 0;
        for( TbUint32 y = 0; y < height; ++y )
        {
            for( TbUint32 x = 0; x < width; ++x )
            {
                if(!TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToUint32ByIndex(value,x*height+y)))
                {
                    return;
                }
                SetMapChip(x,y,value);
            }
        }
        node->GoToParent(1+variable);
        OnInitialize(node);
        node->GoToParent(1);
    }
}

/*!
 * マップチップと衝突しているか
 * @author teco
 * @since 2013.09.15
 */
TbBool TileMap::IsMovable( TbSint32 x , TbSint32 y ) const
{
    TbSint32 mcX = TbClamp<TbSint32>(x,0,m_Width-1);
    TbSint32 mcY = TbClamp<TbSint32>(y,0,m_Height-1);
    return !(GetMapChipAttr(mcX,mcY) & TILE_MAP_CHIP_ATTR_COLLISION);
}

/*!
 * マップチップと衝突しているか
 * @author teco
 * @since 2013.10.13
 */
TbBool TileMap::IsMovable( TbPointS32& from , TbPointS32& to , TbPointS32* contactPoint ) const
{
    for( TbSint32 y = from.y; y <= to.y; ++y ) {
        for( TbSint32 x = from.x; x <= to.x; ++x ) {
            if( !IsMovable( x , y ) ){
                if( contactPoint ){
                    contactPoint->x = x;
                    contactPoint->y = y;
                }
                return TB_FALSE;
            }
        }
    }
    return TB_TRUE;
}

/*!
 * X軸方向に移動可能か
 * @author teco
 * @since 2013.09.15
 */
TbBool TileMap::IsMovableX( TbPointS32& point , TbSint32 height , TbPointS32* contactPoint ) const
{
    for( TbSint32 y = 0; y < height; ++y ) {
        if( !IsMovable( point.x , point.y+y ) ){
            if( contactPoint ){
                contactPoint->x = point.x;
                contactPoint->y = point.y+y;
            }
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}

/*!
 * マップチップと衝突しているか
 * @author teco
 * @since 2013.09.14
 */
TbBool TileMap::IsMovableY( TbPointS32& point , TbSint32 width , TbPointS32* contactPoint ) const
{
    for( TbSint32 x = 0; x < width; ++x ) {
        if( !IsMovable( point.x+x , point.y ) ){
            if( contactPoint ){
                contactPoint->x = point.x+x;
                contactPoint->y = point.y;
            }
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}

/*!
 * スプライトシーン設定
 * @author teco
 * @since 2013.10.24
 */
void TileMap::EnterSpriteScene( TbSpriteScene* scene )
{
    m_SpriteHookerBack.Enter(scene);
    m_SpriteHookerFront.Enter(scene);
}

/*!
 * スプライトシーン設定
 * @author teco
 * @since 2013.10.24
 */
void TileMap::ExitSpriteScene()
{
    m_SpriteHookerBack.Exit();
    m_SpriteHookerFront.Exit();
}

/*!
 * 背景描画
 * @author teco
 * @since 2013.10.24
 */
void TileMap::OnDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    // デバッグ表示
    for( TbUint32 y = 0; y < m_Height; ++y ) {
        for( TbUint32 x = 0; x < m_Width; ++x ) {
            if( m_MapData[y*m_Width+x].type == BLK )
            {
                TbRectF32 rect( static_cast<TbFloat32>((x+m_Offset.x)*m_ChipSize) - GetScroll() , 
                                static_cast<TbFloat32>((y+m_Offset.y)*m_ChipSize) , 
                                static_cast<TbFloat32>(m_ChipSize) , 
                                static_cast<TbFloat32>(m_ChipSize) );
                material->Draw(rect,TbRectF32(),TbColor::Red(),poseMatrix);
            }
        }
    }
}

/*!
 * マップチップの属性取得
 * @author teco
 * @since 2013.10.24
 */
TbUint32 TileMap::GetMapChipAttr( TbUint32 x , TbUint32 y ) const
{
    TbUint32 type = GetMapChip(x,y);
    if( type < TILE_MAP_CHIP_USER_BEGIN ){
        return MAP_CHIP_DEFAULT_ATTR[type];
    }
    TbUint32 userType = (type-TILE_MAP_CHIP_USER_BEGIN);
    if( userType < m_UserMapChipAttrCount ){
        return m_UserMapChipAttrTable[userType];
    }
    return 0;
}
