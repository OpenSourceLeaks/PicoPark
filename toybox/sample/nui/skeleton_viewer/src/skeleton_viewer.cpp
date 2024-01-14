/*!
 * 骨格ビューワ
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */

#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/image/tb_image_bmp.h>
#include <graphics/image/tb_image_tga.h>
#include <base/io/tb_file.h>
#include <base/io/tb_file_manager.h>
#include <input/tb_mouse.h>
#include <nui/tb_nui.h>
#include <framework/tb_framework.h>
#include <graphics/font/tb_font.h>
#include <graphics/font/tb_font_buffer_2d.h>

#include "./skeleton_viewer.h"

using namespace toybox;

namespace
{

// 頂点フォーマット要素
static const TbVertexElement SIMPLE_VERTEX_ELEMENT[] = 
{
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_COLOR , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
};

}

/*! 
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
SkeletonViewer::SkeletonViewer()
    : m_Phase(0)
    , m_BitArray()
    , m_NuiManager( NULL )
    , m_PlayerSkeletonIndex(-1)
    , m_SkeletonVertexBuffer( NULL )
    , m_SkeletonVertexFormat( NULL )
    , m_ColorTexture( NULL ) 
    , m_DepthTexture( NULL )
{
    // NuiManager
    {
        TbNuiCreateParam nuiCreateParam;
        nuiCreateParam.flagBit = TB_NUI_INITIALIZE_BIT_USE_COLOR | TB_NUI_INITIALIZE_BIT_USE_DEPTH | TB_NUI_INITIALIZE_BIT_USE_SKELETON;
        m_NuiManager = new TbNuiManager( nuiCreateParam );
    }

    // 頂点フォーマット、頂点バッファ作成
    {
        m_SkeletonVertexFormat = new TbVertexFormat( SIMPLE_VERTEX_ELEMENT , TB_ARRAY_COUNT_OF(SIMPLE_VERTEX_ELEMENT) );
        m_SkeletonVertexBuffer = new TbVertexBuffer( m_SkeletonVertexFormat ,  sizeof(m_SkeletonVertex));
        for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_SkeletonVertex); ++i ){
            m_SkeletonVertex[i].x = 0.0f;
            m_SkeletonVertex[i].y = 0.0f;
            m_SkeletonVertex[i].z = 0.1f;
            m_SkeletonVertex[i].color = TB_COLOR_A8R8G8B8( 0xFF , 0xFF , 0xFF , 0xFF );
        }
    }

}

/*! 
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
SkeletonViewer::~SkeletonViewer()
{
    TB_SAFE_DELETE( m_SkeletonVertexBuffer );
    TB_SAFE_DELETE( m_SkeletonVertexFormat );
    TB_SAFE_DELETE( m_ColorTexture );
    TB_SAFE_DELETE( m_DepthTexture );
    TB_SAFE_DELETE( m_NuiManager );
}

/*! 
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
void SkeletonViewer::Update()
{
    m_NuiManager->Update();
    switch( m_Phase )
    {
    case 0:
        if( !m_NuiManager->IsReady() ){
            break;
        }
        // カラーテクスチャ生成
        {
            TbTextureDesc texDesc;
            TbDimensionS32 size = TbTexture::GetTextureSize(m_NuiManager->GetColorFrame()->GetResolution());
            texDesc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
            texDesc.flags = 0;
            texDesc.width = size.width;
            texDesc.height = size.height;
            texDesc.levels = 1;
            m_ColorTexture = new TbTexture( texDesc );
        }

        // 深度テクスチャ
        {
            TbTextureDesc texDesc;
            TbDimensionS32 size = TbTexture::GetTextureSize(m_NuiManager->GetDepthFrame()->GetResolution());
            texDesc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
            texDesc.flags = 0;
            texDesc.width = size.width;
            texDesc.height = size.height;
            texDesc.levels = 1;
            m_DepthTexture = new TbTexture( texDesc );
        }
        ++m_Phase;
        break;
    case 1:
        updatePlayerSkeletonIndex();
        updateColorTexture();
        updateDepthTexture();
        updateSkeletonVertexBuffer();
        break;
    }
}

/*!
 * カラー用テクスチャファイル出力
 * @author Miyake Shunsuke
 * @since 2011.08.25
 */
