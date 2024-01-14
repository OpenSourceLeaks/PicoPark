/*!
 * 2D描画
 * @author Miyake Shunsuke
 * @since 2010.12.03
 */

#include "tb_fwd.h"
#include "graphics/render/tb_draw_2d.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/math/tb_gr_matrix_util.h"
#include "graphics/render/tb_render_pass_manager.h"
#include "base/math/tb_matrix.h"


namespace toybox
{

// 参照デバイス
static TbUint32         s_PassIndex = 0;
static TbMatrix         s_WorldMatrix;
static TbVertexBuffer*  s_VertexBufferDraw2dTex  = nullptr;
static TbDepthStencilState* s_DepthStencilsState = nullptr;
static TbUint8*         s_BufferDraw2dTex  = nullptr;
static TbUint32         s_VertexBufferDraw2dTexOffset = 0;
static TbUint32         s_VertexBufferDraw2dTexSize = 0;
static TbUint32         s_VertexBufferDraw2dTexCount = 0;
static TbTexture*       s_Texture = nullptr;
static TbTexture*       s_WhiteTexture = nullptr;
static TbUint32         s_BeginCount = 0;

struct DrawVertexColor
{
    TbFloat32 x , y , z;
    TbColor::ValueType color;
};

void drawVertexBuffer()
{
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(s_PassIndex);
    renderPass->SetVertexBuffer(0, s_VertexBufferDraw2dTex, s_VertexBufferDraw2dTexOffset, sizeof(TbVertexXYZColorTex0));
    renderPass->DrawPrimitive(TB_PT_TRIANGLELIST, 0, s_VertexBufferDraw2dTexCount);
    s_VertexBufferDraw2dTexOffset += s_VertexBufferDraw2dTexSize;
    s_VertexBufferDraw2dTexCount = 0;
    s_VertexBufferDraw2dTexSize = 0;
}

// TbDraw2dTexture用頂点バッファ確保
void TbDraw2dTextureAllocVertexBuffer( TbUint32 byteSize )
{
    if( s_VertexBufferDraw2dTex ) {
        TbDraw2dTextureFreeVertexBuffer();
    }
    byteSize = TB_ALIGN_ROUND_UP(byteSize,sizeof(TbVertexXYZColorTex0));
    s_VertexBufferDraw2dTex = new TbVertexBuffer( &TbVertexFormat::XYZ_COLOR_TEX0SIZE2 , byteSize );
    s_BufferDraw2dTex = new TbUint8[byteSize];

    {
        TbTextureDesc desc;
        desc.height = 32;
        desc.width = 32;
        desc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
        s_WhiteTexture = new TbTexture(desc);
        s_WhiteTexture->WriteWhite(0);
    }
}

// TbDraw2dTexture用頂点バッファ解放
void TbDraw2dTextureFreeVertexBuffer()
{
    TB_SAFE_DELETE(s_VertexBufferDraw2dTex);
    TB_SAFE_DELETE_ARRAY(s_BufferDraw2dTex);
    TB_SAFE_DELETE(s_WhiteTexture);
}

void TbDraw2dTextureResetVertexBuffer()
{
    s_VertexBufferDraw2dTexOffset = 0;
    s_VertexBufferDraw2dTexSize = 0;
    s_VertexBufferDraw2dTexCount = 0;
}

/*!
 * 矩形描画開始処理
 * @author Miyake Shunsuke
 * @since 2011.12.08
 */
TbResult TbDraw2dBegin()
{
    if (s_VertexBufferDraw2dTex) {
        TbDraw2dTextureBegin();
        return TB_S_OK;
    }
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(s_PassIndex);
    if( !renderPass ){
        return TB_E_FAIL;
    }
    // 頂点フォーマットセット
    renderPass->SetShaderProgram(nullptr);
    renderPass->SetFixedShaderVertexFormat( &TbVertexFormat::XYZ_COLOR );
    return TB_S_OK;
}

/*!
 * 矩形描画終了処理
 * @author Miyake Shunsuke
 * @since 2011.12.08
 */
TbResult TbDraw2dEnd()
{
    if (s_VertexBufferDraw2dTex) {
        TbDraw2dTextureEnd();
        return TB_S_OK;
    }
    return TB_S_OK;
}

/*!
 * 矩形テクスチャ描画開始処理
 * @author Miyake Shunsuke
 * @since 2011.12.08
 */
TbResult TbDraw2dTextureBegin()
{
    if (0 != s_VertexBufferDraw2dTexSize || 0 != s_VertexBufferDraw2dTexCount) {
        ++s_BeginCount;
        return TB_S_OK;
    }
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(s_PassIndex);
    if( !renderPass ){
        return TB_E_FAIL;
    }
    // 頂点フォーマットセット
    renderPass->SetShaderProgram(nullptr);
    renderPass->SetFixedShaderVertexFormat( &TbVertexFormat::XYZ_COLOR_TEX0SIZE2 );
    s_Texture = nullptr;
    s_VertexBufferDraw2dTexSize = 0;
    s_VertexBufferDraw2dTexCount = 0;
    return TB_S_OK;
}

/*!
 * 矩形テクスチャ描画終了処理
 * @author Miyake Shunsuke
 * @since 2011.12.08
 */
TbResult TbDraw2dTextureEnd()
{
    if( s_VertexBufferDraw2dTex && 0 < s_VertexBufferDraw2dTexCount )
    {
        if (0 < s_BeginCount) {
            --s_BeginCount;
            return TB_S_OK;
        }
        s_VertexBufferDraw2dTex->Write(s_VertexBufferDraw2dTexOffset+s_VertexBufferDraw2dTexSize,s_BufferDraw2dTex);
        drawVertexBuffer();
    }
    return TB_S_OK;
}

/*!
 * レンダリングパス設定
 * @author Miyake Shunsuke
 * @since 2011.03.20
 */
TbResult TbDraw2dSetRenderPass( TbUint32 index )
{
    if (s_PassIndex != index) {
        if (0 < s_VertexBufferDraw2dTexSize || 0 < s_VertexBufferDraw2dTexCount)
        {
            drawVertexBuffer();
        }
    }
    s_PassIndex = index;
    return TB_S_OK;
}


/*!
 * レンダリングパス取得
 * @author Miyake Shunsuke
 * @since 2013.03.20
 */
TbUint32 TbDraw2dGetRenderPass()
{
    return s_PassIndex;
}

/*!
 * 全体に影響を与える姿勢行列設定
 * @author Miyake Shunsuke
 * @since 2013.06.09
 */
void TbDraw2dSetWorldMatrix( const TbMatrix& matrix )
{
    s_WorldMatrix = matrix;
}

/*!
 * 2D矩形描画
 * @param x X座標
 * @param y Y座標
 * @param w 横幅
 * @param h 縦幅
 * @param matrix 変換行列
 * @param color 色
 * @author Miyake Shunsuke
 * @since 2010.12.03
 */
TbResult TbDrawRect( const TbDrawRectParam& param)
{
    if (s_VertexBufferDraw2dTex) {
        TbDrawRectTextureParam texParam(s_WhiteTexture);
        texParam.pos = param.pos;
        texParam.size = param.size;
        texParam.matrix = param.matrix;
        texParam.color = param.color;           
        TbDrawRectTexture(texParam);
        return TB_E_FAIL;
    }

    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(s_PassIndex);
    if( !renderPass ){
        return TB_E_FAIL;
    }

    // 頂点配列構築
    TbFloat32 x = param.pos.GetX();
    TbFloat32 y = param.pos.GetY();
    TbFloat32 z = param.pos.GetZ();
    TbFloat32 w = param.size.width;
    TbFloat32 h = param.size.height;
    DrawVertexColor v[4]= 
    {
        {  x + w , y , z , param.color.value }, 
        {  x + w , y + h , z , param.color.value },
        {  x ,  y , z , param.color.value },
        {  x ,  y + h , z , param.color.value },
    };

    // 行列設定
    TbMatrix44 pose = s_WorldMatrix * param.matrix;
    renderPass->SetWorldMatrix( pose );

    // 描画
    renderPass->SetTexture( 0 , nullptr );
    renderPass->DrawPrimitive( TB_PT_TRIANGLESTRIP , 2 , v , sizeof(v[0]) );

    return TB_S_OK;
}

/*!
 * 2Dテクスチャ描画
 * @author Miyake Shunsuke
 * @since 2010.12.03
 */
TbResult TbDrawRectTexture( const TbDrawRectTextureParam& param )
{
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(s_PassIndex);
    if( !renderPass ){
        return TB_E_FAIL;
    }

    // 頂点配列構築
    TbFloat32 x = param.pos.GetX();
    TbFloat32 y = param.pos.GetY();
    TbFloat32 z = param.pos.GetZ();
    TbFloat32 w = param.size.width;
    TbFloat32 h = param.size.height;
    TbFloat32 texX = param.rectUV.x;
    TbFloat32 texY = param.rectUV.y;
    TbFloat32 texW = param.rectUV.width;
    TbFloat32 texH = param.rectUV.height;
    TbMatrix44 pose = s_WorldMatrix * param.matrix;

    // 描画
    if( s_Texture != param.texture ){
        if( s_Texture && s_VertexBufferDraw2dTex ) {
            drawVertexBuffer();
        }
        renderPass->SetTexture( 0 , param.texture );
        s_Texture = param.texture;
    }
    TbVector3 pos[] =
    {
        pose * TbVector3(x + w , y , z) ,
        pose * TbVector3(x + w , y + h , z) ,
        pose * TbVector3(x ,  y , z),
        pose * TbVector3(x ,  y + h , z)
    };
    TbVertexXYZColorTex0 v[] =
    {
        { pos[0].GetX() , pos[0].GetY() , pos[0].GetZ() , param.color , texX + texW , texY },
        { pos[1].GetX() , pos[1].GetY() , pos[1].GetZ() , param.color , texX + texW , texY + texH },
        { pos[2].GetX() , pos[2].GetY() , pos[2].GetZ() , param.color , texX , texY },

        { pos[1].GetX() , pos[1].GetY() , pos[1].GetZ() , param.color , texX + texW , texY + texH },
        { pos[2].GetX() , pos[2].GetY() , pos[2].GetZ() , param.color , texX , texY },
        { pos[3].GetX() , pos[3].GetY() , pos[3].GetZ() , param.color , texX , texY + texH },
    };

    if( s_VertexBufferDraw2dTex ) { 
        TbUint32 offset = s_VertexBufferDraw2dTexOffset + s_VertexBufferDraw2dTexSize;
        TbMemCopy(&s_BufferDraw2dTex[offset],v,sizeof(v));
        s_VertexBufferDraw2dTexSize += sizeof(v);
        s_VertexBufferDraw2dTexCount += 2;
    }else{
        renderPass->DrawPrimitive( TB_PT_TRIANGLELIST , 2 , v , sizeof(v[0]) );
    }
    return TB_S_OK;
}

/*!
 * 円形描画
 * @author Miyake_Shunsuke
 * @since 2014.10.13
 */
TbResult TbDrawCircle( const TbDrawCircleParam& param )
{
    if (s_VertexBufferDraw2dTex) {
        // 頂点バッファモードでは未サポート
        return TB_E_FAIL;
    }
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(s_PassIndex);
    if( !renderPass ){
        return TB_E_FAIL;
    }

    // 
    static const TbUint32 DIVIDE_COUNT = 72;
    TbFloat32 RAD = TB_PI_RAD*2 / DIVIDE_COUNT;
    DrawVertexColor v[DIVIDE_COUNT+2];
    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].z = 0.1f;
    v[0].color = param.color.value;
    for( TbUint32 i = 0; i < DIVIDE_COUNT; ++i ) {
        v[i+1].x = TbCos(RAD*static_cast<TbFloat32>(i));
        v[i+1].y = TbSin(RAD*static_cast<TbFloat32>(i));
        v[i+1].z = 0.1f;
        v[i+1].color = param.color.value;
    }
    v[TB_ARRAY_COUNT_OF(v)-1] = v[1];

