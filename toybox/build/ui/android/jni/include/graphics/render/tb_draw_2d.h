/*!
 * 2D描画
 * @author Miyake Shunsuke
 * @since 2010.10.30
 */

#ifndef _INCLUDED_TB_DRAW_2D_H_
#define _INCLUDED_TB_DRAW_2D_H_

#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_matrix.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

class TbTexture;

// 描画パス設定
TbResult TbDraw2dSetRenderPass( TbUint32 passIndex );

// 描画パス取得
TbUint32 TbDraw2dGetRenderPass();

// 全体に影響を与える姿勢行列設定
void TbDraw2dSetWorldMatrix( const TbMatrix& matrix );

// TbDraw2dTexture用頂点バッファ確保
void TbDraw2dTextureAllocVertexBuffer( TbUint32 byteSize );

// TbDraw2dTexture用頂点バッファ解放
void TbDraw2dTextureFreeVertexBuffer();

void TbDraw2dTextureResetVertexBuffer();

// 2D描画開始処理
TbResult TbDraw2dBegin();

// 2D描画終了処理
TbResult TbDraw2dEnd();

// 2Dテクスチャ描画開始処理
TbResult TbDraw2dTextureBegin(TbUint32 passIndex );

// 2Dテクスチャ描画開始処理
TbResult TbDraw2dTextureBegin();

// 2Dテクスチャ描画終了処理
TbResult TbDraw2dTextureEnd();

/////////////////////
//     描画        //
/////////////////////

struct TbDrawRectParam
{
    TbDrawRectParam()
        : pos(0.0f,0.0f,0.1f)
        , size()
        , matrix(TbMatrix::IDENTITY)
        , color(TbColor::White()) 
    {}

    // 矩形設定
    void SetRect( TbFloat32 x , TbFloat32 y , TbFloat32 w , TbFloat32 h ){
        pos.SetXY( x , y );
        size.width = w;
        size.height = h;
    }

    void SetRect( const TbRectF32& rect ){
        pos.SetXY( rect.x , rect.y );
        size.width = rect.width;
        size.height = rect.height;
    }

    // 奥行き設定
    void SetZ( TbFloat32 z ) {
        pos.SetZ(z);
    }

    // カラー設定
    void SetColor( const TbColor& argColor ) {
        color = argColor;
    }

    // 回転
    void Rotate( const TbAngle32 rotate )
    {
        matrix.RotateZ( rotate );
    }

    TbVector3       pos;
    TbDimensionF32  size;
    TbMatrix        matrix;
    TbColor         color;
};

struct TbDrawRectTextureParam : public TbDrawRectParam
{
    TbDrawRectTextureParam( TbTexture* texture )
        : TbDrawRectParam()
        , texture(texture)
        , rectUV()
    {
        rectUV.width = 1.0f;
        rectUV.height = 1.0f;
    }

    void SetRectUV( TbFloat32 x , TbFloat32 y , TbFloat32 w , TbFloat32 h ){
        rectUV.x = x;
        rectUV.y = y;
        rectUV.width = w;
        rectUV.height = h;
    }
    void SetRectUV( const TbRectF32& rect ){
        rectUV = rect;
    }

    TbTexture*      texture;
    TbRectF32       rectUV;
};

struct TbDrawCircleParam
{
    TbDrawCircleParam()
        : pos(0.0f,0.0f,0.1f)
        , radius(1.0f)
        , matrix(TbMatrix::IDENTITY)
        , color(TbColor::White()) 
    {}

    TbVector3       pos;
    TbFloat32       radius;
    TbMatrix        matrix;
    TbColor         color;
};

struct TbDrawLineParam
{
    TbDrawLineParam()
        : pos(0.0f,0.0f)
        , matrix(TbMatrix::IDENTITY)
        , vertex(nullptr)
        , vertexCount(0)
        , color(TbColor::White()) 
    {}

    TbVector2       pos;
    TbFloat32       radius;
    TbMatrix        matrix;
    TbVector2*      vertex;
    TbUint32        vertexCount;
    TbColor         color;
};

struct TbDrawLineRectParam : public TbDrawLineParam
{
    // 矩形設定
    void SetRect( TbFloat32 x , TbFloat32 y , TbFloat32 w , TbFloat32 h )
    {
        rect[0].Set( x , y );
        rect[1].Set( x , y + h );
        rect[2].Set( x + w , y + h );
        rect[3].Set( x + w , y );
        rect[4].Set( x , y );
        vertex = rect;
        vertexCount = TB_ARRAY_COUNT_OF(rect);
    };
    void SetRect( const TbRectF32& rect )
    {
        SetRect( rect.x , rect.y , rect.width , rect.height );
    }
    TbVector2 rect[5];
};

// 2D矩形描画
extern TbResult TbDrawRect(const TbDrawRectParam& param);

// 2Dテクスチャ矩形描画
extern TbResult TbDrawRectTexture( const TbDrawRectTextureParam& param );

// 円形描画
extern TbResult TbDrawCircle( const TbDrawCircleParam& param );

// 線形描画
extern TbResult TbDrawLine( const TbDrawLineParam& param );

}

#endif
