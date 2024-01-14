/*!
 * @file
 * @brief レンダービュークラス
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_view.h"
#include <nn/nn_Assert.h>

#if TB_3DAPI_IS_GFX

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"
#include "graphics/window/tb_window.h"

namespace toybox
{

        const int RenderTargetWidth = 1280;
        const int RenderTargetHeight = 720;

        const char* const VsGlsl =
            "#version 430                                            \n"
            "out gl_PerVertex                                        \n"
            "{                                                       \n"
            "   vec4 gl_Position;                                    \n"
            "};                                                      \n"
            "out vec4 v_FrontColor;                                  \n"
            "out vec2 v_Texcoord0;                                   \n"
            "in vec4 a_Position;                                     \n"
            "in vec4 a_Color;                                       \n"
            "in vec2 a_Texcoord;                                    \n"
            "                                                        \n"
            "layout( std140 ) uniform u_ModelViewProjMatrix         \n"
            "{                                                      \n"
            "   uniform mat4 mat;                                   \n"
            "};                                                     \n"
            "                                                        \n"
            "void main()                                             \n"
            "{                                                       \n"
            "    gl_Position =  mat * a_Position;                   \n"
            "    v_FrontColor = a_Color;                            \n"
            "    v_Texcoord0  = a_Texcoord;                         \n"
            "}                                                       \n";


        const char* const PsGlsl =
            "#version 430                                            \n"
            "layout( location = 0 ) out vec4 o_Color;                \n"
            "uniform sampler2D u_Texture0;                           \n"
            "in   vec4 v_FrontColor;                                 \n"
            "in   vec2 v_Texcoord0;                                  \n"
            "void main() {                                           \n"
            "    vec4 texColor = texture( u_Texture0, v_Texcoord0 ); \n"
            "    o_Color = v_FrontColor * texColor;                  \n"
            "}                                                       \n";

        const float Vertices[] =
        {
            -0.5f, -0.5f, 0.0f, 1.0f , 1.0f , 1.0f, 1.0f , -0.5f, -0.5f,
            -0.5f, 0.5f, 0.0f, 1.0f , 1.0f , 1.0f, 1.0f , 0.5f, 0.5f,
            0.5f, -0.5f, 0.0f, 1.0f , 1.0f , 1.0f, 1.0f , 0.5f, -0.5f,
            0.5f, 0.5f, 0.0f, 1.0f , 1.0f , 1.0f, 1.0f , 0.5f, 0.5f,
        };

        int g_BufferDescriptorBaseIndex = 0;
        int g_TextureDescriptorBaseIndex = 0;
        int g_SamplerDescriptorBaseIndex = 0;

        //-----------------------------------------------------------------------------
        // メモリ

        const size_t GraphicsSystemMemorySize = 8 * 1024 * 1024;

        nn::util::BytePtr g_pMemoryHeap(NULL);
        nn::util::BytePtr g_pMemory(NULL);

        enum MemoryPoolType
        {
            MemoryPoolType_CpuCached_GpuCached,
            MemoryPoolType_CpuUncached_GpuCached,
            MemoryPoolType_CpuInvisible_GpuCached_Compressible,

            MemoryPoolType_End
        };

        const size_t MemoryPoolSize[MemoryPoolType_End] =
        {
            16 * 1024 * 1024,
            16 * 1024 * 1024,
            20 * 1024 * 1024
        };

        void* g_pPoolMemory[MemoryPoolType_End] = {};
        ptrdiff_t g_MemoryPoolOffset[MemoryPoolType_End] = {};

        nn::gfx::MemoryPool g_MemoryPool[MemoryPoolType_End];
        void InitializeMemoryPool()
        {
            // メモリ
            g_pMemoryHeap.Reset(malloc(1024 * 1024));
            g_pMemory = g_pMemoryHeap;

            const int MemoryPoolProperty[MemoryPoolType_End] =
            {
                nn::gfx::MemoryPoolProperty_CpuCached | nn::gfx::MemoryPoolProperty_GpuCached,
                nn::gfx::MemoryPoolProperty_CpuUncached | nn::gfx::MemoryPoolProperty_GpuCached,
                nn::gfx::MemoryPoolProperty_CpuInvisible | nn::gfx::MemoryPoolProperty_GpuCached
                | nn::gfx::MemoryPoolProperty_Compressible
            };

            nn::gfx::MemoryPool::InfoType info;
            for (int idxMemoryPool = 0; idxMemoryPool < MemoryPoolType_End; ++idxMemoryPool)
            {
                info.SetDefault();
                info.SetMemoryPoolProperty(MemoryPoolProperty[idxMemoryPool]);
                size_t alignment = nn::gfx::MemoryPool::GetPoolMemoryAlignment(TbGetGfxDevice(), info);
                size_t granularity = nn::gfx::MemoryPool::GetPoolMemorySizeGranularity(TbGetGfxDevice(), info);
                g_pPoolMemory[idxMemoryPool] = malloc(MemoryPoolSize[idxMemoryPool]);
                void* pPoolMemoryAligned = nn::util::BytePtr(
                    g_pPoolMemory[idxMemoryPool]).AlignUp(alignment).Get();
                size_t memoryPoolSizeAligned = nn::util::align_down(MemoryPoolSize[idxMemoryPool], granularity);
                info.SetPoolMemory(pPoolMemoryAligned, memoryPoolSizeAligned);
                g_MemoryPool[idxMemoryPool].Initialize(TbGetGfxDevice(), info);
                g_MemoryPoolOffset[idxMemoryPool] = 0;
            }
        }

        nn::gfx::CommandBuffer g_CommandBuffer;
        void InitializeCommandBuffer()
        {
            nn::gfx::CommandBuffer::InfoType info;
            info.SetDefault();
            info.SetQueueCapability(nn::gfx::QueueCapability_Graphics);
            info.SetCommandBufferType(nn::gfx::CommandBufferType_Direct);
            g_CommandBuffer.Initialize(TbGetGfxDevice(), info);
        }

        // 頂点シェーダを初期化
        nn::gfx::Shader g_VertexShader;
        int g_SlotMat;
        void InitializeVertexShader()
        {
            nn::gfx::Shader::InfoType info;
            info.SetDefault();
            info.SetSeparationEnabled(true);

            nn::gfx::ShaderCode code;
            code.codeSize = static_cast< uint32_t >(strlen(VsGlsl));
            code.pCode = VsGlsl;
            info.SetShaderCodePtr(nn::gfx::ShaderStage_Vertex, &code);
            info.SetSourceFormat(nn::gfx::ShaderSourceFormat_Glsl);
            info.SetCodeType(nn::gfx::ShaderCodeType_Source);

            nn::gfx::ShaderInitializeResult result = g_VertexShader.Initialize(TbGetGfxDevice(), info);
            NN_ASSERT(result == nn::gfx::ShaderInitializeResult_Success);
            NN_UNUSED(result);
            g_SlotMat = g_VertexShader.GetInterfaceSlot(nn::gfx::ShaderStage_Vertex,
                                    nn::gfx::ShaderInterfaceType_ConstantBuffer, "u_ModelViewProjMatrix");
        }
        // ピクセルシェーダを初期化
        nn::gfx::Shader g_PixelShader;
        int g_SlotTex;
        void InitializePixelShader()
        {
            nn::gfx::Shader::InfoType info;
            info.SetDefault();
            info.SetSeparationEnabled(true);
            nn::gfx::ShaderCode code;
            code.codeSize = static_cast< uint32_t >(strlen(PsGlsl));
            code.pCode = PsGlsl;
            info.SetShaderCodePtr(nn::gfx::ShaderStage_Pixel, &code);
            info.SetSourceFormat(nn::gfx::ShaderSourceFormat_Glsl);
            info.SetCodeType(nn::gfx::ShaderCodeType_Source);

            nn::gfx::ShaderInitializeResult result = g_PixelShader.Initialize(TbGetGfxDevice(), info);
            NN_ASSERT(result == nn::gfx::ShaderInitializeResult_Success);
            NN_UNUSED(result);

            g_SlotTex = g_PixelShader.GetInterfaceSlot(nn::gfx::ShaderStage_Pixel,
                nn::gfx::ShaderInterfaceType_Sampler, "u_Texture0");
        }

        // Initialize the vertex state
        nn::gfx::VertexState g_VertexState;
        static void InitializeVertexState()
        {
            nn::gfx::VertexState::InfoType info;
            info.SetDefault();
            ptrdiff_t stride = sizeof(float) * 9;
            nn::gfx::VertexAttributeStateInfo attribs[3];
            {
                attribs[0].SetDefault();
                attribs[0].SetNamePtr("a_Position");
                attribs[0].SetBufferIndex(0);
                attribs[0].SetFormat(nn::gfx::AttributeFormat_32_32_32_Float);
                attribs[0].SetOffset(0);
            }
            {
                attribs[1].SetDefault();
                attribs[1].SetNamePtr("a_Color");
                attribs[1].SetBufferIndex(0);
                attribs[1].SetFormat(nn::gfx::AttributeFormat_32_32_32_32_Float);
                attribs[1].SetOffset(sizeof(float) * 3);
            }
            {
                attribs[2].SetDefault();
                attribs[2].SetNamePtr("a_Texcoord");
                attribs[2].SetBufferIndex(0);
                attribs[2].SetFormat(nn::gfx::AttributeFormat_32_32_Float);
                attribs[2].SetOffset(sizeof(float) * 7);
            }
            nn::gfx::VertexBufferStateInfo buffer;
            {
                buffer.SetDefault();
                buffer.SetStride(stride);
            }
            info.SetVertexAttributeStateInfoArray(attribs, 3);
            info.SetVertexBufferStateInfoArray(&buffer, 1);
            size_t size = nn::gfx::VertexState::GetRequiredMemorySize(info);
            g_pMemory.AlignUp(nn::gfx::VertexState::RequiredMemoryInfo_Alignment);
            g_VertexState.SetMemory(g_pMemory.Get(), size);
            g_pMemory.Advance(size);
            g_VertexState.Initialize(TbGetGfxDevice(), info, &g_VertexShader);
        }

        // 頂点用のバッファを初期化
        nn::gfx::Buffer g_VertexBuffer;
        void InitializeVertexBuffer()
        {
            nn::gfx::Buffer::InfoType info;
            info.SetDefault();
            info.SetSize(sizeof(Vertices));
            info.SetGpuAccessFlags(nn::gfx::GpuAccess_VertexBuffer);
            if (NN_STATIC_CONDITION(nn::gfx::Buffer::IsMemoryPoolRequired))
            {
                ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuUncached_GpuCached];
                offset = nn::util::align_up(offset, nn::gfx::Buffer::GetBufferAlignment(TbGetGfxDevice(), info));
                g_VertexBuffer.Initialize(TbGetGfxDevice(), info,
                    &g_MemoryPool[MemoryPoolType_CpuUncached_GpuCached], offset, info.GetSize());
                offset += info.GetSize();
            }
            else
            {
                g_VertexBuffer.Initialize(TbGetGfxDevice(), info, NULL, 0, 0);
            }

            void* pMapped = g_VertexBuffer.Map();
            memcpy(pMapped, Vertices, info.GetSize());
//            g_VertexBuffer.FlushMappedRange(0, info.GetSize());
            g_VertexBuffer.Unmap();
        }

        // 定数バッファ用のバッファを初期化
        nn::gfx::Buffer g_ConstantBuffer;
        void InitializeConstantBuffer()
        {
            nn::gfx::Buffer::InfoType info;
            info.SetDefault();
            TbMatrix mtx = TbMatrix::IDENTITY;
            info.SetSize(sizeof(mtx));
            info.SetGpuAccessFlags(nn::gfx::GpuAccess_ConstantBuffer);
            {
                ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuUncached_GpuCached];
                offset = nn::util::align_up(offset, nn::gfx::Buffer::GetBufferAlignment(TbGetGfxDevice(), info));
                g_ConstantBuffer.Initialize(TbGetGfxDevice(), info,
                    &g_MemoryPool[MemoryPoolType_CpuUncached_GpuCached], offset, info.GetSize());
                offset += info.GetSize();
            }
            // 定数バッファを更新
            float* pConstantBuffer = g_ConstantBuffer.Map< float >();
            {
                TbMemCopy(pConstantBuffer,&mtx,sizeof(mtx));
            }
            g_ConstantBuffer.Unmap();

        }

        // テクスチャを初期化
        nn::gfx::Texture g_Texture;
        void InitializeLinearTexture()
        {
            nn::gfx::Texture::InfoType info;
            info.SetDefault();
            info.SetGpuAccessFlags(nn::gfx::GpuAccess_Texture);
            info.SetWidth(256);
            info.SetHeight(256);
            info.SetImageStorageDimension(nn::gfx::ImageStorageDimension_2d);
            info.SetImageFormat(nn::gfx::ImageFormat_R8_G8_B8_A8_Unorm);
            info.SetMipCount(1);
            info.SetTileMode(nn::gfx::TileMode_Linear);

            size_t alignment = nn::gfx::Texture::CalculateMipDataAlignment(TbGetGfxDevice(), info);
            size_t size = nn::gfx::Texture::CalculateMipDataSize(TbGetGfxDevice(), info);
            size_t pitch = nn::gfx::Texture::GetRowPitch(TbGetGfxDevice(), info);
            ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuCached_GpuCached];
            offset = nn::util::align_up(offset, alignment);

            nn::gfx::MemoryPool& memoryPool = g_MemoryPool[MemoryPoolType_CpuCached_GpuCached];
            uint8_t* pixels = nn::util::BytePtr(memoryPool.Map(), offset).Get< uint8_t >();
            for (int y = 0; y < info.GetHeight(); ++y)
            {
                uint8_t* row = pixels + y * pitch;
                for (int x = 0; x < info.GetWidth(); ++x)
                {
                    uint8_t* pixel = row + x * 4;
                    pixel[0] = static_cast< uint8_t >(x);
                    pixel[1] = static_cast< uint8_t >(y);
                    pixel[2] = 0x00;
                    pixel[3] = 0xFF;
                }
            }
            memoryPool.FlushMappedRange(offset, size);
            memoryPool.Unmap();

            g_Texture.Initialize(TbGetGfxDevice(), info, &memoryPool, offset, size);
            offset += size;
        };

        // テクスチャビューを初期化
        nn::gfx::TextureView g_TextureView;
        void InitializeLinearTextureView()
        {
            nn::gfx::TextureView::InfoType info;
            info.SetDefault();
            info.SetImageDimension(nn::gfx::ImageDimension_2d);
            info.SetImageFormat(nn::gfx::ImageFormat_R8_G8_B8_A8_Unorm);
            info.SetTexturePtr(&g_Texture);
            info.SetChannelMapping(nn::gfx::ChannelMapping_Red, nn::gfx::ChannelMapping_Green,
                nn::gfx::ChannelMapping_Blue, nn::gfx::ChannelMapping_Alpha);
            g_TextureView.Initialize(TbGetGfxDevice(), info);
        }

        // サンプラを初期化
        nn::gfx::Sampler g_Sampler;
        void InitializeSampler()
        {
            nn::gfx::Sampler::InfoType info;
            info.SetDefault();
            info.SetFilterMode(nn::gfx::FilterMode_MinLinear_MagLinear_MipPoint);
            info.SetAddressU(nn::gfx::TextureAddressMode_Mirror);
            info.SetAddressV(nn::gfx::TextureAddressMode_Mirror);
            info.SetAddressW(nn::gfx::TextureAddressMode_Mirror);
            g_Sampler.Initialize(TbGetGfxDevice(), info);
        }

        nn::gfx::DescriptorPool g_BufferDescriptorPool;
        void InitializeBufferDescriptorPool()
        {
            nn::gfx::DescriptorPool::InfoType info;
            info.SetDefault();
            info.SetDescriptorPoolType(nn::gfx::DescriptorPoolType_BufferView);
            info.SetSlotCount(g_BufferDescriptorBaseIndex + 1);
            size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize(TbGetGfxDevice(), info);
            ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuUncached_GpuCached];
            offset = nn::util::align_up(offset, nn::gfx::DescriptorPool::GetDescriptorPoolAlignment(TbGetGfxDevice(), info));
            g_BufferDescriptorPool.Initialize(TbGetGfxDevice(), info,
                &g_MemoryPool[MemoryPoolType_CpuUncached_GpuCached], offset, size);
            offset += size;
        }

        nn::gfx::DescriptorPool g_SamplerDescriptorPool;
        void InitializeSamplerDescriptorPool()
        {
            nn::gfx::DescriptorPool::InfoType info;
            info.SetDefault();
            info.SetDescriptorPoolType(nn::gfx::DescriptorPoolType_Sampler);
            info.SetSlotCount(g_SamplerDescriptorBaseIndex + 1);
            size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize(TbGetGfxDevice(), info);
            ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuUncached_GpuCached];
            offset = nn::util::align_up(offset, nn::gfx::DescriptorPool::GetDescriptorPoolAlignment(TbGetGfxDevice(), info));
            g_SamplerDescriptorPool.Initialize(TbGetGfxDevice(), info,
                &g_MemoryPool[MemoryPoolType_CpuUncached_GpuCached], offset, size);
            offset += size;
        }

        nn::gfx::DescriptorPool g_TextureDescriptorPool;
        void InitializeTextureDescriptorPool()
        {
            nn::gfx::DescriptorPool::InfoType info;
            info.SetDefault();
            info.SetDescriptorPoolType(nn::gfx::DescriptorPoolType_TextureView);
            info.SetSlotCount(g_TextureDescriptorBaseIndex + 2);
            size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize(TbGetGfxDevice(), info);
            ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuUncached_GpuCached];
            offset = nn::util::align_up(offset, nn::gfx::DescriptorPool::GetDescriptorPoolAlignment(TbGetGfxDevice(), info));
            g_TextureDescriptorPool.Initialize(TbGetGfxDevice(), info,
                &g_MemoryPool[MemoryPoolType_CpuUncached_GpuCached], offset, size);
            offset += size;
        }
        void InitializeGfxObjects()
        {
#if NN_GFX_IS_TARGET_NVN
            nn::gfx::Device::DataType& deviceData = nn::gfx::AccessorToData(*TbGetGfxDevice());
            nvnDeviceGetInteger(deviceData.pNvnDevice,
                NVN_DEVICE_INFO_RESERVED_TEXTURE_DESCRIPTORS, &g_TextureDescriptorBaseIndex);
            nvnDeviceGetInteger(deviceData.pNvnDevice,
                NVN_DEVICE_INFO_RESERVED_SAMPLER_DESCRIPTORS, &g_SamplerDescriptorBaseIndex);
#endif

            InitializeCommandBuffer();
            InitializeVertexShader();
            InitializePixelShader();

            {
                InitializeVertexState();
            }

            InitializeVertexBuffer();
            InitializeConstantBuffer();

            InitializeLinearTexture();
            InitializeLinearTextureView();
            InitializeSampler();

            {
                InitializeBufferDescriptorPool();
                InitializeSamplerDescriptorPool();
                InitializeTextureDescriptorPool();
            }

            NN_ASSERT(g_pMemoryHeap.Distance(g_pMemory.Get()) < 1024 * 1024);
            for (int idxMemoryPool = 0; idxMemoryPool < MemoryPoolType_End; ++idxMemoryPool)
            {
                NN_ASSERT(static_cast< size_t >(g_MemoryPoolOffset[idxMemoryPool])
                    < MemoryPoolSize[idxMemoryPool]);
            }
            {
                g_BufferDescriptorPool.BeginUpdate();
                {
                    nn::gfx::GpuAddress gpuAddress;
                    g_ConstantBuffer.GetGpuAddress(&gpuAddress);
                    g_BufferDescriptorPool.SetBufferView(g_BufferDescriptorBaseIndex, gpuAddress, sizeof(float) * 16);
                }
                g_BufferDescriptorPool.EndUpdate();

                g_TextureDescriptorPool.BeginUpdate();
                {
                    g_TextureDescriptorPool.SetTextureView(g_TextureDescriptorBaseIndex + 1, &g_TextureView);
                }
                g_TextureDescriptorPool.EndUpdate();

                g_SamplerDescriptorPool.BeginUpdate();
                {
                    g_SamplerDescriptorPool.SetSampler(g_SamplerDescriptorBaseIndex, &g_Sampler);
                }
                g_SamplerDescriptorPool.EndUpdate();
            }
        }

        void OutOfCommandMemoryEventCallback(
            nn::gfx::CommandBuffer* pCommandBuffer, const nn::gfx::OutOfMemoryEventArg& arg)
        {
            NN_ASSERT(arg.minRequiredSize <= 1024);
            NN_UNUSED(arg);
            ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuUncached_GpuCached];
            offset = nn::util::align_up(offset, nn::gfx::CommandBuffer::GetCommandMemoryAlignment(TbGetGfxDevice()));
            pCommandBuffer->AddCommandMemory(
                &g_MemoryPool[MemoryPoolType_CpuUncached_GpuCached], offset, 1024);
            offset += 1024;
        }

        void OutOfControlMemoryEventCallback(
            nn::gfx::CommandBuffer* pCommandBuffer, const nn::gfx::OutOfMemoryEventArg& arg)
        {
            NN_ASSERT(arg.minRequiredSize <= 256);
            NN_UNUSED(arg);
            g_pMemory.AlignUp(nn::gfx::CommandBuffer::GetControlMemoryAlignment(TbGetGfxDevice()));
            pCommandBuffer->AddControlMemory(g_pMemory.Get(), 256);
            g_pMemory.Advance(256);
        }

        void MakeCommand()
        {
            nn::gfx::GpuAddress gpuAddress;

            g_CommandBuffer.Reset();
            ptrdiff_t& offset = g_MemoryPoolOffset[MemoryPoolType_CpuUncached_GpuCached];
            offset = nn::util::align_up(offset, nn::gfx::CommandBuffer::GetCommandMemoryAlignment(TbGetGfxDevice()));
            g_CommandBuffer.AddCommandMemory(
                &g_MemoryPool[MemoryPoolType_CpuUncached_GpuCached], offset, 1024);
            offset += 1024;
            g_pMemory.AlignUp(nn::gfx::CommandBuffer::GetControlMemoryAlignment(TbGetGfxDevice()));
            g_CommandBuffer.AddControlMemory(g_pMemory.Get(), 256);
            g_pMemory.Advance(256);
            g_CommandBuffer.SetOutOfCommandMemoryEventCallback(OutOfCommandMemoryEventCallback);
            g_CommandBuffer.SetOutOfControlMemoryEventCallback(OutOfControlMemoryEventCallback);
            g_CommandBuffer.Begin();
            {
#if 1
                g_VertexBuffer.GetGpuAddress(&gpuAddress);
                g_CommandBuffer.SetVertexBuffer(0, gpuAddress, sizeof(float) * 9, sizeof(Vertices));

                {
                    g_CommandBuffer.SetDescriptorPool(&g_BufferDescriptorPool);
                    g_CommandBuffer.SetDescriptorPool(&g_TextureDescriptorPool);
                    g_CommandBuffer.SetDescriptorPool(&g_SamplerDescriptorPool);
                }

                {
                    g_CommandBuffer.SetShader(&g_VertexShader, nn::gfx::ShaderStageBit_Vertex);
                    g_CommandBuffer.SetShader(&g_PixelShader, nn::gfx::ShaderStageBit_Pixel);
                    g_CommandBuffer.SetVertexState(&g_VertexState);
                }

                {
                    nn::gfx::DescriptorSlot constantBufferDescriptor;
                    nn::gfx::DescriptorSlot textureDescriptor;
                    nn::gfx::DescriptorSlot samplerDescriptor;
                    g_BufferDescriptorPool.GetDescriptorSlot(&constantBufferDescriptor, g_BufferDescriptorBaseIndex);
                    g_TextureDescriptorPool.GetDescriptorSlot(&textureDescriptor, g_TextureDescriptorBaseIndex);
                    g_SamplerDescriptorPool.GetDescriptorSlot(&samplerDescriptor, g_SamplerDescriptorBaseIndex);
                    {
                        textureDescriptor.Offset(nn::gfx::DescriptorPool::GetDescriptorSlotIncrementSize(
                            TbGetGfxDevice(), nn::gfx::DescriptorPoolType_TextureView));
                        g_CommandBuffer.SetConstantBuffer(g_SlotMat,
                            nn::gfx::ShaderStage_Vertex, constantBufferDescriptor);
                        g_CommandBuffer.SetTextureAndSampler(g_SlotTex,
                            nn::gfx::ShaderStage_Pixel, textureDescriptor, samplerDescriptor);
                    }
                }
                g_CommandBuffer.Draw(nn::gfx::PrimitiveTopology_TriangleStrip,4,0);
#endif
            }
            g_CommandBuffer.End();
        } // NOLINT

/////////////////////////////////////////////////////////////////////
// 依存コード

/*!
 * コンストラクタ
 * @parma device 対応デバイス
 * @param param 生成パラメータ
 * @author teco
 */
