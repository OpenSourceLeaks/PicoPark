/*!
 * @file
 * @brief DirectX9の固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_fixed_shader.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "graphics/render/3dapi/tb_shader_program.h"
#include "graphics/render/3dapi/tb_fixed_light.h"
#include "graphics/render/3dapi/tb_fixed_material.h"

namespace toybox
{

////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShaderDepend::TbFixedShaderDepend()
    : m_BitArray()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShaderDepend::~TbFixedShaderDepend()
{
}

////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShader::TbFixedShader()
    : m_VertexFormat( nullptr )
    , m_Depend()
{
}

/*!
 * デストラクタ
 * 
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShader::~TbFixedShader()
{
}

/*!
 * 頂点フォーマット設定
 * @author Miyake Shunsuke
 * @since 2010.05.10
 */
void TbFixedShader::SetVertexFormat( const TbVertexFormat* format )
{
    m_VertexFormat = format;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    TbShaderProgram* program = shaderManager.GetCurrentProgram();
    if( !program || !program->IsActiveShader(TB_SHADER_TYPE_VERTEX) ){
        device.GetDepend().SetCurrentVertexFormat( m_VertexFormat );
    }
}

/*!
 * 行列設定
 * @author Miyake Shunsuke
 * @since 2011.03.17
 */