    // 行列設定
    TbMatrix local = TbMatrix::IDENTITY;
    local.Translate(param.pos);
    local.Scale(param.radius,param.radius,1.0f);
    TbMatrix44 pose = s_WorldMatrix * local * param.matrix;
    renderPass->SetWorldMatrix( pose );

    // 描画
    renderPass->SetTexture( 0 , nullptr );
    renderPass->DrawPrimitive( TB_PT_TRIANGLEFAN , DIVIDE_COUNT , v , sizeof(v[0]) );
    renderPass->SetWorldMatrix(TbMatrix::IDENTITY);

    return TB_S_OK;
}

/*! 
 * 線形描画
 * @author teco
 */
TbResult TbDrawLine( const TbDrawLineParam& param )
{
    if (s_VertexBufferDraw2dTex) {
#if !TB_PLATFORM_IS_WIN
        return TB_E_FAIL;
#endif
        drawVertexBuffer();
    }
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(s_PassIndex);
    if( !renderPass ){
        return TB_E_FAIL;
    }
//    renderPass->SetDepthStencilState(TbRenderStatePreset::DEPTH_STENCIL_2D);

    static const TbUint32 STACK_VERTEX_MAX = 32;
    DrawVertexColor v[STACK_VERTEX_MAX];
    if( param.vertexCount < TB_ARRAY_COUNT_OF(v) ){

        for( TbUint32 i = 0; i < param.vertexCount; ++i ) {
            v[i+1].x = param.vertex[i].GetX();
            v[i+1].y = param.vertex[i].GetY();
            v[i+1].z = 0.1f;
            v[i+1].color = param.color.value;
        }

        // 行列設定
        TbMatrix local = TbMatrix::IDENTITY;
        local.Translate(param.pos.GetX(), param.pos.GetY(), 0.0f);
        TbMatrix44 pose = s_WorldMatrix * local * param.matrix;
        renderPass->SetWorldMatrix( pose );

        // 描画
        renderPass->SetTexture( 0 , nullptr );
        s_Texture = nullptr;
        renderPass->DrawPrimitive( TB_PT_LINESTRIP , param.vertexCount , v , sizeof(v[0]) );
        renderPass->SetWorldMatrix(TbMatrix::IDENTITY);
    }else{
        // まだサポートしていない
        TB_ASSERT(0);
        return TB_E_FAIL;
    }
//    renderPass->SetDepthStencilState(nullptr);
    return TB_S_OK;
}

}
