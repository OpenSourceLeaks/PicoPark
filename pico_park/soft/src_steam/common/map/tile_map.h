/*!
 * タイルマップ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TILE_MAP_H_
#define _INCLUDED_TILE_MAP_H_

#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>
#include "common/map/tile_map.h"
#include <script/lua/tb_script_lua.h>
#include <graphics/sprite/tb_sprite_all.h>

enum TileMapChipType
{
    TILE_MAP_CHIP_INVALID , 
    TILE_MAP_CHIP_NONE , 
    TILE_MAP_CHIP_BLOCK ,

    TILE_MAP_CHIP_FLOOR , 
    TILE_MAP_CHIP_FLOOR_WALL_L , 
    TILE_MAP_CHIP_FLOOR_WALL_R , 

    TILE_MAP_CHIP_CEILING , 
    TILE_MAP_CHIP_CEILING_WALL_L , 
    TILE_MAP_CHIP_CEILING_WALL_R , 

    TILE_MAP_CHIP_WALL_L , 
    TILE_MAP_CHIP_WALL_R , 

    TILE_MAP_CHIP_IN , 
    TILE_MAP_CHIP_IN_LU , 
    TILE_MAP_CHIP_IN_RU ,
    TILE_MAP_CHIP_IN_LD , 
    TILE_MAP_CHIP_IN_RD ,
    TILE_MAP_CHIP_IN_UP , 
    TILE_MAP_CHIP_IN_DW ,
    TILE_MAP_CHIP_IN_LE , 
    TILE_MAP_CHIP_IN_RG ,

    TILE_MAP_CHIP_BLOCK_HU, 
    TILE_MAP_CHIP_BLOCK_HC, 
    TILE_MAP_CHIP_BLOCK_HD ,
    TILE_MAP_CHIP_BLOCK_WL, 
    TILE_MAP_CHIP_BLOCK_WC, 
    TILE_MAP_CHIP_BLOCK_WR ,

    TILE_MAP_CHIP_USER_BEGIN ,
};

struct TileMapChip
{
    TileMapChip()
    {
        type = TILE_MAP_CHIP_NONE;
        userData = NULL;
    }
    TileMapChip( TbUint32 t ) {
        type = t;
        userData = NULL;
    }
    TbUint32    type;
    void*       userData;
};

enum TileMapChipAttr
{
    TILE_MAP_CHIP_ATTR_COLLISION = TB_BIT(0) , // 移動可能
    TILE_MAP_CHIP_ATTR_FRONT = TB_BIT(1) , // 前景
};

class TileMap
{
public:

    typedef TileMap Self;

public:

    // コンストラクタ
    TileMap();

    // デストラクタ
    virtual ~TileMap();

public:

    // 初期化
    void Initialize( TbUint32 width , TbUint32 height , TbUint32 chipSize , TbUint32 playerCount = 0 );

    // 初期化
    void Initialize( TbScriptLuaNode* node , TbUint32 playerCount = 0 );

public:

    // 初期化
    virtual void OnInitialize( TbScriptLuaNode* node ) {}

    // マップチップ描画
    virtual void OnUpdate( TbFloat32 deltatime ) {}

protected:

    // 前景描画
    virtual void OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material ) {}

    // 背景描画
    virtual void OnDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

public:

    // マップチップ設定
    void SetMapChip( TbUint32 x , TbUint32 y , TbUint32 type )
    {
        if( x < m_Width && y < m_Height ){
            m_MapData[y*m_Width+x].type = type;
        }
    }

    // マップチップ取得
    TbUint32 GetMapChip( TbUint32 x , TbUint32 y ) const
    {
        if( x < m_Width && y < m_Height ){
            return m_MapData[y*m_Width+x].type;
        }
        return TILE_MAP_CHIP_INVALID;
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
    const TbPointS32& GetOffset() const {
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

public:

    // スプライトシーン設定
    void EnterSpriteScene( TbSpriteScene* scene );

    // スプライトシーン設定
    void ExitSpriteScene();

public:

    // マップチップと衝突しているか
    TbBool IsMovable( TbSint32 x , TbSint32 y ) const;

    // マップチップと衝突しているか
    TbBool IsMovable( TbPointS32& from , TbPointS32& to , TbPointS32* contactPoint = NULL ) const;

    // X軸方向に移動可能か
    TbBool IsMovableX( TbPointS32& point , TbSint32 height , TbPointS32* contactPoint = NULL ) const;

    // Y軸方向に移動可能か
    TbBool IsMovableY( TbPointS32& point , TbSint32 width , TbPointS32* contactPoint = NULL ) const;

protected:

    // ユーザー用マップチップの属性テーブル設定
    void SetUserMapChipAttrTable( const TbUint32* table , TbUint32 count ) {
        m_UserMapChipAttrTable = table;
        m_UserMapChipAttrCount = count;
    }

    // マップチップの属性取得
    TbUint32 GetMapChipAttr( TbUint32 x , TbUint32 y ) const;

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
    };

private: 
    
    TbBitArray32        m_BitArray;
    TbUint32            m_Width;
    TbUint32            m_Height;
    TbUint32            m_ChipSize;
    TbPointS32          m_Offset;
    TbUint32            m_PlayerCount;
    TileMapChip*        m_MapData;
    const TbUint32*     m_UserMapChipAttrTable; 
    TbUint32            m_UserMapChipAttrCount;
    TbFloat32           m_Scroll;
    TbSpriteSceneHooker m_SpriteHookerBack;    // スプライト処理のフックオブジェクト前景
    TbSpriteSceneHooker m_SpriteHookerFront;   // スプライト処理のフックオブジェクト背景

};

#endif
