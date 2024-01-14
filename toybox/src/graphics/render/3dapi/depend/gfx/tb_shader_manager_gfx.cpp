/*!
 * シェーダー管理クラス( GFX )
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "base/util/tb_depend_cast.h"

#if TB_SHADER_API_IS_GFX

#include "graphics/render/3dapi/tb_shader_program.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"
#include "graphics/render/3dapi/tb_vertex_format.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/3dapi/tb_constant_buffer.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"
#include "base/math/tb_matrix.h"
#include "base/system/tb_assert.h"
#include "base/io/tb_print.h"
#include "base/io/tb_file.h"
#include "base/io/tb_file_manager.h"
#include "base/string/tb_static_string.h"

namespace toybox
{

// 不正値
TbShaderConstHandle TB_SHADER_CONST_HANDLE_INVALID;

namespace
{
    const TbUint32 LOG_BUFFER_SIZE = 512;
    TbChar8 s_LogBuffer[ LOG_BUFFER_SIZE ];

    // TbVertexElement::Typeとの対応テーブル
    static const nn::gfx::AttributeFormat VERTEX_ELEMENT_TYPE[] =
    {
        nn::gfx::AttributeFormat_32_Float ,             // TYPE_FLOAT1
        nn::gfx::AttributeFormat_32_32_Float ,          // TYPE_FLOAT2
        nn::gfx::AttributeFormat_32_32_32_Float ,       // TYPE_FLOAT3
        nn::gfx::AttributeFormat_32_32_32_32_Float,     // TYPE_FLOAT4
        nn::gfx::AttributeFormat_32_32_32_32_Float ,    // TYPE_COLOR 
        nn::gfx::AttributeFormat_8_8_8_8_Uint ,         // TYPE_UBYTE4
        nn::gfx::AttributeFormat_16_16_Sint ,           // TYPE_SHORT2
        nn::gfx::AttributeFormat_16_16_16_16_Sint ,     // TYPE_SHORT4

        nn::gfx::AttributeFormat_8_8_8_8_Unorm ,        // TYPE_UBYTE4N
        nn::gfx::AttributeFormat_16_16_Snorm   ,        // TYPE_SHORT2N
        nn::gfx::AttributeFormat_16_16_16_16_Snorm ,    // TYPE_SHORT4N
        nn::gfx::AttributeFormat_16_16_Unorm,           // TYPE_USHORT2N
        nn::gfx::AttributeFormat_16_16_16_16_Unorm ,    // TYPE_USHORT4N
        nn::gfx::AttributeFormat_10_10_10_2_Uint ,      // TYPE_UDEC3
        nn::gfx::AttributeFormat_10_10_10_2_Unorm ,     // TYPE_UDEC3N
        nn::gfx::AttributeFormat_16_16_Float ,          // TYPE_FLOAT16_2
        nn::gfx::AttributeFormat_16_16_16_16_Float ,    // TYPE_FLOAT16_4
        nn::gfx::AttributeFormat_Undefined ,            // TYPE_UNSUSED
    };

    static const TbUint32 VERTEX_ELEMENT_SIZE_TABLE[] =
    {
        4 ,             // TYPE_FLOAT1
        8 ,          // TYPE_FLOAT2
        12 ,       // TYPE_FLOAT3
        16 ,     // TYPE_FLOAT4
        16 ,    // TYPE_COLOR 
        4 ,         // TYPE_UBYTE4
        4 ,           // TYPE_SHORT2
        8 ,     // TYPE_SHORT4
        4 ,        // TYPE_UBYTE4N
        4   ,        // TYPE_SHORT2N
        8 ,    // TYPE_SHORT4N
        4 ,           // TYPE_USHORT2N
        8 ,    // TYPE_USHORT4N
        4 ,      // TYPE_UDEC3
        4 ,     // TYPE_UDEC3N
        4 ,          // TYPE_FLOAT16_2
        8 ,    // TYPE_FLOAT16_4
        0 ,            // TYPE_UNSUSED};
    };

    TbStaticString32 s_UsageVarName[ TbVertexElement::USAGE_MAX - 1 ] =
    {
        "a_Position",
        "a_Blendweight",
        "a_BlendIndices",
        "a_Normal",
        "a_Psize",
        "a_Texcoord",
        "a_Tangent",
        "a_Binormal",
        "a_Tessfactor",
        "a_PositionT",
        "a_Color",
        "a_Fog",
        "a_Depth",
        "a_Sample",
    };

    const TbSint32 USAGE_VAR_USER_MAX = 5;

    TbStaticString32 s_UsageVarNameUser[ USAGE_VAR_USER_MAX ] =
    {
        "a_UserData0",
        "a_UserData1",
        "a_UserData2",
        "a_UserData3",
        "a_UserData4",
    };

    // システム予約用uniform変数名
    enum {
        UNIFORM_VAR_SYSTEM_MODELVIEWPROJECT_MATRIX ,
        UNIFORM_VAR_SYSTEM_MAX 
    };

    const TbUint32 CONSTANT_MAX = 10;
    TbStaticString32 s_ConstantName[ CONSTANT_MAX ] = 
    {
        "u_ModelViewProjMatrix" ,
    };

    TbStaticString32 s_TextureVarName = "u_Texture";
    
    // デフォルトのシェーダ最大数
    const TbUint32 SHADER_DEFAULT_MAX = 20;

    // 定数ハンドルを更新
    TbBool updateConstHandle( TbUint32 shaderBit , TbShaderProgram* program , TbShaderConstHandleDepend& handle ) {
        if( program ){
            return program->GetDepend().UpdateConstantHandle(shaderBit,handle);
        }
        return TB_FALSE;
    }
}

/*!
 * 設定
 * @author teco
 */
