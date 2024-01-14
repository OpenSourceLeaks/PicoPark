/*!
 * アクション用マップ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_ACTION_MAP_H_
#define _INCLUDED_CR_ACTION_MAP_H_

#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>

#include <crayon/cr_def.h>
#include <crayon/cr_util.h>
#include <crayon/map/cr_tile_map.h>

namespace crayon
{

enum
{
    CR_ACTION_MAP_CHIP_DOKAN_LU = CR_TILE_MAP_CHIP_USER_BEGIN , 
    CR_ACTION_MAP_CHIP_DOKAN_LD , 
    CR_ACTION_MAP_CHIP_DOKAN_RU , 
    CR_ACTION_MAP_CHIP_DOKAN_RD , 

    CR_ACTION_MAP_CHIP_BREAK1   , // 壊れるブロック
    CR_ACTION_MAP_CHIP_BREAK2   , // 壊れるブロック
    CR_ACTION_MAP_CHIP_BREAK3   , // 壊れるブロック
    CR_ACTION_MAP_CHIP_BREAK4   , // 壊れるブロック
    CR_ACTION_MAP_CHIP_BREAK5, // 壊れるブロック(速度アップ)

    CR_ACTION_MAP_CHIP_END ,
    CR_ACTION_MAP_CHIP_MAX = CR_ACTION_MAP_CHIP_END - CR_TILE_MAP_CHIP_USER_BEGIN ,
    CR_ACTION_MAP_CHIP_BREAK_BEGIN = CR_ACTION_MAP_CHIP_BREAK1 , 
    CR_ACTION_MAP_CHIP_BREAK_END = CR_ACTION_MAP_CHIP_BREAK5+1 ,
};

class CrActionCollision;

class CrActionMap : public CrTileMap
{
public:
    
    typedef CrTileMap Super;

public:

    typedef toybox::TbStaticFunction32<void (const toybox::TbPointS32& point , const toybox::TbVector2& dir)> HittedMapChipCallback;

public:

    // コンストラクタ
    CrActionMap(const toybox::TbRectF32* mapUvRecSTable);

    // デストラクタ
    virtual ~CrActionMap();

public:

    // アクターを移動する
    toybox::TbVector2 Move( CrActionCollision* collision , const toybox::TbVector2& mov , toybox::TbVector2* resultCorrectMov = NULL );

    // どこかが接触している
    TbBool IsContacted(const CrActionCollision* collision) const;

    // 移動不可能チップと接しているか
    TbBool IsContacted( const CrActionCollision* collision , const toybox::TbVector2& dir , toybox::TbPointS32* contactPoint = NULL ) const;

    // 移動不可能チップと接しているか
    TbBool IsContacted( const CrActionCollision* collision , CrContactFace face , toybox::TbPointS32* contactPoint = NULL ) const {
        return IsContacted(collision,CrUtil::GetContactFaceNormal(face),contactPoint);
    }

    // 指定マップチップは存在するか
    TbBool Exists(TbUint32 beginChip, TbUint32 endChip) const;

protected:

    // 背景描画
    virtual void OnDrawBack( const toybox::TbMatrix& poseMatrix , toybox::TbSpriteMaterial* material ) TB_OVERRIDE;

    // 前景描画
    virtual void OnDrawFront( const toybox::TbMatrix& poseMatrix , toybox::TbSpriteMaterial* material ) TB_OVERRIDE;

private:

    const toybox::TbRectF32*  m_UvRect;

};

}

#endif
