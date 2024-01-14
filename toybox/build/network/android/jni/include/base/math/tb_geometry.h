/*!
 * @file
 * @author Miyake Shunsuke
 * @since 2010.4.10
 */
 
#ifndef _INCLUDED_TB_GEOMETRY_H_
#define _INCLUDED_TB_GEOMETRY_H_

#include <base/math/tb_math_util.h>
#include <base/math/tb_vector2.h>

namespace toybox
{

// ポイント
struct TbPointS16
{
    TbPointS16( TbSint16 inX , TbSint16 inY ) 
        : x(inX) , y(inY)
    {}
    TbPointS16() : x(0) , y(0) {}
    TbSint16 x;
    TbSint16 y;
};

// ポイント
struct TbPointS32
{
    TbPointS32( TbSint32 inX , TbSint32 inY ) 
        : x(inX) , y(inY)
    {}
    TbPointS32() : x(0) , y(0) {}

    // 加算演算子
    TbPointS32 operator+( const TbPointS32& val ) const {
        TbPointS32 result;
        result.x = x + val.x;
        result.y = y + val.y;
        return result;
    }

    // 除算演算子
    TbPointS32& operator/=( TbSint32 val ) {
        x /= val;
        y /= val;
        return *this;
    }

    // 乗算演算子
    TbPointS32& operator*=( TbSint32 val ) {
        x *= val;
        y *= val;
        return *this;
    }

    // 比較演算子
    TbBool operator==( const TbPointS32& val ) const {
        return x == val.x && y == val.y;
    }

    // 比較演算子
    TbBool operator!=( const TbPointS32& val ) const {
        return x != val.x || y != val.y;
    }

    TbSint32 x;
    TbSint32 y;
};

// サイズ
struct TbDimensionS16
{
    TbDimensionS16( TbSint16 inW , TbSint16 inH ) 
        : width(inW) , height( inH ) 
    {}
    TbDimensionS16() : width(0) , height(0) {}
    TbSint16 width;
    TbSint16 height;
};

// サイズ
struct TbDimensionS32
{
    TbDimensionS32( TbSint32 inW , TbSint32 inH ) 
        : width(inW) , height( inH ) 
    {}
    TbDimensionS32() : width(0) , height(0) {}
    TbSint32 width;
    TbSint32 height;
};

// サイズ
struct TbDimensionF32
{
    TbDimensionF32( TbFloat32 inW , TbFloat32 inH ) 
        : width(inW) , height( inH ) 
    {}
    TbDimensionF32() : width(0.0f) , height(0.0f) {}
    TbFloat32 width;
    TbFloat32 height;
};

// 矩形
struct TbRectS32
{
    TbRectS32( TbSint32 inX , TbSint32 inY , TbSint32 inW , TbSint32 inH ) 
        : x(inX) , y(inY) , width(inW) , height( inH ) 
    {}
    TbRectS32() : x(0) , y(0) , width(0) , height(0) {}
    TbRectS32& operator/=( TbSint32 val ) {
        x /= val;
        y /= val;
        width /= val;
        height /= val;
        return *this;
    }
    TbRectS32& operator*=( TbSint32 val ) {
        x *= val;
        y *= val;
        width *= val;
        height *= val;
        return *this;
    }
    TbSint32 x;
    TbSint32 y;
    TbSint32 width;
    TbSint32 height;
};

struct TbRectU32
{
    TbRectU32(TbUint32 inX, TbUint32 inY, TbUint32 inW, TbUint32 inH)
        : x(inX), y(inY), width(inW), height(inH)
    {}
    TbRectU32() : x(0), y(0), width(0), height(0) {}
    TbRectU32& operator/=(TbUint32 val) {
        x /= val;
        y /= val;
        width /= val;
        height /= val;
        return *this;
    }
    TbRectU32& operator*=(TbUint32 val) {
        x *= val;
        y *= val;
        width *= val;
        height *= val;
        return *this;
    }
    TbUint32 x;
    TbUint32 y;
    TbUint32 width;
    TbUint32 height;
};


// 矩形
struct TbRectF32Pod
{
    TbFloat32 x;
    TbFloat32 y;
    TbFloat32 width;
    TbFloat32 height;
};

// 矩形
struct TbRectF32
{
    TbRectF32( TbFloat32 inX , TbFloat32 inY , TbFloat32 inW , TbFloat32 inH ) 
        : x(inX) , y(inY) , width(inW) , height( inH ) 
    {}

    TbRectF32() : x(0.0f) , y(0.0f) , width(0.0f) , height(0.0f) {}