void TbShaderConstHandle::Set( TbUint32 index )
{
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    const TbChar8* srcName = shaderManager.GetDepend().GetConstantName( index );
    TbStrCopy( m_Depend.name , TB_ARRAY_COUNT_OF(m_Depend.name) , srcName );
    TbFill(m_Depend.handle, TbShaderConstHandleDepend::INVALID_HANDLE);
}

// 依存コード

/*!
 * コンストラクタ
 * @author teco
 */
TbShaderManagerDepend::TbShaderManagerDepend()
    : m_BitArray()
    , m_VertexShaderPool( SHADER_DEFAULT_MAX )
    , m_PixelShaderPool( SHADER_DEFAULT_MAX  )
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbShaderManagerDepend::~TbShaderManagerDepend()
{
    nn::gfx::Device* device = TbGetGfxDevice();
    // シェーダ、プログラム削除
    {
        VertexShaderPool::Iterator ite = m_VertexShaderPool.Begin();
        VertexShaderPool::Iterator endIte = m_VertexShaderPool.End();
        for( ; ite != endIte; ++ite ){
            VertexShader shader = *ite;
            if (shader.vertexState) {
                shader.vertexState->Finalize(device);
                TB_SAFE_DELETE_ARRAY(shader.vertexStateMemory);
            }
            shader.shader->Finalize(device);
            TB_SAFE_DELETE(shader.shader);
        }
    }
    //
    {
        ShaderPool::Iterator ite = m_PixelShaderPool.Begin();
        ShaderPool::Iterator endIte = m_PixelShaderPool.End();
        for( ; ite != endIte; ++ite ){
            nn::gfx::Shader* shader = *ite;
            shader->Finalize(device);
            TB_SAFE_DELETE(shader);
        }
    }
}

/*!
 * 定数名設定
 * @author teco
 */
const TbChar8* TbShaderManagerDepend::GetConstantName( const TbUint32 index )
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    if( index < TB_ARRAY_COUNT_OF(s_ConstantName) ){
        return s_ConstantName[index].GetBuffer();
    }
    return nullptr;
}

/*!
 * 定数名設定
 * @author teco
 */
TbResult TbShaderManagerDepend::SetConstantName( const TbUint32 index , const TbChar8* name )
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    if( index >= common.GetConstantCount() ){
        return TB_E_FAIL;
    }
    s_ConstantName[ index ] = name;
    return TB_S_OK;
}

