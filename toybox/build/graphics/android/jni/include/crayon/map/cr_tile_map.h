/*!
 * タイルマップ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_TILE_MAP_H_
#define _INCLUDED_CR_TILE_MAP_H_

#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>
#include <script/lua/tb_script_lua.h>
#include <graphics/sprite/tb_sprite_all.h>

namespace crayon
{

enum CrTileMapChipTypeA
{
    CR_TILE_MAP_CHIP_INVALID , 
    CR_TILE_MAP_CHIP_NONE , 
    CR_TILE_MAP_CHIP_BLOCK ,

    CR_TILE_MAP_CHIP_FLOOR , 
    CR_TILE_MAP_CHIP_FLOOR_WALL_L , 
    CR_TILE_MAP_CHIP_FLOOR_WALL_R , 

    CR_TILE_MAP_CHIP_CEILING , 
    CR_TILE_MAP_CHIP_CEILING_WALL_L , 
    CR_TILE_MAP_CHIP_CEILING_WALL_R , 

    CR_TILE_MAP_CHIP_WALL_L , 
    CR_TILE_MAP_CHIP_WALL_R , 

    CR_TILE_MAP_CHIP_IN , 
    CR_TILE_MAP_CHIP_IN_LU , 
    CR_TILE_MAP_CHIP_IN_RU ,
    CR_TILE_MAP_CHIP_IN_LD , 
    CR_TILE_MAP_CHIP_IN_RD ,
    CR_TILE_MAP_CHIP_IN_UP , 
    CR_TILE_MAP_CHIP_IN_DW ,
    CR_TILE_MAP_CHIP_IN_LE , 
    CR_TILE_MAP_CHIP_IN_RG ,

    CR_TILE_MAP_CHIP_BLOCK_HU, 
    CR_TILE_MAP_CHIP_BLOCK_HC, 
    CR_TILE_MAP_CHIP_BLOCK_HD ,
    CR_TILE_MAP_CHIP_BLOCK_WL, 
    CR_TILE_MAP_CHIP_BLOCK_WC, 
    CR_TILE_MAP_CHIP_BLOCK_WR ,

    CR_TILE_MAP_CHIP_USER_BEGIN ,
};

struct CrTileMapChip
{
    CrTileMapChip()
    {
        type = CR_TILE_MAP_CHIP_NONE;
        userData = NULL;
    }
    CrTileMapChip( TbUint32 t ) {
        type = t;
        userData = NULL;
    }
    TbUint32    type;
    void*       userData;
};

enum CrTileMapChipAttr
{
    CR_TILE_MAP_CHIP_ATTR_COLLISION = TB_BIT(0) , // 移動可能
    CR_TILE_MAP_CHIP_ATTR_FRONT = TB_BIT(1) , // 前景
};

class CrTileMap
{
public:

    typedef CrTileMap Self;

public:

    // コンストラクタ
    CrTileMap();

    // デストラクタ
    virtual ~CrTileMap();

public:

    // 初期化
    void Initialize( TbUint32 width , TbUint32 height , TbUint32 chipSize , TbUint32 playerCount = 0 );

    // 初期化
    void Initialize( toybox::TbScriptLuaNode* node , TbUint32 playerCount = 0 );

public:

    // マップチップ設定
    void SetMapChip( TbUint32 x , TbUint32 y , TbUint32 type )
    {
        if( x < m_Width && y < m_Height ){
            m_MapData[y*m_Width+x].type = type;
        }
    }

    // マップチップ設定
    void ReplaceMapChip(TbUint32 before, TbUint32 after)
    {
        for (TbUint32 y = 0; y < m_Height; ++y)
        {
            for (TbUint32 x = 0; x < m_Width; ++x)
            {
                if (before == GetMapChip(x, y))
                {
                    SetMapChip(x,y,after);
                }
            }
        }
    }

    // マップチップ取得
    TbUint32 GetMapChip( TbUint32 x , TbUint32 y ) const
    {
        if( x < m_Width && y < m_Height ){
            return m_MapData[y*m_Width+x].type;
        }
        return CR_TILE_MAP_CHIP_INVALID;
    };

    // マップチップにユーザーデータ設定
    void SetMapChipUserData( TbUint32 x , TbUint32 y , void* userData )
    {
        if( x < m_Width && y < m_Height ){
            m_MapData[y*m_Width+x].userData = userData;
        }
    }

    // マップチップユーザーデータ取得
    void* GetMapChipUserData( TbUint32 x , TbUint32 y )
    {
        if( x < m_Width && y < m_Height ){
            return m_MapData[y*m_Width+x].userData;
        }
        return NULL;
    };

    // チップサイズ取得
    TbUint32 GetChipSize() const {
        return m_ChipSize;
    }

    // 横幅取得
    TbUint32 GetTileWidth() const {
        return m_Width;
    }

    // 縦幅取得
    TbUint32 GetTileHeight() const {
        return m_Height;
    }

    // 横幅取得
    TbUint32 GetWidth() const {
        return m_Width * m_ChipSize;
    }

    // 縦幅取得
    TbUint32 GetHeight() const {
        return m_Height * m_ChipSize;
    }

    // オフセット座標
    const toybox::TbPointS32& GetOffset() const {
        return  m_Offset;
    }

    // スクロール設定
    void SetScroll( TbFloat32 scroll ) {
        m_Scroll = scroll;
    }

    // スクロール取得
    TbFloat32 GetScroll() const {
        return m_Scroll;
    }

    // カリング矩形設定
    void SetCullingRect(const toybox::TbRectU32& rect) {
        m_CullingRect = rect;
    }

    // カリングしているか
    TbBool IsCulling(TbUint32 x, TbUint32 y) {
        return !(m_CullingRect.x <= x && x < m_CullingRect.x+ m_CullingRect.width) ||
               !(m_CullingRect.y <= y && y < m_CullingRect.y + m_CullingRect.height);
    }

    // 可視設定
    void SetVisible(TbBool isVisible) {
        m_BitArray.SetBit(FLAG_VISIBLE,isVisible);
    }

    // 可視取得
    TbBool IsVisible() const {
        return m_BitArray.Test(FLAG_VISIBLE);
    }

    // 可変マップ取得
    TbBool IsVariable() const {
        return m_BitArray.Test(FLAG_VARIABLE);
    }

public:

    // スプライトシーン設定
    void EnterSpriteScene( toybox::TbSpriteScene* scene );

    // スプライトシーン設定
    void ExitSpriteScene();

public:

    // マップチップと衝突しているか
    TbBool IsMovable( TbSint32 x , TbSint32 y ) const;

    // マップチップと衝突しているか
    TbBool IsMovable( toybox::TbPointS32& from , toybox::TbPointS32& to , toybox::TbPointS32* contactPoint = NULL ) const;

    // X軸方向に移動可能か
    TbBool IsMovableX( toybox::TbPointS32& point , TbSint32 height , toybox::TbPointS32* contactPoint = NULL ) const;

    // Y軸方向に移動可能か
    TbBool IsMovableY( toybox::TbPointS32& point , TbSint32 width , toybox::TbPointS32* contactPoint = NULL ) const;

    // ユーザー用マップチップの属性テーブル設定
    void SetUserMapChipAttrTable( const TbUint32* table , TbUint32 count ) {
        m_UserMapChipAttrTable = table;
        m_UserMapChipAttrCount = count;
    }

    // マップチップの属性取得
    TbUint32 GetMapChipAttr( TbUint32 x , TbUint32 y ) const;

public:

    // 初期化
    virtual void OnInitialize(toybox::TbScriptLuaNode* node) {}

    // マップチップ描画
    virtual void OnUpdate(TbFloat32 deltatime) {}

protected:

    // 前景描画
    virtual void OnDrawFront(const toybox::TbMatrix& poseMatrix, toybox::TbSpriteMaterial* material) {}

    // 背景描画
    virtual void OnDrawBack(const toybox::TbMatrix& poseMatrix, toybox::TbSpriteMaterial* material);

private:

    // 解放
    void release() {
        if( !m_BitArray.Test(FLAG_STATIC_MAP) ){
            TB_SAFE_DELETE_ARRAY(m_MapData);
        }
        m_MapData = NULL;
        m_Width = 0;
        m_Height =0;
        m_SpriteHookerBack.Exit();
        m_SpriteHookerFront.Exit();
    }

private:

    enum Flag
    {
        FLAG_STATIC_MAP ,
        FLAG_VISIBLE , 
        FLAG_VARIABLE ,
    };

private: 
    
    toybox::TbBitArray32        m_BitArray;
    TbUint32                    m_Width;
    TbUint32                    m_Height;
    TbUint32                    m_ChipSize;
    toybox::TbRectU32           m_CullingRect;
    toybox::TbPointS32          m_Offset;
    TbUint32                    m_PlayerCount;
    CrTileMapChip*              m_MapData;
    const TbUint32*             m_UserMapChipAttrTable; 
    TbUint32                    m_UserMapChipAttrCount;
    TbFloat32                   m_Scroll;
    toybox::TbSpriteSceneHooker m_SpriteHookerBack;    // スプライト処理のフックオブジェクト前景
    toybox::TbSpriteSceneHooker m_SpriteHookerFront;   // スプライト処理のフックオブジェクト背景

};

}

#endif