TbRenderViewDepend::TbRenderViewDepend( const TbRenderViewParameter& param )
    : m_DepthStencil(nullptr)
    , m_CurrentRenderTarget(0)
{
    TbFillNull(m_RenderTarget);
    TbFillNull(m_ScanBuffers);
    nn::gfx::SwapChain::InfoType info;

    nn::vi::Layer* layer = param.window->GetDepend().layer;
    nn::gfx::Device* device = TbGetGfxDevice();
    info.SetDefault();
    info.SetLayer(layer);
    info.SetWidth(param.backBufferWidth);
    info.SetHeight(param.backBufferHeight);
    info.SetFormat(nn::gfx::ImageFormat_R8_G8_B8_A8_UnormSrgb);
    info.SetBufferCount(2);
    if (NN_STATIC_CONDITION(nn::gfx::SwapChain::IsMemoryPoolRequired))
    {
        size_t size = m_SwapChain.CalculateScanBufferSize(device, info);
        size_t align = nn::gfx::SwapChain::GetScanBufferAlignment(device, info);
        int property = nn::gfx::MemoryPoolProperty_CpuInvisible |
                       nn::gfx::MemoryPoolProperty_GpuCached |
                       nn::gfx::MemoryPoolProperty_Compressible;
        m_MemoryPool.Initialize(property,size,align);
        m_SwapChain.Initialize(device, info, m_MemoryPool.Get(), 0, size);
    }
    else
    {
        m_SwapChain.Initialize(device, info, nullptr, 0, 0);
    }
    m_SwapChain.GetScanBufferViews(m_ScanBuffers, TB_ARRAY_COUNT_OF(m_ScanBuffers));

    {
        nn::gfx::Semaphore::InfoType displayInfo;
        displayInfo.SetDefault();
        m_DisplaySemaphore.Initialize(device, displayInfo);
    }

    if (param.enableCreateDepthStencil)
    {
        // 深度ステンシルを生成
        TbTextureDesc desc;
        desc.colorFormat = TB_COLOR_FORMAT_D16;
        desc.flags = TbTextureDesc::FLAG_BIT_DEPTH_STENCIL;
        desc.width = param.backBufferWidth;
        desc.height = param.backBufferHeight;
        m_DepthStencil = new TbTexture(desc);
    }

    InitializeMemoryPool();
    InitializeGfxObjects();
    MakeCommand();

    int next = 0;
    m_SwapChain.AcquireNextScanBufferIndex(&next, &m_DisplaySemaphore, nullptr);
    m_CurrentRenderTarget = next;
}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderViewDepend::~TbRenderViewDepend()
{
    nn::gfx::Device* device = TbGetGfxDevice();
    m_DisplaySemaphore.Finalize(device);
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_RenderTarget); ++i){
        TB_SAFE_DELETE(m_RenderTarget[i]);
    }
    m_SwapChain.Finalize(device);
    m_MemoryPool.Finalize();
    TB_SAFE_DELETE(m_DepthStencil);
}