/*!
 * 頂点シェーダ追加
 * @author teco
 */
TB_INLINE TbShaderId TbShaderManagerDepend::addVertexShader( nn::gfx::Shader* shader)
{
    if( shader && !m_VertexShaderPool.IsFull() ){
        VertexShader vs;
        vs.shader = shader;
        return m_VertexShaderPool.Add(vs);
    }
    return TB_SHADER_ID_INVALID;
}

/*!
 * ピクセルシェーダ追加
 * @author teco
 */
TB_INLINE TbShaderId TbShaderManagerDepend::addPixelShader(nn::gfx::Shader* shader)
{
    if( shader && !m_PixelShaderPool.IsFull() ){
        return m_PixelShaderPool.Add(shader);
    }
    return TB_SHADER_ID_INVALID;
}

extern nn::gfx::TextureView g_TextureView;

/*! 
 * テクスチャサンプラ変数情報更新
 * @author teco
 */
void TbShaderManagerDepend::updateSamplerVar()
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return;
    }
    if(!device.GetDepend().IsRecordingCommand()){
        return;
    }
    // デバイスに設定されているテクスチャがあれば、シェーダ側でも利用していると仮定する
    // 利用している場合は、変数にバインドする
    if( device.GetActiveTextureCount() > 0 )
    {
        TbSint32 texCount = device.GetActiveTextureCount();
        TbSint32 setCount = 0;
        nn::gfx::CommandBuffer* commandBuffer = TbGetGfxCommandBuffer();

        nn::gfx::Shader* shader = GetShader(TB_SHADER_TYPE_PIXEL, program->GetCurrentShader(TB_SHADER_TYPE_PIXEL));
        if (shader)
        {
            for (TbSint32 i = 0; i < TbRenderDevice::TEXTURE_STAGE_MAX && setCount < texCount; ++i) {
                if (toybox::TbTexture* texture = device.GetTexture(i, TB_SHADER_TYPE_PIXEL)) {
                    toybox::TbTextureSampler* sampler = device.GetTextureSampler(i,TB_SHADER_TYPE_PIXEL);
                    if (!texture || !sampler) {
                        continue;
                    }
                    TbStaticString128 varName = TbStaticString128::Format("%s%d", common.GetTextureVarName(), i);
                    const nn::gfx::DescriptorSlot* texDescSlot = texture->GetDepend().GetDescriptorSlot();
                    TbSint32 slot = shader->GetInterfaceSlot(nn::gfx::ShaderStage_Pixel,nn::gfx::ShaderInterfaceType_Sampler,varName.GetBuffer());
                    if (0 <= slot) {
                        commandBuffer->SetDescriptorPool(texture->GetDepend().GetDescriptorPool());
                        commandBuffer->SetDescriptorPool(sampler->GetDepend().GetDescriptorPool());
                        commandBuffer->SetTextureAndSampler( slot, nn::gfx::ShaderStage_Pixel, *texDescSlot, *sampler->GetDepend().GetDescriptorSlot() );
                    }
                    ++setCount;
                }
            }
        }
    }
}

/*!
 * テクスチャサンプラ変数情報更新
 * @author teco
 */
void TbShaderManagerDepend::updateSamplerVar( TbUint32 slot )
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return;
    }
    nn::gfx::Shader* shader = GetShader(TB_SHADER_TYPE_PIXEL, program->GetCurrentShader(TB_SHADER_TYPE_PIXEL));
    nn::gfx::CommandBuffer* commandBuffer = TbGetGfxCommandBuffer();
    if (shader && device.GetDepend().IsRecordingCommand() )
    {
        if (toybox::TbTexture* texture = device.GetTexture(slot, TB_SHADER_TYPE_PIXEL)) {
            toybox::TbTextureSampler* sampler = device.GetTextureSampler(slot, TB_SHADER_TYPE_PIXEL);
            const nn::gfx::DescriptorSlot* texDescSlot = texture->GetDepend().GetDescriptorSlot();
            TbStaticString128 varName = TbStaticString128::Format("%s%d", common.GetTextureVarName(), slot);
            TbSint32 slot = shader->GetInterfaceSlot(nn::gfx::ShaderStage_Pixel, nn::gfx::ShaderInterfaceType_Sampler, varName.GetBuffer());
            if (0 <= slot) {
                commandBuffer->SetDescriptorPool(texture->GetDepend().GetDescriptorPool());
                commandBuffer->SetDescriptorPool(sampler->GetDepend().GetDescriptorPool());
                commandBuffer->SetTextureAndSampler(slot, nn::gfx::ShaderStage_Pixel, *texDescSlot , *sampler->GetDepend().GetDescriptorSlot());
            }
        }
    }

}