    TbRectF32( const TbRectF32Pod& rect , 
               TbFloat32 ofsX = 0.0f , 
               TbFloat32 ofsY = 0.0f , 
               TbFloat32 ofsW = 0.0f , 
               TbFloat32 ofsH =0.0f  ) : x(rect.x+ofsX) , y(rect.y+ofsY) , width(rect.width+ofsW) , height(rect.height+ofsH) {}
    
    // 比較演算子
    TbBool operator==( const TbRectF32& src ) const 
    {
        return TB_FLOAT32_EQUAL(x,src.x) &&
               TB_FLOAT32_EQUAL(y,src.y) &&
               TB_FLOAT32_EQUAL(width,src.width) &&
               TB_FLOAT32_EQUAL(height,src.height);
    }

    // 比較演算子
    TbBool operator!=( const TbRectF32& src ) const 
    {
        return !TB_FLOAT32_EQUAL(x,src.x) ||
               !TB_FLOAT32_EQUAL(y,src.y) ||
               !TB_FLOAT32_EQUAL(width,src.width) ||
               !TB_FLOAT32_EQUAL(height,src.height);
    }

    TbVector2 GetLeftTop() const {
        return TbVector2(x,y);
    }

    TbVector2 GetLeftBottom() const {
        return TbVector2(x, y+height);
    }

    TbVector2 GetRightTop() const {
        return TbVector2(x+width , y );
    }

    TbVector2 GetRightBottom() const {
        return TbVector2(x+width, y+height);
    }

    TbFloat32 x;
    TbFloat32 y;
    TbFloat32 width;
    TbFloat32 height;
};

// 矩形同士の加算演算子
inline TbRectF32 operator+(const TbRectF32& left , const TbRectF32& right )
{
    TbRectF32 result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.width = left.width + right.width;
    result.height = left.height + right.height;
    return result;
}

// 矩形同士の引算演算子
inline TbRectF32 operator-(const TbRectF32& left , const TbRectF32& right )
{
    TbRectF32 result;
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    result.width = left.width - right.width;
    result.height = left.height - right.height;
    return result;
}

// 矩形とfloatの乗算演算子
inline TbRectF32 operator*(const TbRectF32& left , TbFloat32 right )
{
    TbRectF32 result;
    result.x = left.x * right;
    result.y = left.y * right;
    result.width = left.width * right;
    result.height = left.height * right;
    return result;
}

// 矩形とfloatの乗算演算子
inline TbRectF32 operator*(const TbRectF32& left, const TbVector2& right)
{
    TbRectF32 result;
    result.x = left.x * right.GetX();
    result.y = left.y * right.GetY();
    result.width = left.width * right.GetX();
    result.height = left.height * right.GetY();
    return result;
}

// 矩形とfloatの割算演算子
inline TbRectF32 operator/(const TbRectF32& left , TbFloat32 right )
{
    TbRectF32 result;
    result.x = left.x / right;
    result.y = left.y / right;
    result.width = left.width / right;
    result.height = left.height / right;
    return result;
}

TbBool TbIsCollided( TbRectF32& rect1 , TbRectF32& rect2 );
TbBool TbIsCollided( TbPointS32& point , TbRectF32& rect2 );
TbBool TbIsCollided( TbPointS32& point , TbRectF32& rect2 );

struct TbRangeS32
{
    TbRangeS32(TbSint32 b, TbSint32 e) : begin(b), end(e) {}
    TbRangeS32() : begin(0), end(0) {}
    TbSint32 begin;
    TbSint32 end;
};

struct TbRangeF32
{
    TbRangeF32( TbFloat32 b , TbFloat32 e ) : begin(b) , end(e ) {}
    TbRangeF32() : begin(0.0f) , end(0.0f) {}
    TbFloat32 begin;
    TbFloat32 end;
};

// 円形
struct TbCircleF32
{
    TbCircleF32() : x(0.0f) , y(0.0f) , radius(0.0f) {}
    TbCircleF32( TbFloat32 tmpX , TbFloat32 tmpY , TbFloat32 tmpRadius )
    {
        x = tmpX;
        y = tmpY;
        radius = tmpRadius;
    }
    TbFloat32 x;
    TbFloat32 y;
    TbFloat32 radius;
};

// マージン
struct TbMarginF32Pod
{
    TbFloat32 left;
    TbFloat32 right;
    TbFloat32 top;
    TbFloat32 bottom;
};

struct TbMarginF32
{
    TbMarginF32() : left(0.0f) , right(0.0f) , top(0.0f) , bottom(0.0f) {}
    TbFloat32 left;
    TbFloat32 right;
    TbFloat32 top;
    TbFloat32 bottom;
};

}

#endif