/*!
* 同期
* @author teco
*/
void TbRenderViewDepend::Sync()
{
    TbRenderDevice::GetInstance().GetDepend().GetQueue().SyncSemaphore(&m_DisplaySemaphore);
}


/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
TbRenderView::TbRenderView( const TbRenderViewParameter& param , TbSint32 index )
    : m_BitArray()
    , m_DepthStencil( nullptr )
    , m_Depend( param )
    , m_FrontBufferFormat( TB_COLOR_FORMAT_X8R8G8B8 )
{
    TB_ASSERT(index == -1);
    m_Param = param;
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Depend.m_RenderTarget); ++i)
    {
        m_Depend.m_RenderTarget[i] = TB_NEW TbRenderTarget(this, i);
    }
    if( m_Param.enableCreateDepthStencil ){
        // 0番目のビューをコピー(DirectX9は、SwapChain毎ではなく自動生成されるのは一つのデバイスに一つのため)
        m_DepthStencil = m_Depend.m_DepthStencil->GetDepthStencil();
    }
}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderView::~TbRenderView()
{
}

/*!
 * 画面に表示
 * @author teco
 */
void TbRenderView::Present()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    nn::gfx::Queue& queue = device.GetDepend().GetQueue();

//    queue.ExecuteCommand(&g_CommandBuffer,nullptr);
    queue.Present(&m_Depend.m_SwapChain, 1);
    queue.Flush();
    queue.Sync();

    int next = 0;
    m_Depend.m_SwapChain.AcquireNextScanBufferIndex(&next,&m_Depend.m_DisplaySemaphore,nullptr);
    m_Depend.m_CurrentRenderTarget = next;
}