/*!
 * シェーダー生成
 * @author teco
 */
nn::gfx::Shader* TbShaderManagerDepend::createShader(nn::gfx::ShaderStage stage , const TbChar8* buffer , TbUint32 bufferSize )
{
    nn::gfx::Device* device = TbGetGfxDevice();
    nn::gfx::Shader::InfoType info;
    info.SetDefault();
    info.SetSeparationEnabled(true);

    nn::gfx::ShaderCode code;
    code.codeSize = bufferSize;
    code.pCode = buffer;
    info.SetShaderCodePtr(stage, &code);
    info.SetSourceFormat(nn::gfx::ShaderSourceFormat_Glsl);
    info.SetCodeType(nn::gfx::ShaderCodeType_Source);

    nn::gfx::Shader* shader = new nn::gfx::Shader();
    nn::gfx::ShaderInitializeResult result = shader->Initialize(device,info);
    if (!TB_VERIFY(result== nn::gfx::ShaderInitializeResult_Success)) {
        TB_SAFE_DELETE(shader);
    }
    return shader;
}

/*!
 * コンストラクタ
 * @author teco
 */
TbShaderManager::TbShaderManager()
    : m_BitArray()
    , m_Depend()
    , m_ShaderProgram(nullptr)
{
    TbFill(m_ConstHandle,TB_SHADER_CONST_HANDLE_INVALID);
    m_ConstHandle[CONST_HANDLE_WVP].Set(TB_SHADER_CONST_INDEX_WVP);
}

/*!
 * デストラクタ
 * @author teco
 */
TbShaderManager::~TbShaderManager()
{
}

/*!
* 初期化
* @author teco
*/
void TbShaderManager::Initialize()
{
    m_Depend.m_ConstantBufferWVP.Initialize(sizeof(TbMatrix));
}

/*!
* 後始末
* @author teco
*/
void TbShaderManager::Finalize()
{
    m_Depend.m_ConstantBufferWVP.Finalize();
}

/*!
 * シェーダプログラム生成(メモリから)
 * @param bufferType 引き渡すバッファの種類
 * @param buffer シェーダバッファ
 * @param entry GLSLでは無視されます
 * @param args GLSLでは無視されます
 * @author teco
 */
TbShaderId TbShaderManager::CreateShader( TbShaderType type ,
                                          TbShaderBufferType bufferType , 
                                          const TbChar8* buffer ,
                                          const TbUint32 bufferSize ,
                                          const TbChar8* entry ,
                                          const TbChar8** args )
{
    TbShaderId result = TB_SHADER_ID_INVALID;
    if( !TB_VERIFY(buffer) ){
        return result;
    }
    switch( type )
    {
    case TB_SHADER_TYPE_VERTEX:
        if( !m_Depend.isFullVertexShaderPool() ){
            nn::gfx::Shader* shader = m_Depend.createShader( nn::gfx::ShaderStage_Vertex , buffer , bufferSize );
            if(shader){
                result = m_Depend.addVertexShader( shader );
            }
        }
        break;
    case TB_SHADER_TYPE_PIXEL:
        if( !m_Depend.isFullPixelShaderPool() ){
            nn::gfx::Shader* shader = m_Depend.createShader(nn::gfx::ShaderStage_Pixel, buffer , bufferSize );
            if( shader ){
                result = m_Depend.addPixelShader( shader );
            }
        }
        break;
    default:
        {
            TB_ASSERT(0);
        }
        break;
    }
    return result;
}