TbResult SkeletonViewer::ExportColorImage( Self::ExportFileFormat format )
{
    struct ExportInfo
    {
        const TbChar8* fileName;
        TbBool (*exportFunc)( TB_OUT TbImageExportResult& result , const TbImageExportInfo& info , const TbImageAllocFunc allocFunc );
    };
    static const ExportInfo EXPORT_INFO[] =
    {
        { "colorMap.bmp" , TbImageBmp::Export } , 
        { "colorMap.tga" , TbImageTga::Export }
    };
    TB_STATIC_ASSERT( EXPORT_FILE_FORMAT_MAX == TB_ARRAY_COUNT_OF(EXPORT_INFO) );
    TbFile texFile( EXPORT_INFO[format].fileName , TB_FILE_OPERATE_WRITE , TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
    texFile.Open();
    void* texBuffer = NULL;
    TbUint32 pitch;
    TbUint32 height;
    if( TB_S_OK == m_ColorTexture->Lock( 0 , TB_LOCK_FLAG_BIT_READ_ONLY , &texBuffer , pitch , height ) ){
        TbImageExportInfo exportInfo;
        exportInfo.buffer = texBuffer;
        exportInfo.bufSize = pitch * height;
        exportInfo.width = m_ColorTexture->GetWidth(0);
        exportInfo.height = m_ColorTexture->GetHeight(0);
        exportInfo.colorFormat = m_ColorTexture->GetFormat(0);
        exportInfo.flag = 0;
        TbImageExportResult exportResult;
        if( EXPORT_INFO[format].exportFunc( exportResult , exportInfo , NULL ) ){
            texFile.Write( exportResult.fileBuf , exportResult.fileBufSize ); 
        }
        TbImageAllocatorDefault::Free( exportResult.fileBuf );
    }
    m_ColorTexture->Unlock( 0 );
    return TB_S_OK;
}

/*!
 * 骨格用頂点バッファ取得
 * @author Miyake Shunsuke
 * @since 2011.08.15
 */
TbVertexBuffer* SkeletonViewer::GetSkeletonVertexBuffer( TbUint32* stride )
{
    if( stride ){
        *stride = sizeof(Self::VertexType);
    }
    return m_SkeletonVertexBuffer;
}

/*!
 * プレイヤーのスケルトンインデックスを更新
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
void SkeletonViewer::updatePlayerSkeletonIndex()
{
    m_PlayerSkeletonIndex = -1;
    if( m_NuiManager->GetTrackedSkeletonCount() > 0 ){
        for( TbSint32 i = 0; i < TB_NUI_SKELETON_MAX; ++i ){
            if( m_NuiManager->GetSkeleton( i )->trackedSkeleton ){
                m_PlayerSkeletonIndex = i;
                break;
            }
        }
    }
}

/*!
 * カラーテクスチャ更新
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
void SkeletonViewer::updateColorTexture()
{
    TbNuiColorFrame* colorFrame = m_NuiManager->GetColorFrame();
    TbNuiDepthFrame* depthFrame = m_NuiManager->GetDepthFrame();
    if( colorFrame && colorFrame->IsReady() ){
        const TbUint8* srcBuffer = static_cast<const TbUint8*>(colorFrame->LockBuffer());
        if( srcBuffer ){
            TbUint8* dstBuffer = NULL;
            TbUint32 dstPitch;
            TbUint32 dstHeight;
            TbUint32 color = 0;
            TbNuiColorPos colorPos = { colorFrame->GetResolutionType() , 0 , 0 };
            TbNuiDepthPos depthPos = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
            if( TB_S_OK == m_ColorTexture->Lock( 0 , TB_LOCK_FLAG_BIT_WRITE_ONLY , reinterpret_cast<void**>(&dstBuffer) , dstPitch , dstHeight ) )
            {
                TbUint32 index = 0;
                TbSint16 playerIndex = 0;
#if 0
                if( IsEnableMaskPlayerColor() && depthFrame && depthFrame->IsReady() && TB_SUCCEEDED(depthFrame->Lock()) ){
                    // マスク
                    TbMemClear( dstBuffer , dstPitch * dstHeight );
                    for( depthPos.y = 0; depthPos.y < depthFrame->GetResolution().height; ++depthPos.y ){
                        for( depthPos.x = 0; depthPos.x < depthFrame->GetResolution().width; ++depthPos.x ){
                            depthPos.depth = depthFrame->GetDepth( depthPos.x , depthPos.y , &playerIndex );
                            if( playerIndex >= 0 && TB_SUCCEEDED(m_NuiManager->TransformDepthToColor( colorPos , depthPos )) ){

                                index = colorPos.y * colorFrame->GetPitch() + colorPos.x * 4;
                                color = TB_COLOR_A8R8G8B8( 0xFF , srcBuffer[ index+2 ] , srcBuffer[ index+1 ] , srcBuffer[ index ] );
                                TbMemCopy( &dstBuffer[ ( colorPos.y * dstPitch ) + ( colorPos.x * 4 ) ] , &color , 4 );

                                colorPos.x += 1;
                                index = colorPos.y * colorFrame->GetPitch() + colorPos.x * 4;
                                color = TB_COLOR_A8R8G8B8( 0xFF , srcBuffer[ index+2 ] , srcBuffer[ index+1 ] , srcBuffer[ index ] );
                                TbMemCopy( &dstBuffer[ ( colorPos.y * dstPitch ) + ( colorPos.x * 4 ) ] , &color , 4 );

                                colorPos.y += 1;
                                index = colorPos.y * colorFrame->GetPitch() + colorPos.x * 4;
                                color = TB_COLOR_A8R8G8B8( 0xFF , srcBuffer[ index+2 ] , srcBuffer[ index+1 ] , srcBuffer[ index ] );
                                TbMemCopy( &dstBuffer[ ( colorPos.y * dstPitch ) + ( colorPos.x * 4 )] , &color , 4 );

                                colorPos.x -= 1;
                                index = colorPos.y * colorFrame->GetPitch() + colorPos.x * 4;
                                color = TB_COLOR_A8R8G8B8( 0xFF , srcBuffer[ index+2 ] , srcBuffer[ index+1 ] , srcBuffer[ index ] );
                                TbMemCopy( &dstBuffer[ ( colorPos.y * dstPitch ) + ( colorPos.x * 4 ) ] , &color , 4 );
                            }
                        }
                    }
                    depthFrame->Unlock();
                }else
#endif
                {
                    for( TbSint32 y = 0; y < colorFrame->GetResolution().height; ++y ){
                        for( TbSint32 x = 0; x < colorFrame->GetResolution().width; ++x ){
                            color = TB_COLOR_A8R8G8B8( 0xFF , srcBuffer[x*4+2] , srcBuffer[x*4+1] , srcBuffer[x*4] );
                            if( IsEnableMaskPlayerColor() )
                            {
                                colorPos.x = x;
                                colorPos.y = y;
                                TbSint16 playerIndex = -1; 
                                if(!TB_SUCCEEDED(m_NuiManager->TransformColorToDepth(depthPos,&playerIndex,colorPos))){
                                    color = 0;
                                }else if( playerIndex < 0) {
                                    color = 0;
                                }
                            }
                            TbMemCopy( &dstBuffer[x*4] , &color , 4 );
                        }
                        dstBuffer += dstPitch;
                        srcBuffer += colorFrame->GetPitch();
                    }
                }
                m_ColorTexture->Unlock(0);
            }
            colorFrame->UnlockBuffer();
        }
    }
}

/*!
 * 深度テクスチャ更新
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
void SkeletonViewer::updateDepthTexture()
{
    TbNuiColorFrame* colorFrame = m_NuiManager->GetColorFrame();
    TbNuiDepthFrame* depthFrame = m_NuiManager->GetDepthFrame();
    if( depthFrame && depthFrame->IsReady() ){
        if( TB_SUCCEEDED( depthFrame->Lock() ) ){
            TbUint8* dstBuffer = NULL;
            TbUint32 dstPitch;
            TbUint32 dstHeight;
            const TbUint32* colorBuffer = static_cast<const TbUint32*>(colorFrame->LockBuffer());
            if( TB_S_OK == m_DepthTexture->Lock( 0 , TB_LOCK_FLAG_BIT_WRITE_ONLY , reinterpret_cast<void**>(&dstBuffer) , dstPitch , dstHeight ) )
            {
                TbSint16 playerIndex = 0;
                TbFloat32 highHalfDepth = 0.0f;
                TbFloat32 lowHalfDepth = 0.0f;
                for( TbSint32 y = 0; y < depthFrame->GetResolution().height; ++y ){
                    for( TbSint32 x = 0; x < depthFrame->GetResolution().width; ++x ){
                        TbUint32 color = 0;
                        TbUint16 depth = depthFrame->GetDepth( x , y , &playerIndex );
                        TbNuiGetHalfRateFromDepth( highHalfDepth , lowHalfDepth , depth );
                        TbUint8 green = static_cast<TbUint8>( highHalfDepth * 255.0f );
                        TbUint8 blue = static_cast<TbUint8>( lowHalfDepth * 255.0f );
                        color = TB_COLOR_A8R8G8B8( 0xFF , 0x0 , green , blue );
                        // マスク
                        if( playerIndex < 0 ){
                            if( IsEnableMaskPlayerDepth() ){
                                color = TB_COLOR_A8R8G8B8( 0xFF , 0 , 0 , 0 );
                            }
                        }else{
#if 0
                            if( IsEnableMaskPlayerColor() )
                            {
                                color = TB_COLOR_A8R8G8B8( 0xFF , 0 , 0 , 0 );
                                TbNuiColorPos colorPos = { colorFrame->GetResolutionType() , 0 , 0 };
                                TbNuiDepthPos depthPos = { depthFrame->GetResolutionType() , x , y , depth };
                                if(TB_SUCCEEDED(m_NuiManager->TransformDepthToColor(colorPos,depthPos)) ){
                                    if( 0 <= colorPos.x && colorPos.x < colorFrame->GetResolution().width &&
                                        0 <= colorPos.y && colorPos.y < colorFrame->GetResolution().height ) 
                                    {
                                        if( 1080 < colorPos.y ){
                                            int b = 0;
                                        }
                                        color = colorBuffer[colorPos.y*colorFrame->GetResolution().width+colorPos.x];
                                    }
                                }
                                color = TB_COLOR_A8R8G8B8( 0xff , 0x99 , 0x00 , 0xCC );
                            }
#endif
                        }
                        TbMemCopy( &dstBuffer[x*4] , &color , 4 );
                    }
                    dstBuffer += dstPitch;
                }
                m_DepthTexture->Unlock(0);
            }
            depthFrame->Unlock();
        }
    }
}

/*!
 * 骨格の頂点バッファ更新
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
void SkeletonViewer::updateSkeletonVertexBuffer()
{
    writePlayerSkeletonPosToVertexBuffer( 0 , TB_NUI_SKELETON_POS_HEAD , TB_NUI_SKELETON_POS_SHOULDER_C );
    writePlayerSkeletonPosToVertexBuffer( 1 , TB_NUI_SKELETON_POS_SHOULDER_C , TB_NUI_SKELETON_POS_SHOULDER_L );
    writePlayerSkeletonPosToVertexBuffer( 2 , TB_NUI_SKELETON_POS_SHOULDER_L , TB_NUI_SKELETON_POS_ELBOW_L );
    writePlayerSkeletonPosToVertexBuffer( 3 , TB_NUI_SKELETON_POS_ELBOW_L , TB_NUI_SKELETON_POS_WRIST_L );
    writePlayerSkeletonPosToVertexBuffer( 4 , TB_NUI_SKELETON_POS_WRIST_L , TB_NUI_SKELETON_POS_HAND_L );
    writePlayerSkeletonPosToVertexBuffer( 5 , TB_NUI_SKELETON_POS_SHOULDER_C , TB_NUI_SKELETON_POS_SHOULDER_R );
    writePlayerSkeletonPosToVertexBuffer( 6 , TB_NUI_SKELETON_POS_SHOULDER_R , TB_NUI_SKELETON_POS_ELBOW_R );
    writePlayerSkeletonPosToVertexBuffer( 7 , TB_NUI_SKELETON_POS_ELBOW_R , TB_NUI_SKELETON_POS_WRIST_R );
    writePlayerSkeletonPosToVertexBuffer( 8 , TB_NUI_SKELETON_POS_WRIST_R , TB_NUI_SKELETON_POS_HAND_R );
    writePlayerSkeletonPosToVertexBuffer( 9 , TB_NUI_SKELETON_POS_SHOULDER_C , TB_NUI_SKELETON_POS_SPINE );
    writePlayerSkeletonPosToVertexBuffer( 10 , TB_NUI_SKELETON_POS_SPINE , TB_NUI_SKELETON_POS_HIP_C );
    writePlayerSkeletonPosToVertexBuffer( 11 , TB_NUI_SKELETON_POS_HIP_C , TB_NUI_SKELETON_POS_HIP_L );
    writePlayerSkeletonPosToVertexBuffer( 12 , TB_NUI_SKELETON_POS_HIP_L , TB_NUI_SKELETON_POS_KNEE_L );
    writePlayerSkeletonPosToVertexBuffer( 13 , TB_NUI_SKELETON_POS_KNEE_L , TB_NUI_SKELETON_POS_ANKLE_L );
    writePlayerSkeletonPosToVertexBuffer( 14 , TB_NUI_SKELETON_POS_ANKLE_L , TB_NUI_SKELETON_POS_FOOT_L );
    writePlayerSkeletonPosToVertexBuffer( 15 , TB_NUI_SKELETON_POS_HIP_C , TB_NUI_SKELETON_POS_HIP_R );
    writePlayerSkeletonPosToVertexBuffer( 16 , TB_NUI_SKELETON_POS_HIP_R , TB_NUI_SKELETON_POS_KNEE_R );
    writePlayerSkeletonPosToVertexBuffer( 17 , TB_NUI_SKELETON_POS_KNEE_R , TB_NUI_SKELETON_POS_ANKLE_R );
    writePlayerSkeletonPosToVertexBuffer( 18 , TB_NUI_SKELETON_POS_ANKLE_R , TB_NUI_SKELETON_POS_FOOT_R );
    m_SkeletonVertexBuffer->Write(sizeof(m_SkeletonVertex),m_SkeletonVertex);
}

/*!
 * 骨格位置座標を頂点バッファに書き込み
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */
void SkeletonViewer::writePlayerSkeletonPosToVertexBuffer( TbSint32 index , TbNuiSkeletonPos pos1 , TbNuiSkeletonPos pos2 )
{
    const TbNuiSkeleton* skeleton = m_NuiManager->GetSkeleton( m_PlayerSkeletonIndex );
    if( skeleton && skeleton->trackedSkeleton ){
        TbNuiDepthPos depthPos;
        depthPos.resolutionType = m_NuiManager->GetDepthFrame()->GetResolutionType();
        {
            TbVector4& pos = skeleton->trackedSkeleton->skeletonPos[ pos1 ];
            m_NuiManager->TransformSkeletonToDepth( depthPos , pos );
            m_SkeletonVertex[index*2].x = static_cast<TbFloat32>(depthPos.x) / 320.0f - 0.5f;
            m_SkeletonVertex[index*2].y = -static_cast<TbFloat32>(depthPos.y) / 240.0f + 0.5f;
        }
        {
            TbVector4& pos = skeleton->trackedSkeleton->skeletonPos[ pos2 ];
            m_NuiManager->TransformSkeletonToDepth( depthPos , pos );
            m_SkeletonVertex[index*2+1].x = ( static_cast<TbFloat32>(depthPos.x) / 320.0f ) - 0.5f;
            m_SkeletonVertex[index*2+1].y = ( -static_cast<TbFloat32>(depthPos.y) / 240.0f ) + 0.5f;
        }
    }
}