/*!
 * レンダーターゲット取得
 * @author teco
 */
TbRenderTarget* TbRenderView::GetRenderTarget( TbUint32 backBufferIndex )
{
    TB_ASSERT(0== backBufferIndex);
    return m_Depend.m_RenderTarget[m_Depend.m_CurrentRenderTarget];
}

/*!
 * フロントバッファの横幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetFrontBufferWidth()
{
    return m_Param.backBufferWidth;
}

/*!
 * フロントバッファのピッチ幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetFrontBufferPitch()
{
    return m_Param.backBufferHeight * TbGetColorFormatByteSize(m_FrontBufferFormat);
}

/*!
 * フロントバッファの縦幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetFrontBufferHeight()
{
    return m_Param.backBufferHeight;
}

/*!
 * フロントバッファのフォーマット取得
 * @author teco
 */
TbColorFormat TbRenderView::GetFrontBufferFormat()
{
    return m_FrontBufferFormat;
}

/*!
 * フロントバッファ読み込み
 * @author teco
 */
TbResult TbRenderView::ReadFrontBuffer( void* readBuffer , TbUint32 readBufferSize , const TbRectS32* readRect , TbBool* isDownToUp )
{
    TB_ASSERT(!"unsupported");
    return TB_S_OK;
}

/*!
 * バックバッファ読み込み
 * @author teco
 */
TbUint32 TbRenderView::GetBackBufferWidth()
{
    TbUint32 result = m_Param.backBufferWidth;
    return result;
}

/*!
 * バックバッファのピッチ幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetBackBufferPitch()
{
    TbUint32 result = m_Param.backBufferWidth * TbGetColorFormatByteSize(m_Param.backBufferFormat);
    return result;
}

/*!
 * バックバッファの縦幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetBackBufferHeight()
{
    TbUint32 result = m_Param.backBufferHeight;
    return result;
}

/*!
 * バックバッファのフォーマット取得
 * @author teco
 */
TbColorFormat TbRenderView::GetBackBufferFormat()
{
    return m_Param.backBufferFormat;
}
    
/*!
 * バックバッファの読み込み
 * @author teco
 */
TbResult TbRenderView::ReadBackBuffer( void* readBuffer , TbUint32 readBufferSize , const TbRectS32* readRect , TbBool* isDownToUp )
{
    TB_ASSERT(!"unsupported");
    return TB_S_OK;
}

}

#endif
