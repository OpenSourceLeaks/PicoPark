/*!
 * アクション用マップ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_MAP_H_
#define _INCLUDED_ACTION_MAP_H_

#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>
#include "common/map/tile_map.h"
#include "common/cmn_util.h"

enum
{
    ACTION_MAP_CHIP_DOKAN_LU = TILE_MAP_CHIP_USER_BEGIN , 
    ACTION_MAP_CHIP_DOKAN_LD , 
    ACTION_MAP_CHIP_DOKAN_RU , 
    ACTION_MAP_CHIP_DOKAN_RD , 
    ACTION_MAP_CHIP_BREAK1   , // 壊れるブロック
    ACTION_MAP_CHIP_BREAK2   , 
    ACTION_MAP_CHIP_BREAK3   , 
    ACTION_MAP_CHIP_BREAK4   , 
    ACTION_MAP_CHIP_END ,
    ACTION_MAP_CHIP_MAX = ACTION_MAP_CHIP_END - TILE_MAP_CHIP_USER_BEGIN ,
    ACTION_MAP_CHIP_BREAK_BEGIN = ACTION_MAP_CHIP_BREAK1 , 
    ACTION_MAP_CHIP_BREAK_END = ACTION_MAP_CHIP_BREAK4+1 ,
};

class ActionCollision;

class ActionMap : public TileMap
{
    typedef TileMap Super;

public:

    typedef TbStaticFunction32<void (const TbPointS32& point , const TbVector2& dir)> HittedMapChipCallback;

public:

    // コンストラクタ
    ActionMap();

    // デストラクタ
    virtual ~ActionMap();

public:

    // アクターを移動する
    TbVector2 Move( ActionCollision* collision , const TbVector2& mov , TbVector2* resultCorrectMov = NULL );

    // どこかが接触している
    TbBool IsContacted(const ActionCollision* collision ) const;

    // 移動不可能チップと接しているか
    TbBool IsContacted( const ActionCollision* collision , const TbVector2& dir , TbPointS32* contactPoint = NULL ) const;

    // 移動不可能チップと接しているか
    TbBool IsContacted( const ActionCollision* collision , CmnContactFace face , TbPointS32* contactPoint = NULL ) const {
        return IsContacted(collision,CmnUtil::GetContactFaceNormal(face),contactPoint);
    }

    // 指定マップチップは存在するか
    TbBool Exists( TbUint32 beginChip , TbUint32 endChip ) const;

protected:

    // 背景描画
    virtual void OnDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material ) TB_OVERRIDE;

    // 前景描画
    virtual void OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material ) TB_OVERRIDE;

};

#endif
