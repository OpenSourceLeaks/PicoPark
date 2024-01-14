/*!
 * 描画に関わる行列ユーティリティ関数
 * @author Miyake Shunsuke
 * @since 2011.10.26
 */

#include "tb_fwd.h"
#include "graphics/math/tb_gr_matrix_util.h"
#include "graphics/render/3dapi/tb_3dapi_types.h"

namespace toybox
{

/*!
 * 正射影行列
 * @author Miyake Shunsuke
 * @since 2012.02.25
 */
TbResult TbMatrixSetOrthoLH( TbMatrix& matrix , TbFloat32 width , TbFloat32 height , TbFloat32 znear , TbFloat32 zfar )
{
    if( TbAbs( znear - zfar ) > TB_EPSILON )
    {
#if ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS )
        matrix.Set( 2.0f/width , 0.0f        , 0.0f , 0.0f , 
                    0.0f       , 2.0f/height , 0.0f , 0.0f , 
                    0.0f       , 0.0f        , 1.0f/(zfar-znear) , 0.0f ,
                    0.0f       , 0.0f        , -znear/(zfar-znear) , 1.0f );
#elif ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_MINUS_AND_PLUS )
        matrix.Set( 2.0f/width , 0.0f   , 0.0f                       , 0.0f , 
                    0.0f   , 2.0f/height , 0.0f                       , 0.0f , 
                    0.0f   , 0.0f   , 2.0f/(zfar-znear)          , 0.0f ,
                    0.0f   , 0.0f   , -(znear+zfar)/(zfar-znear) , 1.0f );
#else
    #error Invalid TB_DEPTH_VALUE_TYPE
#endif
        return TB_S_OK;
    }
    return TB_E_INVALIDARG;
}

/*!
 * 正射影行列
 * @author Miyake Shunsuke
 * @since 2012.02.25
 */
TbResult TbMatrixSetOrthoRH( TbMatrix& matrix , TbFloat32 width , TbFloat32 height , TbFloat32 znear , TbFloat32 zfar )
{
    return TbMatrixSetOrthoLH( matrix , width , height , znear , zfar );
}

/*!
 * 正射影行列
 * @author Miyake Shunsuke
 * @since 2012.02.25
 */
TbResult TbMatrixSetOrthoOffCenterLH( TbMatrix& matrix , TbFloat32 left , TbFloat32 right , TbFloat32 top , TbFloat32 bottom , TbFloat32 znear , TbFloat32 zfar )
{
    if( TbAbs( znear - zfar ) > TB_EPSILON )
    {
#if ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS )
        matrix.Set( 2.0f/(right-left)        , 0.0f                     , 0.0f , 0.0f , 
                    0.0f                     , 2.0f/(top-bottom)        , 0.0f , 0.0f , 
                    0.0f                     , 0.0f                     , 1.0f/(zfar-znear) , 0.0f ,
                    (left+right)/(left-right), -(top+bottom)/(bottom-top), -znear/(zfar-znear) , 1.0f );
#elif ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_MINUS_AND_PLUS )
        matrix.Set( 2.0f/(right-left)        , 0.0f                     , 0.0f , 0.0f , 
                    0.0f                     , 2.0f/(top-bottom)        , 0.0f , 0.0f , 
                    0.0f                     , 0.0f                     , 2.0f/(zfar-znear) , 0.0f ,
                    (left+right)/(left-right), -(top+bottom)/(bottom-top), -znear/(zfar-znear) , 1.0f );
#else
    #error Invalid TB_DEPTH_VALUE_TYPE
#endif
        return TB_TRUE;
    }
    return TB_E_INVALIDARG;
}

/*!
 * 正射影行列
 * @author Miyake Shunsuke
 * @since 2012.02.25
 */
TbResult TbMatrixSetOrthoOffCenterRH( TbMatrix& matrix , TbFloat32 left , TbFloat32 right , TbFloat32 top , TbFloat32 bottom , TbFloat32 znear , TbFloat32 zfar )
{
    return TbMatrixSetOrthoOffCenterLH( matrix , left , right , top , bottom , znear , zfar );
}


/*!
 * 透視射影行列( 左手座標系 )
 * @author Miyake Shunsuke
 * @since 2011.03.15
 */
TbResult TbMatrixSetPerspectiveLH( TbMatrix& matrix , TbAngle32 fovy,TbFloat32 aspect,TbFloat32 znear,TbFloat32 zfar)
{
    TbFloat32 h = 1.0f/TbTan(TbAng32ToRad(fovy)/2.0f);
    TbFloat32 w = h / aspect;
    if( TbAbs( znear - zfar ) > TB_EPSILON )
    {
#if ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS )
        matrix.Set( w , 0.0f , 0.0f , 0.0f , 
                   0.0f , h , 0.0f , 0.0f , 
                   0.0f , 0.0f , zfar/(zfar-znear) , 1.0f ,
                   0.0f , 0.0f , -znear*zfar/(zfar-znear) , 0.0f );
#elif ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_MINUS_AND_PLUS )
        matrix.Set( w , 0.0f , 0.0f , 0.0f , 
                   0.0f , h , 0.0f , 0.0f , 
                   0.0f , 0.0f , (zfar + znear)/(zfar-znear) , 1.0f ,
                   0.0f , 0.0f , -2.0f*znear*zfar/(zfar-znear) , 0.0f );
#else
    #error Invalid TB_DEPTH_VALUE_TYPE
#endif
        return TB_S_OK;
    }
    return TB_E_INVALIDARG;
}

/*!
 * 透視射影行列( 右手座標系 )
 * @author Miyake Shunsuke
 * @since 2011.03.15
 */
TbResult TbMatrixSetPerspectiveRH(TbMatrix& matrix , TbAngle32 fovy,TbFloat32 aspect,TbFloat32 znear,TbFloat32 zfar)
{
    TbFloat32 h = 1.0f/TbTan(TbAng32ToRad(fovy)/2.0f);
    TbFloat32 w = h / aspect;
    if( TbAbs( znear - zfar ) > TB_EPSILON )
    {
#if ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_ZERO_AND_PLUS )
        matrix.Set( w , 0.0f , 0.0f , 0.0f , 
                    0.0f , h , 0.0f , 0.0f , 
                    0.0f , 0.0f , zfar/(znear-zfar) , -1.0f ,
                    0.0f , 0.0f , znear*zfar/(znear-zfar) , 0.0f );
#elif ( TB_DEPTH_VALUE_TYPE == TB_DEPTH_VALUE_TYPE_MINUS_AND_PLUS )
        matrix.Set( w , 0.0f , 0.0f , 0.0f , 
                    0.0f , h , 0.0f , 0.0f , 
                    0.0f , 0.0f , (zfar + znear)/(zfar-znear) , 1.0f ,
                    0.0f , 0.0f , -2.0f*znear*zfar/(zfar-znear) , 0.0f );
#else
    #error Invalid TB_DEPTH_VALUE_TYPE
#endif
        return TB_S_OK;
    }
    return TB_E_INVALIDARG;
}

/*!
 * ビュー行列
 * @author Miyake Shunsuke
 * @since 2011.03.15
 */
TbResult TbMatrixSetLookAtLH(TbMatrix& matrix ,  const TbVector4& eye , const TbVector4& at , const TbVector4& up )
{
    TbVector4 zaxis = at - eye;
    zaxis.Normalize();
    TbVector4 xaxis = up.Cross(zaxis);
    xaxis.Normalize();
    TbVector4 yaxis = zaxis.Cross(xaxis);
    yaxis.Normalize();

    matrix.Set( xaxis.GetX() , yaxis.GetX() , zaxis.GetX() , 0.0f , 
                xaxis.GetY() , yaxis.GetY() , zaxis.GetY() , 0.0f , 
                xaxis.GetZ() , yaxis.GetZ() , zaxis.GetZ() , 0.0f , 
                -xaxis.Dot(eye) , -yaxis.Dot(eye) , -zaxis.Dot(eye) , 1.0f );
    return TB_S_OK;
}

/*!
 * ビュー行列
 * @author Miyake Shunsuke
 * @since 2011.03.15
 */
TbResult TbMatrixSetLookAtRH(TbMatrix& matrix , const TbVector4& eye , const TbVector4& at , const TbVector4& up )
{
    TbVector4 zaxis = eye - at;
    zaxis.Normalize();
    TbVector4 xaxis = up.Cross(zaxis);
    xaxis.Normalize();
    TbVector4 yaxis = zaxis.Cross(xaxis);
    yaxis.Normalize();

    matrix.Set( xaxis.GetX() , yaxis.GetX() , zaxis.GetX() , 0.0f , 
                xaxis.GetY() , yaxis.GetY() , zaxis.GetY() , 0.0f , 
                xaxis.GetZ() , yaxis.GetZ() , zaxis.GetZ() , 0.0f , 
                -xaxis.Dot(eye) , -yaxis.Dot(eye) , -zaxis.Dot(eye) , 1.0f );
    return TB_S_OK;
}

}