/*!
 * 頂点シェーダプログラム生成(ファイルから)(完了復帰)
 * @param bufferType 引き渡すバッファの種類
 * @param buffer シェーダバッファ
 * @param entry GLSLでは無視されます
 * @param args GLSLでは無視されます
 * @author teco
 */
TbShaderId TbShaderManager::CreateShaderFromFile( TbShaderType type ,
                                                  TbShaderBufferType bufferType , 
                                                  const TbChar8* filePath ,
                                                  const TbChar8* entry ,
                                                  const TbChar8** args )
{
    TbShaderId result = TB_SHADER_ID_INVALID;
    TbFile file( filePath , TB_FILE_OPERATE_READ , TB_FILE_SYNC , TB_FILE_PATH_ID_PRESET_SHADER );
    file.Open();
    file.Read();
    result = CreateShader( type , bufferType , reinterpret_cast<const TbChar8*>(file.GetReadBuffer()) , file.GetReadBufferSize() ,  entry , args );
    return result;
}

/*!
 * プログラム作成
 * @author teco
 */
TbShaderProgram* TbShaderManager::CreateProgram()
{
    return TB_NEW TbShaderProgram();
}

/*!
 * プログラム削除
 * @author teco
 */
void TbShaderManager::DeleteProgram( TbShaderProgram* program )
{
    TB_DELETE program;
}

/*!
 * シェーダプログラム設定
 * @author teco
 */
TbResult TbShaderManager::SetProgram( TbShaderProgram* program )
{
    if( m_ShaderProgram == program && TbRenderDevice::GetInstance().GetDepend().IsEnableCommandCache() ) {
        return TB_S_OK;
    }
    if (!program) {
        // 固定機能シェーダ設定
        TbFixedShader& fixedShader = TbRenderDevice::GetInstance().GetFixedShader();
        fixedShader.GetDepend().Activate();
        return TB_S_OK;
    }
    m_ShaderProgram = program;
    if( program ){
        program->Update();

        // シェーダ設定
        if (TbRenderDevice::GetInstance().GetDepend().IsRecordingCommand())
        {
            nn::gfx::CommandBuffer* commandBuffer = TbGetGfxCommandBuffer();
            TbShaderId vertexShaderId = program->GetCurrentShader(TB_SHADER_TYPE_VERTEX);
            TbShaderId pixelShaderId = program->GetCurrentShader(TB_SHADER_TYPE_PIXEL);
            TbShaderManagerDepend::VertexShader* vertexShader = m_Depend.m_VertexShaderPool.GetAt(vertexShaderId);
            nn::gfx::Shader** pixelShader = m_Depend.m_PixelShaderPool.GetAt(pixelShaderId);
            commandBuffer->SetShader(vertexShader->shader,nn::gfx::ShaderStageBit_Vertex);
            commandBuffer->SetShader(*pixelShader,nn::gfx::ShaderStageBit_Pixel);
            // 頂点フォーマット設定
            commandBuffer->SetVertexState(vertexShader->vertexState);

            m_Depend.updateSamplerVar();
        }

        // GLSLの場合はシェーダ変わるとuniform変数の場所が変わるのでリセットしておく
        for( TbUint32 i =0 ; i < TB_ARRAY_COUNT_OF(m_ConstHandle); ++i ) {
            m_ConstHandle[CONST_HANDLE_WVP].m_Depend.ResetHandle();
        }

    }
    return TB_S_OK;
}

/*!
 * 頂点フォーマットを頂点シェーダにバインド
 * @author teco
 */
