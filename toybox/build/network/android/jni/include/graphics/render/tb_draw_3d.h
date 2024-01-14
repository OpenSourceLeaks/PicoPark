/*!
 * 3D描画
 * @author Miyake Shunsuke
 * @since 2010.10.30
 */

#ifndef _INCLUDED_TB_DRAW_3D_H_
#define _INCLUDED_TB_DRAW_3D_H_

#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_matrix.h>

namespace toybox
{

// 描画パス設定
TbResult TbDraw3dSetRenderPass( TbUint32 passIndex );

// 描画パス取得
TbUint32 TbDraw3dGetRenderPass();

// 座標系描画
TbResult TbDrawCoordinate( const TbMatrix& mtx );

// ライン描画
TbResult TbDrawLine( const TbVector4& pos1 , const TbVector4& pos2 );

// 箱描画
TbResult TbDrawBox( const TbVector4& pos , TbFloat32 width , TbFloat32 height , TbFloat32 depth , const TbColor& color );

// ワイヤー箱描画
TbResult TbDrawWireBox( const TbVector4& pos , TbFloat32 width , TbFloat32 height , TbFloat32 depth );

// XY平面にグリッド描画
TbResult TbDrawGridXY( const TbVector4& pos , TbFloat32 width , TbFloat32 height );

// YZ平面にグリッド描画
TbResult TbDrawGridY( const TbVector4& pos , TbFloat32 height , TbFloat32 depth );

// ZX平面にグリッド描画
TbResult TbDrawGridZ( const TbVector4& pos , TbFloat32 depth , TbFloat32 width );

}

#endif
