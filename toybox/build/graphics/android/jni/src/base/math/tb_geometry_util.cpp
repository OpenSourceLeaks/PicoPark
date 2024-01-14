/*!
 * @file
 * @brief ジオメトリ系ユーティリティ関数
 * @author Miyake Shunsuke
 */

#include <tb_fwd.h>
#include <base/math/tb_geometry_util.h>
#include <base/math/tb_math_util.h>
#include <base/math/tb_vector2.h>

namespace toybox
{

/*!
 * 線の整数座標配列サイズを取得(ブレゼンハムアルゴリズム)
 * @author Miyake Shunsuke
 * @since 2013.09.28
 */
TbUint32 TbGetLinePointCount( const TbPointS32& start , 
                              const TbPointS32& end )
{
    TbSint32 deltaX = TbAbs(end.x - start.x);
    TbSint32 deltaY = TbAbs(end.y - start.y);
    return TbMax(deltaX,deltaY);
}

/*!
 * 線を整数座標配列で取得(ブレゼンハムアルゴリズム)
 * @author Miyake Shunsuke
 * @since 2013.09.28
 */
TbUint32 TbGetLinePointArray( TbPointS32* pointArray , 
                          TbUint32 pointCount , 
                          const TbPointS32& start , 
                          const TbPointS32& end )
{
    if( 0 == pointCount || !pointArray ){
        return 0;
    }
    TbSint32 nextX = start.x;
    TbSint32 nextY = start.y;
    TbSint32 deltaX = end.x - start.x;
    TbSint32 deltaY = end.y - start.y;
    TbUint32 step = 0;
    TbSint32 stepX = TbSign(deltaX);
    TbSint32 stepY = TbSign(deltaY);
    deltaX = TbAbs(deltaX*2);
    deltaY = TbAbs(deltaY*2);

    // 開始点設定
    pointArray[step].x = nextX;
    pointArray[step].y = nextY;
    ++step;
    if( pointCount <= step ){
        // バッファが足りない
        return step;
    }

    if( deltaX > deltaY )
    {
        // X方向に伸びていく
        TbSint32 fraction = deltaY - (deltaX/2);
        while( nextX != end.x )
        {
            if( fraction >= 0 ) {
                nextY += stepY;
                fraction -= deltaX;
            }
            nextX += stepX;
            fraction += deltaY;
            pointArray[step].x = nextX;
            pointArray[step].y = nextY;
            ++step;
            if( pointCount <= step ){
                // バッファが足りない
                return step;
            }
        }
    }
    else
    {
        // Y方向に伸びていく
        TbSint32 fraction = deltaX - (deltaY/2);
        while( nextY != end.y )
        {
            if( fraction >= 0 ) {
                nextX += stepX;
                fraction -= deltaY;
            }
            nextY += stepY;
            fraction += deltaX;
            pointArray[step].x = nextX;
            pointArray[step].y = nextY;
            ++step;
            if( pointCount <= step ){
                // バッファが足りない
                return step;
            }
        }
    }
    return step;
}

/*!
 * 半円の整数座標配列で取得
 * @author Miyake Shunsuke
 * @since 2013.09.28
 */
TbUint32 TbGetEllipseePointCount( TbSint32 width , 
                                  TbSint32 height , 
                                  const TbPointS32& center )
{
    width = TbAbs(width);
    height = TbAbs(height);
    // 直線
    if( width == 0 && height == 0 ){
        return 1;
    }else if( width == 0 ){
        return 2*height;
    }else if( height == 0 ){
        return 2*width;
    }
    TbUint32 count = 2;
    TbSint32 widthSq = width * width;
    TbSint32 heightSq = height * height;
    TbSint32 wx = 0;
    TbSint32 wy = height;
    TbSint32 xa = 0;
    TbSint32 ya = widthSq * 2 * height;
    TbSint32 thresh = widthSq / 4 - widthSq * height;

    while(1) {
        thresh += xa + heightSq;

        if (thresh >= 0) {
            ya -= widthSq * 2;
            thresh -= ya;
            wy--;
        }

        xa += heightSq * 2;
        wx++;

        if (xa >= ya){
            break;
        }
        count+=4;
    }
    if( width > wx && wy == 0 ){
        count = (width-wx)*2;
    }

    count += 2;
    wx = width;
    wy = 0;
    xa = heightSq * 2 * width;

    ya = 0;
    thresh = heightSq / 4 - heightSq * width;

    while(1) {
        thresh += ya + widthSq;

        if (thresh >= 0) {
            xa -= heightSq * 2;
            thresh = thresh - xa;
            wx--;
        }

        ya += widthSq * 2;
        wy++;

        if (ya > xa){
            break;
        }

        count += 4;
    }
    if( height > wy && wx == 0 ){
        count = (height-wy)*2;
    }
    return count;
}

/*!
 * 楕円を整数座標配列で取得
 * @author Miyake Shunsuke
 * @since 2013.09.28
 */
TbUint32 TbGetEllipsePointArray( TbPointS32* pointArray , 
                                 TbUint32 pointCount ,
                                 const TbPointS32& center ,
                                 TbSint32 width , 
                                 TbSint32 height)
{
    return TbGetEllipsePointArray( pointArray , 
                                   pointCount , 
                                   center , 
                                   width , 
                                   height , 
                                   0 , 
                                   TB_PI_ANG32*2 ,
                                   width , 
                                   height );
}

#define ADD_POINT(newX,newY) \
    pointArray[count].x = (newX); \
    pointArray[count].y = (newY); \
    ++count; \
    if( pointCount <= count ) { return count; }

#define ADD_POINT_ANG(newX,newY,isRange) \
    if( isRange ) { \
        pointArray[count].x = (newX); \
        pointArray[count].y = (newY); \
        ++count; \
        if( pointCount <= count ) { return count; } \
    }

// 半円を整数座標配列で取得(角度制限、半径制限付き)
TbUint32 TbGetEllipsePointArray( TbPointS32* pointArray , 
                                 TbUint32 pointCount ,
                                 const TbPointS32& center ,
                                 TbSint32 width , 
                                 TbSint32 height , 
                                 TbAngle32 beginAng , 
                                 TbAngle32 endAng , 
                                 TbSint32  widthLimit , 
                                 TbSint32  heightLimit
                               )
{
    TbAngle32 ANG_90 = TB_PI_ANG32/2;
    TbAngle32 ANG_180 = TB_PI_ANG32;
    TbAngle32 ANG_270 = TB_PI_ANG32+TB_PI_ANG32/2;
    TbAngle32 ANG_360 = TB_PI_ANG32*2;

    // 角度範囲用情報
    TB_RESQUE_RET(beginAng%(TB_PI_ANG32/2)==0,0); // まだ90度単位でしか未対応
    TB_RESQUE_RET(endAng%(TB_PI_ANG32/2)==0,0);
    TB_RESQUE_RET(beginAng<=endAng,0);
    TB_RESQUE_RET(-ANG_90<=beginAng&&beginAng<=ANG_360,0); // -90 - 450まで対応
    TB_RESQUE_RET(0<=endAng&&endAng<=ANG_360+ANG_90,0);

    TbBool isAngRange0to90 = ( (beginAng <= 0 && (ANG_90) <= endAng) || 
                               (beginAng <= ANG_360 && (ANG_360+ANG_90) <= endAng) );
    TbBool isAngRange90to180 = ( beginAng <= (ANG_90) && (ANG_180) <= endAng );
    TbBool isAngRange180to270 = ( beginAng <= (ANG_180) && (ANG_270) <= endAng );
    TbBool isAngRange270to360 = ( (beginAng <= (ANG_270) && ANG_360 <= endAng) || 
                                  (beginAng <= -ANG_90 && 0 <= endAng) );

    // A Fast Bresenham Type Algorithm For Drawing Ellipse
    width = TbAbs(width);
    height = TbAbs(height);
    widthLimit = TbMin(width,TbAbs(widthLimit));
    heightLimit = TbMin(height,TbAbs(heightLimit));
    
    if( 0 == pointCount || !pointArray ){
        return 0;
    }
    TbUint32 count = 0;

    // 点
    if( width == 0 && height == 0 ){
        ADD_POINT(center.x,center.y);
        return count;
    // 直線
    }else if( width == 0 ){
        // 縦
        // 角度制限
        if( isAngRange0to90 || isAngRange270to360 )
        {
            for( TbSint32 i = -heightLimit; i < 0; ++i ) {
                ADD_POINT(center.x,center.y+i);
            }
        }

        // 角度制限
        if( isAngRange90to180 || isAngRange180to270 )
        {
            for( TbSint32 i = 0; i < heightLimit; ++i ) {
                ADD_POINT(center.x,center.y+i);
            }
        }

        return count;
    }else if( height == 0 ){
        // 横

        // 角度制限
        if( isAngRange180to270 || isAngRange270to360 )
        {
            for( TbSint32 i = -widthLimit; i < 0; ++i ) {
                ADD_POINT(center.x+i,center.y);
            }
        }

        // 角度制限
        if( isAngRange0to90 || isAngRange90to180 )
        {
            for( TbSint32 i = 0; i < widthLimit; ++i ) {
                ADD_POINT(center.x+i,center.y);
            }
        }
        return count;
    }

    // y方向から
    ADD_POINT_ANG(center.x, center.y+heightLimit,isAngRange90to180 || isAngRange180to270);
    ADD_POINT_ANG(center.x, center.y-heightLimit,isAngRange0to90 || isAngRange270to360);
    TbSint32 widthSq = width * width;
    TbSint32 heightSq = height * height;
    TbSint32 wx = 0;
    TbSint32 wy = height;
    TbSint32 xa = 0;
    TbSint32 ya = widthSq * 2 * height;
    TbSint32 thresh = widthSq / 4 - widthSq * height;

    while(1) {
        thresh += xa + heightSq;

        if (thresh >= 0) {
            ya -= widthSq * 2;
            thresh -= ya;
            wy--;
        }

        xa += heightSq * 2;
        wx++;

        if (xa >= ya){
            break;
        }

        TbSint32 wxLimit = TbMin(wx,widthLimit);
        TbSint32 wyLimit = TbMin(wy,heightLimit);
        ADD_POINT_ANG(center.x+wxLimit, center.y-wyLimit,isAngRange0to90);
        ADD_POINT_ANG(center.x+wxLimit, center.y+wyLimit,isAngRange90to180);
        ADD_POINT_ANG(center.x-wxLimit, center.y+wyLimit,isAngRange180to270);
        ADD_POINT_ANG(center.x-wxLimit, center.y-wyLimit,isAngRange270to360);
    }

    while( widthLimit > wx && wy == 0 ) {
        ADD_POINT_ANG(center.x+wx, center.y,isAngRange0to90 || isAngRange90to180);
        ADD_POINT_ANG(center.x-wx, center.y,isAngRange180to270 || isAngRange270to360);
        ++wx;
    }

    // x方向から
    ADD_POINT_ANG(center.x+widthLimit, center.y,isAngRange0to90 || isAngRange90to180);
    ADD_POINT_ANG(center.x-widthLimit, center.y,isAngRange180to270 || isAngRange270to360);
    wx = width;
    wy = 0;
    xa = heightSq * 2 * width;

    ya = 0;
    thresh = heightSq / 4 - heightSq * width;

    while(1) {
        thresh += ya + widthSq;

        if (thresh >= 0) {
            xa -= heightSq * 2;
            thresh = thresh - xa;
            wx--;
        }

        ya += widthSq * 2;
        wy++;

        if (ya > xa){
            break;
        }

        TbSint32 wxLimit = TbMin(wx,widthLimit);
        TbSint32 wyLimit = TbMin(wy,heightLimit);
        ADD_POINT_ANG(center.x+wxLimit, center.y-wyLimit,isAngRange0to90);
        ADD_POINT_ANG(center.x+wxLimit, center.y+wyLimit,isAngRange90to180);
        ADD_POINT_ANG(center.x-wxLimit, center.y+wyLimit,isAngRange180to270);
        ADD_POINT_ANG(center.x-wxLimit, center.y-wyLimit,isAngRange270to360);
    }
    while( heightLimit > wy && wx == 0 ) {
        ADD_POINT_ANG(center.x, center.y+wy,isAngRange90to180 || isAngRange180to270);
        ADD_POINT_ANG(center.x, center.y-wy,isAngRange0to90 || isAngRange270to360);
        ++wy;
    }
    return count;
}

/*!
 * 交差判定
 * @author Miyake Shunsuke
 */
TbBool TbIsGeometryContacted( const TbRectF32& c1 , const TbRectF32& c2 )
{
    if( c1.x < (c2.x+c2.width) &&
        c2.x < (c1.x+c1.width) &&
        c1.y < (c2.y+c2.height) &&
        c2.y < (c1.y+c1.height) )
    {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 交差判定
 * @author Miyake Shunsuke
 */
TbBool TbIsGeometryContacted( const TbCircleF32& c1 , const TbCircleF32& c2 )
{
    TbVector2 delta( c1.x-c2.x , c1.y-c2.y );
    if( delta.Length() < (c1.radius+c2.radius) )
    {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 交差判定
 * @author Miyake Shunsuke
 */
TbBool TbIsGeometryContacted( const TbRectF32& c1 , const TbCircleF32& c2 )
{
    TB_ASSERT(!"");
    return TB_FALSE;
}

};
