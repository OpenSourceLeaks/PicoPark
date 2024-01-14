/*!
 * 描画に関わる行列ユーティリティ関数
 * @author Miyake Shunsuke
 * @since 2011.10.26
 */

#ifndef _INCLUD_TB_GR_MATRIX_UTIL_H_
#define _INCLUD_TB_GR_MATRIX_UTIL_H_

#include "base/math/tb_matrix.h"
#include "base/math/tb_vector4.h"

namespace toybox
{

//正射影行列
TbResult TbMatrixSetOrthoLH( TbMatrix& matix , TbFloat32 width , TbFloat32 height , TbFloat32 znear , TbFloat32 zfar );
TbResult TbMatrixSetOrthoRH( TbMatrix& matix , TbFloat32 width , TbFloat32 height , TbFloat32 znear , TbFloat32 zfar );

//正射影行列
TbResult TbMatrixSetOrthoOffCenterLH( TbMatrix& matix , TbFloat32 left , TbFloat32 right , TbFloat32 top , TbFloat32 bottom , TbFloat32 znear , TbFloat32 zfar );
TbResult TbMatrixSetOrthoOffCenterRH( TbMatrix& matix , TbFloat32 left , TbFloat32 right , TbFloat32 top , TbFloat32 bottom , TbFloat32 znear , TbFloat32 zfar );

//透視射影行列
TbResult TbMatrixSetPerspectiveLH( TbMatrix& matix , TbAngle32 fovy,TbFloat32 aspect,TbFloat32 znear,TbFloat32 zfar );
TbResult TbMatrixSetPerspectiveRH( TbMatrix& matix , TbAngle32 fovy,TbFloat32 aspect,TbFloat32 znear,TbFloat32 zfar );

//ビュー行列
TbResult TbMatrixSetLookAtLH( TbMatrix& matix , const TbVector4& eye , const TbVector4& at , const TbVector4& up );
TbResult TbMatrixSetLookAtRH( TbMatrix& matix , const TbVector4& eye , const TbVector4& at , const TbVector4& up );

}

#endif