TbBool TbShaderManager::BindVertexFormat( const TbVertexFormat* format , const TbShaderId vertexShaderId )
{
    TbShaderManagerDepend::VertexShader* shader = m_Depend.m_VertexShaderPool.GetAt(vertexShaderId);
    if (shader && !shader->vertexState ) {
        nn::gfx::Device* device = TbGetGfxDevice();
        shader->format = format;
        TbSint32 elementCount = format->GetDepend().GetElementCount();
        TbStaticString128 usageName[32];
        TB_RESQUE_RET(elementCount <= TB_ARRAY_COUNT_OF(usageName),TB_FALSE);

        nn::gfx::VertexAttributeStateInfo* attrib = new nn::gfx::VertexAttributeStateInfo[elementCount];
        ptrdiff_t stride = 0;
        for (TbSint32 i = 0; i < elementCount; ++i) {
            const TbVertexElement* element = format->GetDepend().GetElement(i);
            attrib[i].SetDefault();
            attrib[i].SetBufferIndex(0);
            attrib[i].SetFormat(VERTEX_ELEMENT_TYPE[element->type]);
            attrib[i].SetOffset(element->offset);
            const TbChar8* name = GetElementUsageVarName(static_cast<TbVertexElement::Usage>(element->usage));
            usageName[i] = TbStaticString128::Format("%s%d", name, element->usageIndex);

            attrib[i].SetNamePtr(usageName[i].GetBuffer());
            stride += VERTEX_ELEMENT_SIZE_TABLE[element->type];
        }

        nn::gfx::VertexBufferStateInfo buffer;
        buffer.SetDefault();
        buffer.SetStride(stride);

        nn::gfx::VertexState::InfoType vertexInfo;
        vertexInfo.SetVertexAttributeStateInfoArray(attrib, elementCount);
        vertexInfo.SetVertexBufferStateInfoArray(&buffer, 1);
        shader->vertexState = new nn::gfx::VertexState();

        size_t size = nn::gfx::VertexState::GetRequiredMemorySize(vertexInfo);
        if (0 < size) {
            size_t align = nn::gfx::VertexState::RequiredMemoryInfo_Alignment;
            shader->vertexStateMemory = new TbUint8[size+align];
            void* alignedMemory = nn::util::BytePtr(shader->vertexStateMemory).AlignUp(align).Get();
            shader->vertexState->SetMemory(alignedMemory, size);
        }
        shader->vertexState->Initialize(device, vertexInfo, shader->shader);
        TB_SAFE_DELETE_ARRAY(attrib);
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * デバイスのBeginSceneと同時に呼ばれる関数
 * @author teco
 */
void TbShaderManager::OnBeginScene()
{
    m_Depend.m_ConstantBufferWVP.Reset();
}

/*!
 * デバイスのEndSceneと同時に呼ばれる関数
 * @author teco
 */
void TbShaderManager::OnEndScene()
{
}

/*!
 * 利用可能な定数数取得
 * @author teco
 */
TbUint32 TbShaderManager::GetConstantCount() const
{
    return CONSTANT_MAX;
}

/*! 
 * bool型頂点シェーダ定数設定( ハンドルから )
 * @author teco
 */
void TbShaderManager::SetConstantBool( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbBool* value ,TbUint32 count )
{
    TB_ASSERT(!"need Constant Buffer");
}

/*! 
 * float32型頂点シェーダ定数設定( ハンドルから )
 * @author teco
 */
void TbShaderManager::SetConstantF32( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbFloat32* value , TbUint32 count )
{
    TB_ASSERT(!"need Constant Buffer");
}

/*! 
 * sint32型頂点シェーダ定数設定( ハンドルから )
 * @author teco
 */
void TbShaderManager::SetConstantS32( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbSint32* value , TbUint32 count )
{
    TB_ASSERT(!"need Constant Buffer");
}

/*! 
 * TbMatrix型頂点シェーダ定数設定( ハンドルから )
 * @author teco
 */
void TbShaderManager::SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbMatrix* value , TbUint32 count )
{
    TB_ASSERT(!"need Constant Buffer");
}

/*! 
 * TbMatrix型頂点シェーダ定数設定( ハンドルから )
 * @author teco
 */
void TbShaderManager::SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbMatrix43* value , TbUint32 count )
{
    TB_ASSERT(!"need Constant Buffer");
}

/*!
 * 定数バッファ設定
 * @author teco
 */