TbResult TbFixedShader::SetMatrix( TbFixedShaderMatrixType matrixType , const TbMatrix44& matrix )
{
    D3DTRANSFORMSTATETYPE TS_TYPE[ TB_FIXED_SHADER_MATRIX_MAX ] =
    {
        D3DTS_WORLD , 
        D3DTS_VIEW , 
        D3DTS_PROJECTION , 
        D3DTS_TEXTURE0 , 
    };
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if( 0 <= matrixType && matrixType < TB_FIXED_SHADER_MATRIX_MAX && device ){
        device->SetTextureStageState( 0 ,D3DTSS_TEXTURETRANSFORMFLAGS , D3DTTFF_COUNT2 ); // 後で修正せよ
        device->SetTransform( TS_TYPE[ matrixType ] , reinterpret_cast<const D3DMATRIX*>( &matrix ) );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 固定機能シェーダに渡す行列取得
 * @author Miyake Shunsuke
 * @since 2011.05.15
 */
TbResult TbFixedShader::GetMatrix( TbFixedShaderMatrixType matrixType , TB_OUT TbMatrix44& matrix )
{
    // 行列タイプ
    static const D3DTRANSFORMSTATETYPE TS_TYPE[ TB_FIXED_SHADER_MATRIX_MAX ] =
    {
        D3DTS_WORLD , 
        D3DTS_VIEW , 
        D3DTS_PROJECTION , 
        D3DTS_TEXTURE0 , 
    };
    if( 0 <= matrixType && matrixType < TB_FIXED_SHADER_MATRIX_MAX ) // 行列タイプは正しいか
    {
        TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9()->GetTransform( TS_TYPE[ matrixType ] , reinterpret_cast<D3DMATRIX*>(&matrix) );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * シェーディングモード設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetShadeMode( TbFixedShaderShadeMode shadeMode )
{
    static const D3DSHADEMODE CONVERT_TABLE[] = {
        D3DSHADE_FLAT , 
        D3DSHADE_GOURAUD , 
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_FIXED_SHADER_SHADE_MODE_MAX );
    if( 0 <= shadeMode && shadeMode < TB_FIXED_SHADER_SHADE_MODE_MAX ){
        TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9()->SetRenderState( D3DRS_SHADEMODE , CONVERT_TABLE[shadeMode] );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 * @since 2011.03.23
 */
TbResult TbFixedShader::SetEnableLighting( TbBool isEnable )
{
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if( device ){
        device->SetRenderState( D3DRS_LIGHTING , isEnable );
    }
    return TB_S_OK;
}


/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetEnableLight( TbUint32 index , TbBool isEnable )
{
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    HRESULT hr = deviceDepend.GetDeviceDX9()->LightEnable( index , isEnable );
    if( S_OK == hr ){
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ライト設定
#include <graphics/tb_render_device.h>
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetLight( TbUint32 index , const TbFixedLight* light )
{
    if( light )
    {
        TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
        HRESULT hr = deviceDepend.GetDeviceDX9()->SetLight( index , &(light->GetDepend().lightDX9) );
        if( S_OK == hr ){
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 *マテリアル設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetMaterial( const TbFixedMaterial* material )
{
    if( material )
    {
        TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
        HRESULT hr = deviceDepend.GetDeviceDX9()->SetMaterial( &(material->GetDepend().materialDX9) );
        if( S_OK == hr ){
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

static const DWORD CONVERT_TABLE_TEX_ARG[] = {
    D3DTA_TEXTURE ,
    D3DTA_DIFFUSE , 
};

static const D3DTEXTUREOP CONVERT_TABLE_TEX_OP[] = {
    D3DTOP_SELECTARG1 , 
    D3DTOP_MODULATE , 
    D3DTOP_ADD ,
    D3DTOP_SUBTRACT
};

/*!
 * テクスチャ合成方法設定( 入力RGB )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    static const D3DTEXTURESTAGESTATETYPE CONVERT_TABLE_ARG_INDEX[] = {
        D3DTSS_COLORARG1 , 
        D3DTSS_COLORARG2 , 
        D3DTSS_COLORARG0 , 
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) == TB_TEXTURE_ARG_MAX );
    if( TB_ARRAY_COUNT_OF(CONVERT_TABLE_ARG_INDEX) <= argIndex || 
        TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) <= arg ){
        return TB_E_INVALIDARG;
    }
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    LPDIRECT3DDEVICE9 deviceDx9 = deviceDepend.GetDeviceDX9();
    HRESULT hr = deviceDx9->SetTextureStageState( stage , CONVERT_TABLE_ARG_INDEX[argIndex] , CONVERT_TABLE_TEX_ARG[arg] );
    if( !TB_VERIFY( SUCCEEDED(hr) ) ){
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( RGB演算方式 )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureColorOp( TbUint32 stage , TbTextureOp op )
{
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_OP) == TB_TEXTURE_OP_MAX );
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    LPDIRECT3DDEVICE9 deviceDx9 = deviceDepend.GetDeviceDX9();
    HRESULT hr = deviceDx9->SetTextureStageState( stage , D3DTSS_COLOROP , CONVERT_TABLE_TEX_OP[op] );
    if( !TB_VERIFY( SUCCEEDED(hr) ) ){
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( 入力アルファ )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    static const D3DTEXTURESTAGESTATETYPE CONVERT_TABLE_ARG_INDEX[] = {
        D3DTSS_ALPHAARG1 , 
        D3DTSS_ALPHAARG2 , 
        D3DTSS_ALPHAARG0 , 
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) == TB_TEXTURE_ARG_MAX );
    if( TB_ARRAY_COUNT_OF(CONVERT_TABLE_ARG_INDEX) <= argIndex || 
        TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) <= arg ){
        return TB_E_INVALIDARG;
    }
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    LPDIRECT3DDEVICE9 deviceDx9 = deviceDepend.GetDeviceDX9();
    HRESULT hr = deviceDx9->SetTextureStageState( stage , CONVERT_TABLE_ARG_INDEX[argIndex] , CONVERT_TABLE_TEX_ARG[arg] );
    if( !TB_VERIFY( SUCCEEDED(hr) ) ){
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( アルファ演算方式 )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureAlphaOp( TbUint32 stage , TbTextureOp op )
{
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_OP) == TB_TEXTURE_OP_MAX );
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    LPDIRECT3DDEVICE9 deviceDx9 = deviceDepend.GetDeviceDX9();
    HRESULT hr = deviceDx9->SetTextureStageState( stage , D3DTSS_ALPHAOP , CONVERT_TABLE_TEX_OP[op] );
    if( !TB_VERIFY( SUCCEEDED(hr) ) ){
        return TB_E_FAIL;
    }
    return TB_S_OK;

}


}

#endif