void TbShaderManager::SetConstantBuffer(TbUint32 shaderBit, TbShaderConstHandle& handle, TbConstantBuffer* buffer)
{
    nn::gfx::CommandBuffer* commandBuffer = TbGetGfxCommandBuffer();
    if (updateConstHandle(shaderBit, m_ShaderProgram, handle.m_Depend)) {
        if (!TbRenderDevice::GetInstance().GetDepend().IsRecordingCommand()) {
            return;
        }
        for (TbUint32 i = TB_SHADER_TYPE_BEGIN; i < TB_SHADER_TYPE_END; ++i) {
            TbShaderType type = static_cast<TbShaderType>(i);
            if (!(shaderBit & TB_BIT(i))) {
                continue;
            }
            nn::gfx::ShaderStage stage = TbShaderTypeToGfx(type);
            
#if TB_CONSTANT_BUFFER_ENABLE_DESCRIPTOR
            commandBuffer->SetDescriptorPool(buffer->GetDepend().GetDescriptorPool());
            commandBuffer->SetConstantBuffer(handle.m_Depend.handle[i],stage,*buffer->GetDepend().GetDescriptorSlot());
#else
            nn::gfx::GpuAddress gpuAddress;
            buffer->GetDepend().GetGpuAddress(&gpuAddress);
            commandBuffer->SetConstantBuffer(handle.m_Depend.handle[i], stage, gpuAddress, buffer->GetSize());
#endif
        }
    }
}

/*!
 * TbVertexElement::Usage に対応したシェーダコード内の変数名取得
 * @author teco
 */
const TbChar8* TbShaderManager::GetElementUsageVarName( TbVertexElement::Usage usage )
{
    if( 0 <= usage && usage < TB_ARRAY_COUNT_OF( s_UsageVarName ) ){
        return s_UsageVarName[ usage ].GetBuffer();
    }
    return "";
}

/*!
 * TbVertexElement::Usage に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
 * @author teco
 */
void TbShaderManager::SetElementUsageVarName( TbVertexElement::Usage usage , const TbChar8* name )
{
    if( 0 <= usage && usage < TB_ARRAY_COUNT_OF( s_UsageVarName ) ){
        s_UsageVarName[ usage ] = name;
    }
}

/*!
 * TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名取得
 * @author teco
 */
const TbChar8* TbShaderManager::GetElementUsageUserVarName( TbSint32 index )
{
    if( 0 <= index && index < TB_ARRAY_COUNT_OF( s_UsageVarNameUser ) ){
        return s_UsageVarNameUser[ index ].GetBuffer();
    }
    return "";
}

/*!
 * TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
 * @author teco
 */
void TbShaderManager::SetElementUsageUserVarName( TbSint32 index , const TbChar8* name )
{
    if( 0 <= index && index < TB_ARRAY_COUNT_OF( s_UsageVarNameUser ) ){
        s_UsageVarNameUser[ index ] = name;
    }
}

/*!
 * テクスチャ変数名取得
 * @author teco
 */
const TbChar8* TbShaderManager::GetTextureVarName()
{
    return s_TextureVarName.GetBuffer();
}

/*!
 * テクスチャ変数名設定
 * @author teco
 */
void TbShaderManager::SetTextureVarName( const TbChar8* name )
{
    s_TextureVarName = name;
}

TbFloat32 rotY = 0.0f;

/*!
 * world view projection行列更新
 * @author teco
 */
void TbShaderManager::updateWVP()
{
    TbMatrix mtx = m_Matrix[MATRIX_PROJECTION] * m_Matrix[MATRIX_VIEW] * m_Matrix[MATRIX_WORLD];
    TbConstantBuffer* buffer = m_Depend.m_ConstantBufferWVP.Allocate();
    if(TB_VERIFY(buffer)){
        buffer->Write(0,sizeof(TbMatrix),&mtx);
        SetConstantBuffer(TB_SHADER_TYPE_BIT_VERTEX, m_ConstHandle[CONST_HANDLE_WVP], buffer);
    }
}

}

#endif