/*!
 * NUI�X�P���g���Ǘ��N���X
 * @author teco
 */

#include <tb_fwd.h>
#include <nui/tb_nui_simple_manager.h>

#include <base/algorithm/tb_sort.h>
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/image/tb_image_bmp.h>
#include <graphics/image/tb_image_tga.h>
#include <base/io/tb_file.h>
#include <base/io/tb_file_manager.h>
#include <input/tb_keyboard.h>
#include <nui/tb_nui.h>
#include <framework/tb_framework.h>
#include <graphics/font/tb_font.h>
#include <framework/debug/menu/tb_debug_menu_manager.h>

namespace toybox
{

static TbSint32 MASK_CORRECT_LEFT_UNIT_X = 2;
static TbSint32 MASK_CORRECT_LEFT_UNIT_Y = 6;
static TbSint32 MASK_CORRECT_LEFT_LOOP_X = 2;
static TbSint32 MASK_CORRECT_LEFT_LOOP_Y = 2;
static TbSint32 MASK_CORRECT_RIGHT_UNIT_X = 4;
static TbSint32 MASK_CORRECT_RIGHT_UNIT_Y = 6;
static TbSint32 MASK_CORRECT_RIGHT_LOOP_X = 2;
static TbSint32 MASK_CORRECT_RIGHT_LOOP_Y = 2;

static TbSint32 MASK_CORRECT_DEPTH_UNIT_X = 1;
static TbSint32 MASK_CORRECT_DEPTH_UNIT_Y = 6;
static TbSint32 MASK_CORRECT_DEPTH_LOOP_X = 1;
static TbSint32 MASK_CORRECT_DEPTH_LOOP_Y = 1;

/*! 
 * �R���X�g���N�^
 * @author teco
 */
TbNuiSimpleManager::TbNuiSimpleManager( TbUint32 playerMax )
    : m_BitArray()
    , m_NuiManager( NULL )
    , m_PlayerSkeletonCount( 0 )
    , m_PlayerSkeletonMax( playerMax )
    , m_PlayerSkeletonIndexBit(0)
    , m_ColorTextureFrontIndex(0)
    , m_DepthTexture( NULL )
    , m_Bright(50)
{
    m_PlayerMaskCorrectParam.leftUnit.x = MASK_CORRECT_LEFT_UNIT_X;
    m_PlayerMaskCorrectParam.leftUnit.y = MASK_CORRECT_LEFT_UNIT_Y;
    m_PlayerMaskCorrectParam.leftLoop.x = MASK_CORRECT_LEFT_LOOP_X;
    m_PlayerMaskCorrectParam.leftLoop.y = MASK_CORRECT_LEFT_LOOP_Y;
    m_PlayerMaskCorrectParam.rightUnit.x = MASK_CORRECT_RIGHT_UNIT_X;
    m_PlayerMaskCorrectParam.rightUnit.y = MASK_CORRECT_RIGHT_UNIT_Y;
    m_PlayerMaskCorrectParam.rightLoop.x = MASK_CORRECT_RIGHT_LOOP_X;
    m_PlayerMaskCorrectParam.rightLoop.y = MASK_CORRECT_RIGHT_LOOP_Y;
    m_PlayerMaskCorrectParam.depthUnit.x = MASK_CORRECT_DEPTH_UNIT_X;
    m_PlayerMaskCorrectParam.depthUnit.y = MASK_CORRECT_DEPTH_UNIT_Y;
    m_PlayerMaskCorrectParam.depthLoop.x = MASK_CORRECT_DEPTH_LOOP_X;
    m_PlayerMaskCorrectParam.depthLoop.y = MASK_CORRECT_DEPTH_LOOP_Y;

    SetEnableUpdateSkeletonVertexBuffer(TB_TRUE);
    SetEnableUpdateColorTexture(TB_TRUE);
    SetEnableUpdateDepthTexture(TB_TRUE);
    m_DepthMaskColor = TbColorU32::White();

    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PlayerInfo); ++i )
    {
        m_PlayerInfo[i].skeletonIndex = -1;
        m_PlayerInfo[i].isEnableMask = TB_TRUE;
        for( TbSint32 j = 0; j < TB_ARRAY_COUNT_OF(m_PlayerInfo[i].colorSkeleton); ++j ){
            m_PlayerInfo[i].colorSkeleton[j].SetW(1.0f);
        }
    }

    // NuiManager
    {
        TbNuiCreateParam nuiCreateParam;
        nuiCreateParam.flagBit = TB_NUI_INITIALIZE_BIT_USE_COLOR | TB_NUI_INITIALIZE_BIT_USE_DEPTH | TB_NUI_INITIALIZE_BIT_USE_SKELETON;
        m_NuiManager = new TbNuiManager( nuiCreateParam );
    }

    // ���_�t�H�[�}�b�g�A���_�o�b�t�@�쐬
    {
        for( TbUint32 playerIndex = 0; playerIndex < TB_NUI_TRACKED_SKELETON_MAX; ++playerIndex )
        {
            m_PlayerInfo[playerIndex].skeletonVertexBuffer = new TbVertexBuffer( &TbVertexFormat::XYZ_COLOR ,  sizeof(m_PlayerInfo[playerIndex].skeletonVertex));
            for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PlayerInfo[i].skeletonVertex); ++i ){
                TbVertexXYZColor& vertex = m_PlayerInfo[playerIndex].skeletonVertex[i];
                vertex.x = 0.0f;
                vertex.y = 0.0f;
                vertex.z = 0.01f;
                vertex.color = TB_COLOR_A8R8G8B8( 0xFF , 0xFF , 0xFF , 0xFF );
            }
            m_PlayerInfo[playerIndex].skeletonVertexBuffer->Write(
                sizeof(m_PlayerInfo[playerIndex].skeletonVertex),
                m_PlayerInfo[playerIndex].skeletonVertex);
        }
    }

}

/*! 
 * �f�X�g���N�^
 * @author teco
 */
TbNuiSimpleManager::~TbNuiSimpleManager()
{
    for( TbUint32 playerIndex = 0; playerIndex < TB_NUI_TRACKED_SKELETON_MAX; ++playerIndex ) {
        TB_SAFE_DELETE(m_PlayerInfo[playerIndex].skeletonVertexBuffer);
    }
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ColorTexture); ++i ) {
        TB_SAFE_DELETE( m_ColorTexture[i] );
    }
    TB_SAFE_DELETE( m_DepthTexture );
    TB_SAFE_DELETE( m_NuiManager );
}

/*! 
 * �X�V
 * @author teco
 */
void TbNuiSimpleManager::Update()
{
    m_NuiManager->Update();
    if( !m_BitArray.Test( FLAG_READY ) ){
        if( m_NuiManager->IsReady() ){
            // �J���[�e�N�X�`������
            {
                TbTextureDesc texDesc;
                TbDimensionS32 size = TbTexture::GetTextureSize(m_NuiManager->GetColorFrame()->GetResolution(),TB_FALSE);
                texDesc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
                texDesc.flags = 0;
                texDesc.width = size.width;
                texDesc.height = size.height;
                texDesc.levels = 1;
                for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ColorTexture); ++i ) {
                    m_ColorTexture[i] = new TbTexture( texDesc );
                }
                m_ColorUV.width = static_cast<TbFloat32>(m_NuiManager->GetColorFrame()->GetResolution().width)/static_cast<TbFloat32>(size.width);
                m_ColorUV.height = static_cast<TbFloat32>(m_NuiManager->GetColorFrame()->GetResolution().height)/static_cast<TbFloat32>(size.height);
            }

            // �[�x�e�N�X�`��
            {
                TbTextureDesc texDesc;
                TbDimensionS32 size = TbTexture::GetTextureSize(m_NuiManager->GetDepthFrame()->GetResolution(),TB_FALSE);
                texDesc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
                texDesc.flags = 0;
                texDesc.width = size.width;
                texDesc.height = size.height;
                texDesc.levels = 1;
                m_DepthTexture = new TbTexture( texDesc );
                m_DepthUV.width = static_cast<TbFloat32>(m_NuiManager->GetDepthFrame()->GetResolution().width)/static_cast<TbFloat32>(size.width);
                m_DepthUV.height = static_cast<TbFloat32>(m_NuiManager->GetDepthFrame()->GetResolution().height)/static_cast<TbFloat32>(size.height);
            }

            m_BitArray.SetBit( FLAG_READY , TB_TRUE );
        }
    }else{

        if( !(TbDebugMenuManager::GetInstance() && TbDebugMenuManager::GetInstance()->IsVisible()) )
        {
            // �J�����̌X���ύX
            if( TbKeyboard::IsOn( TB_KEY_CONTROL_L ) ){
                LONG angle = 0;
                if( TbKeyboard::IsPress( TB_KEY_DOWN ) ){
                    m_NuiManager->AddTiltMotorAngleDeg( 1 );
                }
                if( TbKeyboard::IsPress( TB_KEY_UP ) ){
                    m_NuiManager->AddTiltMotorAngleDeg( - 1 );
                }
            }
        }

        // �g���b�L���O���̍X�V
        updatePlayerSkeletonIndex();
        updateColorTexture();
        updateDepthTexture();
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_PlayerInfo); ++i ) {
            updateColorSkeleton(i);
        }
    }
    m_NuiManager->GetDepend().ResetFrame();
}

/*!
 * �J���[�p�e�N�X�`���t�@�C���o��
 * @author teco
 */
TbResult TbNuiSimpleManager::ExportColorImage( Self::ExportFileFormat format )
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
    TbTexture* colorTexture = m_ColorTexture[m_ColorTextureFrontIndex];    
    TbFile texFile( EXPORT_INFO[format].fileName , TB_FILE_OPERATE_WRITE, TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
    texFile.Open();
    void* texBuffer = NULL;
    TbUint32 pitch;
    TbUint32 height;
    if( TB_S_OK == colorTexture->Lock( 0 , TB_LOCK_FLAG_BIT_READ_ONLY , &texBuffer , pitch , height ) ){
        TbImageExportInfo exportInfo;
        exportInfo.buffer = texBuffer;
        exportInfo.bufSize = pitch * height;
        exportInfo.width = colorTexture->GetWidth(0);
        exportInfo.height = colorTexture->GetHeight(0);
        exportInfo.colorFormat = colorTexture->GetFormat(0);
        exportInfo.flag = 0;
        TbImageExportResult exportResult;
        if( EXPORT_INFO[format].exportFunc( exportResult , exportInfo , NULL ) ){
            texFile.Write( exportResult.fileBuf , exportResult.fileBufSize ); 
        }
        TbImageAllocatorDefault::Free( exportResult.fileBuf );
    }
    colorTexture->Unlock( 0 );
    return TB_S_OK;
}

/*!
 * ���i�p���_�o�b�t�@�擾
 * @author teco
 */
TbVertexBuffer* TbNuiSimpleManager::GetSkeletonVertexBuffer( TbUint32 playerIndex , TbUint32* stride )
{
    if( playerIndex < TB_NUI_TRACKED_SKELETON_MAX ) {
        if( stride ){
            *stride = sizeof(TbVertexXYZColor);
        }
        return m_PlayerInfo[playerIndex].skeletonVertexBuffer;
    }
    return NULL;
}

/*!
 * �ő�v���C���[�X�P���g�����ݒ�
 * @author teco
 */
void TbNuiSimpleManager::SetPlayerSkeletonMax( TbUint32 max )
{
    m_PlayerSkeletonMax = TbClamp<TbUint32>( max , 1 , TB_NUI_TRACKED_SKELETON_MAX );
}

/*!
 * �v���C���[�X�P���g���C���f�b�N�X�擾
 * @author teco
 */
TbSint32 TbNuiSimpleManager::GetPlayerSkeletonIndex( TbUint32 playerIndex ) const
{
    if( playerIndex < m_PlayerSkeletonCount){
        return m_PlayerInfo[playerIndex].skeletonIndex;
    }
    return -1;
}

/*!
 * �J���[�o�b�t�@�Ƀ}�b�s���O���ꂽ���i���W�擾
 * @author teco
 */
const toybox::TbVector4* TbNuiSimpleManager::GetPlayerSkeletonColorPos( TbUint32 playerIndex ) const
{
    if( playerIndex < m_PlayerSkeletonCount ){
        return m_PlayerInfo[playerIndex].colorSkeleton;
    }
    return NULL;
}

/*!
 * �v���C���[�ʃ}�X�N�\�ݒ�
 * @author teco
 */
void TbNuiSimpleManager::SetEnablePlayerMask( TbUint32 playerIndex , TbBool isEnable )
{
    if( playerIndex < m_PlayerSkeletonCount ){
        m_PlayerInfo[playerIndex].isEnableMask = isEnable;
    }
}

/*!
 * �v���C���[�ʃ}�X�N�\�ݒ�
 * @author teco
 */
TbBool TbNuiSimpleManager::IsEnablePlayerMask( TbUint32 playerIndex )
{
    if( playerIndex < m_PlayerSkeletonCount ){
        return m_PlayerInfo[playerIndex].isEnableMask;
    }    
    return TB_FALSE;
}


/*!
 * �v���C���[�̃X�P���g���C���f�b�N�X���X�V
 * @author teco
 */
void TbNuiSimpleManager::updatePlayerSkeletonIndex()
{
    //���ҏ��
    struct CandidateInfo
    {
        static TbBool compareCandidateDistance( const CandidateInfo& left , const CandidateInfo& right )
        {
            return left.distanceFromSweetPoint < right.distanceFromSweetPoint;
        }
        static TbBool compareCandidateX( const CandidateInfo& left , const CandidateInfo& right )
        {
            return left.posXFromSweetPoint > right.posXFromSweetPoint;
        }
        TbFloat32 distanceFromSweetPoint;
        TbFloat32 posXFromSweetPoint;
        TbSint32 index;
    };

    static TbVector4 SWEET_POINT( 0.0f , 0.0f , 2.0f , 1.0f );
    m_PlayerSkeletonCount = 0;
    m_PlayerSkeletonIndexBit = 0;
    for( TbSint32 i = 0; i < TB_NUI_TRACKED_SKELETON_MAX; ++i ){
        m_PlayerInfo[i].skeletonIndex = -1;
    }

    CandidateInfo candidate[TB_NUI_SKELETON_MAX];
    TbMemClear( candidate , sizeof(candidate) );
    if( m_NuiManager->GetTrackedSkeletonCount() > 0 ){
        for( TbSint32 i = 0; i < TB_NUI_SKELETON_MAX; ++i ){
            // ��x�C���f�b�N�X��������
            TbNuiTrackedSkeleton* trackedSkeleton = m_NuiManager->GetSkeleton( i )->trackedSkeleton;
            if( trackedSkeleton ){
                candidate[m_PlayerSkeletonCount].index = i;
                candidate[m_PlayerSkeletonCount].distanceFromSweetPoint = (trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_HIP_C] - SWEET_POINT).Length();
                candidate[m_PlayerSkeletonCount].posXFromSweetPoint = (trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_HIP_C].GetX() - SWEET_POINT.GetX());
                ++m_PlayerSkeletonCount;
            }
        }
    }
    // �\�[�g���Ċi�[
    if( m_PlayerSkeletonMax == 1 ){
        TbInsertionSort( &candidate[0] , &candidate[m_PlayerSkeletonCount] , CandidateInfo::compareCandidateDistance );
    }else{
        // �����l����ꍇ�͍�����
        TbInsertionSort( &candidate[0] , &candidate[m_PlayerSkeletonCount] , CandidateInfo::compareCandidateX );
    }

    m_PlayerSkeletonCount = TbMin( m_PlayerSkeletonMax , m_PlayerSkeletonCount );
    for( TbUint32 i = 0; i < m_PlayerSkeletonCount; ++i ){
        if( m_PlayerInfo[i].isEnableMask ){
            m_PlayerSkeletonIndexBit |= TB_BIT(candidate[i].index);
        }
        m_PlayerInfo[i].skeletonIndex = candidate[i].index;
    }

}

/*!
 * �J���[�e�N�X�`���X�V
 * @author teco
 * @since 2011.08.14
 */
void TbNuiSimpleManager::updateColorTexture()
{
    if( !IsEnableUpdateColorTexture() ){
        return;
    }
    TbNuiColorFrame* colorFrame = m_NuiManager->GetColorFrame();
    TbNuiDepthFrame* depthFrame = m_NuiManager->GetDepthFrame();

    m_ColorTextureFrontIndex = (m_ColorTextureFrontIndex+1) % TB_ARRAY_COUNT_OF(m_ColorTexture);
    TbTexture* texture = m_ColorTexture[m_ColorTextureFrontIndex];

    if( colorFrame && colorFrame->IsReady() ){
        if( TB_SUCCEEDED(colorFrame->Lock()) ){
            if( IsEnableColorPlayerMask() )
            {
                if( TB_FAILED(depthFrame->Lock())) {
                    colorFrame->Unlock();
                    return;
                }
            }
            TbUint8* dstBuffer = NULL;
            TbUint32 dstPitch;
            TbUint32 dstHeight;
            TbUint32 color = 0;
            if( TB_S_OK == texture->Lock( 0 , TB_LOCK_FLAG_BIT_WRITE_ONLY , reinterpret_cast<void**>(&dstBuffer) , dstPitch , dstHeight ) )
            {
                TbUint8* dstP = dstBuffer;
                TbNuiColorPos colorPos = { colorFrame->GetResolutionType() , 0 , 0 };
                TbNuiDepthPos depthPos = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
                for( TbSint32 y = 0; y < colorFrame->GetResolution().height; ++y ){
                    for( TbSint32 x = 0; x < colorFrame->GetResolution().width; ++x ){
                        color = colorFrame->GetColor(x,y);
                        if( IsEnableColorPlayerMask() )
                        {
                            colorPos.x = x;
                            colorPos.y = y;
                            TbSint16 skeletonIndex = -1; 
                            if( TB_FAILED(m_NuiManager->TransformColorToDepth(depthPos,&skeletonIndex,colorPos)) ){
#if TB_IS_DEBUG
                                if( IsEnableDebugUnmapRegion() ){
                                    color = TB_COLOR_A8R8G8B8(0xFF,0,0xFF,0xFF);
                                }else
#endif
                                {
                                    color = 0;
                                }
                            }else if( !isEnableTrackingPlayerSkeleotn(skeletonIndex) ) {
                                color = 0;
                            }else{
#if 0
                                switch( skeletonIndex )
                                {
                                case 0:
                                    color = TB_COLOR_A8R8G8B8(0xFF,0xFF,0,0);
                                    break;
                                case 1:
                                    color = TB_COLOR_A8R8G8B8(0xFF,0,0xFF,0);
                                    break;
                                case 2:
                                    color = TB_COLOR_A8R8G8B8(0xFF,0,0,0xFF);
                                    break;
                                case 3:
                                    color = TB_COLOR_A8R8G8B8(0xFF,0xFF,0,0xFF);
                                    break;
                                case 4:
                                    color = TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0);
                                    break;
                                case 5:
                                    color = TB_COLOR_A8R8G8B8(0xFF,0,0xFF,0xFF);
                                    break;
                                }
#endif
                            }
                        }
                        *reinterpret_cast<TbUint32*>(&dstP[x*4]) = color;
                    }
                    dstP += dstPitch;
                }

#if 1
                // ����␳
                if( IsEnableColorPlayerMask() && IsEnableMaskCorrect() )
                {
                    TbNuiColorPos colorPos = { colorFrame->GetResolutionType() , 0 , 0 };
                    TbNuiDepthPos depthPos = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
                    TbNuiDepthPos preDepthPos;
                    TbNuiDepthPos firstDepthPos = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
                    TbNuiDepthPos baseDepthPos = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
                    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_PlayerInfo); ++i ) {
                        const TbNuiSkeleton* skeleton = m_NuiManager->GetSkeleton( m_PlayerInfo[i].skeletonIndex );
                        if( skeleton && skeleton->trackedSkeleton ){
                            TbPointS32 headPoint( TbLerp(m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_HEAD].GetX(),
                                                         m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_SHOULDER_C].GetX(),
                                                         m_PlayerMaskCorrectParam.beginRatio) , 
                                                  TbLerp(m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_HEAD].GetY(),
                                                         m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_SHOULDER_C].GetY(),
                                                         m_PlayerMaskCorrectParam.beginRatio)
                                                );
                            TbPointS32 checkYPoint( TbLerp(m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_HEAD].GetX(),
                                                         m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_SHOULDER_C].GetX(),
                                                         m_PlayerMaskCorrectParam.checkYBeginRatio) , 
                                                    TbLerp(m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_HEAD].GetY(),
                                                         m_PlayerInfo[i].colorSkeleton[TB_NUI_SKELETON_POS_SHOULDER_C].GetY(),
                                                         m_PlayerMaskCorrectParam.checkYBeginRatio));
                            TbVector4 headPos = TbLerp(skeleton->trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_HEAD],
                                                       skeleton->trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_SHOULDER_C],
                                                       m_PlayerMaskCorrectParam.beginRatio);
                            m_NuiManager->TransformSkeletonToDepth(firstDepthPos,headPos);
                            baseDepthPos = firstDepthPos;
                            for( TbSint32 y = headPoint.y; 0 <= y; --y ){
                                TbBool isUpdateX = TB_FALSE;
                                depthPos = firstDepthPos;
                                TbFloat32 errorOffset = 0.0f;
                                // ��
                                for( TbSint32 x = headPoint.x; 0 <= x; --x ){
                                    colorPos.x = x;
                                    colorPos.y = y;
                                    preDepthPos = depthPos;
                                    TbSint16 skeletonIndex = -1;
                                    if( TB_FAILED(m_NuiManager->TransformColorToDepth(depthPos,&skeletonIndex,colorPos)) ){
                                        depthPos = preDepthPos;
                                        errorOffset += m_PlayerMaskCorrectParam.errorOffset;
                                        if( 1.0f <= errorOffset ){
                                            depthPos.x -= 1;
                                            errorOffset -= 1.0f;
                                        }
                                        if( isEnableDrawHeadPointByError(preDepthPos,TB_TRUE,checkYPoint.y < y) ){
#if TB_IS_DEBUG
                                            if( IsEnableDebugMaskCorrect() ){
                                                color = TB_COLOR_A8R8G8B8(0xFF,0xFF,0,0);
                                            }else
#endif
                                            {
                                                color = colorFrame->GetColor(x,y);;
                                            }
                                            *reinterpret_cast<TbUint32*>(&dstBuffer[ y*dstPitch + x*4 ]) = color;
                                            isUpdateX = TB_TRUE;
                                        }else{
                                            break;
                                        }
                                    }else if( !isEnableTrackingPlayerSkeleotn(skeletonIndex) ) {
                                        if( isEnableDrawHeadPointByError(depthPos,TB_TRUE,checkYPoint.y < y) ){
#if TB_IS_DEBUG
                                            if( IsEnableDebugMaskCorrect() ){
                                                color = TB_COLOR_A8R8G8B8(0xFF,0xFF,0,0);
                                            }else
#endif
                                            {
                                                color = colorFrame->GetColor(x,y);;
                                            }
                                            *reinterpret_cast<TbUint32*>(&dstBuffer[ y*dstPitch + x*4 ]) = color;
                                            isUpdateX = TB_TRUE;
                                        }else{
                                            break;
                                        }
                                        errorOffset = 0.0f;
                                    }else{
                                        isUpdateX = TB_TRUE;
                                        firstDepthPos.y = depthPos.y;
                                        errorOffset = 0.0f;
                                    }
                                }
                                // �E
                                depthPos = firstDepthPos;
                                for( TbSint32 x = headPoint.x; x < colorFrame->GetResolution().width; ++x ){
                                    colorPos.x = x;
                                    colorPos.y = y;
                                    TbSint16 skeletonIndex = -1; 
                                    preDepthPos = depthPos;
                                    if( TB_FAILED(m_NuiManager->TransformColorToDepth(depthPos,&skeletonIndex,colorPos)) ){
                                        depthPos = preDepthPos;
                                        depthPos.x += 1;
                                        if( isEnableDrawHeadPointByError(preDepthPos,TB_FALSE,checkYPoint.y < y) ){
#if TB_IS_DEBUG
                                            if( IsEnableDebugMaskCorrect() ){
                                                color = TB_COLOR_A8R8G8B8(0xFF,0,0xFF,0);
                                            }else
#endif
                                            {
                                                color = colorFrame->GetColor(x,y);
                                            }
                                            *reinterpret_cast<TbUint32*>(&dstBuffer[ y*dstPitch + x*4 ]) = color;
                                            isUpdateX = TB_TRUE;
                                        }else{

                                            break;
                                        }
                                    }else if( !isEnableTrackingPlayerSkeleotn(skeletonIndex) ) {
                                        if( isEnableDrawHeadPointByError(depthPos,TB_FALSE,checkYPoint.y < y) ){
#if TB_IS_DEBUG
                                            if( IsEnableDebugMaskCorrect() ){
                                                color = TB_COLOR_A8R8G8B8(0xFF,0,0xFF,0);
                                            }else
#endif
                                            {
                                                color = colorFrame->GetColor(x,y);;
                                            }
                                            *reinterpret_cast<TbUint32*>(&dstBuffer[ y*dstPitch + x*4 ]) = color;
                                            isUpdateX = TB_TRUE;
                                        }else{
                                            break;
                                        }
                                    }else{
                                        isUpdateX = TB_TRUE;
                                        firstDepthPos.y = depthPos.y;
                                    }
                                }
                                if( !isUpdateX ){
                                    break;
                                }
                                --firstDepthPos.y;
                            }
                        }
                    }
                }
#endif

                texture->Unlock(0);
            }
        }
        colorFrame->Unlock();
        depthFrame->Unlock();
    }
}

/*!
 * ���̈ʒu�`��\��
 * @author teco
 */
TbBool TbNuiSimpleManager::isEnableDrawHeadPointByError( const TbNuiDepthPos& depthPos , TbBool isLeft , TbBool isEnableIgnoreY )
{
    TbSint16 skeletonIndex = -1; 
    TbNuiDepthFrame* depthFrame = m_NuiManager->GetDepthFrame();
    TbSint32 beginY = 1;
    TbSint32 endY = isLeft ? m_PlayerMaskCorrectParam.leftLoop.y : m_PlayerMaskCorrectParam.rightLoop.y;
    if( isEnableIgnoreY ){
        beginY = 0;
        endY = 0;
    }
    if( isLeft ){
        for( TbSint32 ofsY = beginY; ofsY <= endY; ++ofsY ) 
        {
            for( TbSint32 ofsX = 1; ofsX <= m_PlayerMaskCorrectParam.leftLoop.x; ++ofsX ) 
            {
                depthFrame->GetDepth( depthPos.x+ofsX*m_PlayerMaskCorrectParam.leftUnit.x,
                                      depthPos.y+ofsY*m_PlayerMaskCorrectParam.leftUnit.y,
                                      &skeletonIndex);
                if( isEnableTrackingPlayerSkeleotn(skeletonIndex) ){
                    return TB_TRUE;
                }
            }
        }
    }else{
        for( TbSint32 ofsY = beginY; ofsY <= endY; ++ofsY ) 
        {
            for( TbSint32 ofsX = 1; ofsX <= m_PlayerMaskCorrectParam.rightLoop.x; ++ofsX ) 
            {
                depthFrame->GetDepth(depthPos.x-ofsX*m_PlayerMaskCorrectParam.rightUnit.x,
                                     depthPos.y+ofsY*m_PlayerMaskCorrectParam.rightUnit.y,
                                     &skeletonIndex);
                if( isEnableTrackingPlayerSkeleotn(skeletonIndex) ){
                    return TB_TRUE;
                }
            }
        }
    }
    return TB_FALSE;
}

/*!
 * ���̈ʒu�`��\��
 * @author teco
 */
TbBool TbNuiSimpleManager::isEnableDrawDepthHeadPointByError( const TbNuiDepthPos& depthPos , TbBool isLeft , TbBool isEnableIgnoreY )
{
    TbSint16 skeletonIndex = -1; 
    TbNuiDepthFrame* depthFrame = m_NuiManager->GetDepthFrame();
    TbSint32 beginY = 1;
    TbSint32 endY = m_PlayerMaskCorrectParam.depthLoop.y;
    TbSint32 unit = isLeft ? 1 : -1;
    if( isEnableIgnoreY ){
        beginY = 0;
        endY = 0;
    }
    for( TbSint32 ofsY = beginY; ofsY <= endY; ++ofsY ) 
    {
        for( TbSint32 ofsX = 1; ofsX <= m_PlayerMaskCorrectParam.depthLoop.x; ++ofsX ) 
        {
            depthFrame->GetDepth(depthPos.x+unit*ofsX*m_PlayerMaskCorrectParam.depthUnit.x,
                                    depthPos.y+ofsY*m_PlayerMaskCorrectParam.depthUnit.y,
                                    &skeletonIndex);
            if( isEnableTrackingPlayerSkeleotn(skeletonIndex) ){
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}


/*!
 * �[�x�e�N�X�`���X�V
 * @author teco
 * @since 2011.08.14
 */
void TbNuiSimpleManager::updateDepthTexture()
{
    if( !IsEnableUpdateDepthTexture() ){
        return;
    }
    TbNuiDepthFrame* depthFrame = m_NuiManager->GetDepthFrame();
    if( depthFrame && depthFrame->IsReady() ){
        if( TB_SUCCEEDED( depthFrame->Lock() ) ){
            TbUint8* dstBuffer = NULL;
            TbUint8* dstBufferP = NULL;
            TbUint32 dstPitch;
            TbUint32 dstHeight;
            if( TB_S_OK == m_DepthTexture->Lock( 0 , TB_LOCK_FLAG_BIT_WRITE_ONLY , reinterpret_cast<void**>(&dstBufferP) , dstPitch , dstHeight ) )
            {
                dstBuffer = dstBufferP;
                TbSint16 skeletonIndex = 0;
                TbFloat32 highHalfDepth = 0.0f;
                TbFloat32 lowHalfDepth = 0.0f;
                for( TbSint32 y = 0; y < depthFrame->GetResolution().height; ++y ){
                    for( TbSint32 x = 0; x < depthFrame->GetResolution().width; ++x ){
                            skeletonIndex = -1;
                            TbUint16 depth = depthFrame->GetDepth( x , y , &skeletonIndex );
                            TbNuiGetHalfRateFromDepth( highHalfDepth , lowHalfDepth , depth );
                            TbColorU32 color = TbColorU32::Black();
                            color.SetBlueU8( static_cast<TbUint8>( lowHalfDepth * 255.0f ) );
                            color.SetGreenU8( static_cast<TbUint8>( lowHalfDepth * 255.0f ) );
                            // �}�X�N
                            if( IsEnableDepthPlayerMask() )
                            {
                                if( IsEnableDepthMaskColor() ){
                                    color = m_DepthMaskColor;
                                }
                                if( !isEnableTrackingPlayerSkeleotn(skeletonIndex) ){
                                    color.SetColorU8(0,0,0,0);
                                }
                            }
                            TbMemCopy( &dstBuffer[x*4] , &color.value , 4 );
                    }
                    dstBuffer += dstPitch;
                }
                
#if 1
                // ����␳
                if( IsEnableDepthPlayerMask() && IsEnableMaskCorrect() && IsEnableDepthMaskColor() )
                {
                    dstBuffer = dstBufferP;
                    TbUint32 color = 0;
                    TbNuiDepthPos depthPos = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
                    TbNuiDepthPos preDepthPos;

                    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_PlayerInfo); ++i ) {
                        const TbNuiSkeleton* skeleton = m_NuiManager->GetSkeleton( m_PlayerInfo[i].skeletonIndex );
                        if( skeleton && skeleton->trackedSkeleton ){
                            TbVector4 headPos = TbLerp(skeleton->trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_HEAD],
                                                       skeleton->trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_SHOULDER_C],
                                                       m_PlayerMaskCorrectParam.beginRatio);
                            TbVector4 checkYBeginHeadPos = TbLerp(skeleton->trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_HEAD],
                                                       skeleton->trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_SHOULDER_C],
                                                       m_PlayerMaskCorrectParam.checkYBeginRatio);
                            TbNuiDepthPos headPoint = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
                            TbNuiDepthPos checkYHeadPoint = { depthFrame->GetResolutionType() , 0 , 0 , 0 };
                            m_NuiManager->TransformSkeletonToDepth(headPoint,headPos);
                            m_NuiManager->TransformSkeletonToDepth(checkYHeadPoint,checkYBeginHeadPos);

                            TbSint16 skeletonIndex = -1; 
                            for( TbSint32 y = headPoint.y; 0 <= y; --y ){
                                depthPos.y = y;
                                TbBool isUpdateX = TB_FALSE;
                                // ��
                                for( TbSint32 x = headPoint.x; 0 <= x; --x ){
                                    skeletonIndex = -1;
                                    depthPos.x = x;
                                    depthFrame->GetDepth(x,y,&skeletonIndex);
                                    if( !isEnableTrackingPlayerSkeleotn(skeletonIndex) ) {
                                        if( isEnableDrawDepthHeadPointByError(depthPos,TB_TRUE,checkYHeadPoint.y < y) ){
#if TB_IS_DEBUG
                                            if( IsEnableDebugMaskCorrect() ){
                                                color = TB_COLOR_A8R8G8B8(0xFF,0xFF,0,0);
                                            }else
#endif
                                            {
                                                color = m_DepthMaskColor.value;
                                            }
                                            *reinterpret_cast<TbUint32*>(&dstBuffer[ y*dstPitch + x*4 ]) = color;
                                            isUpdateX = TB_TRUE;
                                        }else{
                                            break;
                                        }
                                    }else{
                                        isUpdateX = TB_TRUE;
                                    }
                                }
                                // �E
                                for( TbSint32 x = headPoint.x; x < depthFrame->GetResolution().width; ++x ){
                                    skeletonIndex = -1;
                                    depthPos.x = x;
                                    depthFrame->GetDepth(x,y,&skeletonIndex);
                                    if( !isEnableTrackingPlayerSkeleotn(skeletonIndex) ) {
                                        if( isEnableDrawDepthHeadPointByError(depthPos,TB_FALSE,checkYHeadPoint.y < y) ){
#if TB_IS_DEBUG
                                            if( IsEnableDebugMaskCorrect() ){
                                                color = TB_COLOR_A8R8G8B8(0xFF,0,0xFF,0);
                                            }else
#endif
                                            {
                                                color = m_DepthMaskColor.value;;
                                            }
                                            *reinterpret_cast<TbUint32*>(&dstBuffer[ y*dstPitch + x*4 ]) = color;
                                            isUpdateX = TB_TRUE;
                                        }else{
                                            break;
                                        }
                                    }else{
                                        isUpdateX = TB_TRUE;
                                    }
                                }
                                if( !isUpdateX ){
                                    break;
                                }
                            }
                        }
                    }
                }
#endif

                m_DepthTexture->Unlock(0);
            }
            depthFrame->Unlock();
        }
    }
}

/*!
 * �J���[�o�b�t�@�ɂ����鍜�i���W
 * @author teco
 * @since 2012.08.09
 */
void TbNuiSimpleManager::updateColorSkeleton( TbSint32 playerIndex )
{
    const TbNuiSkeleton* skeleton = m_NuiManager->GetSkeleton( m_PlayerInfo[playerIndex].skeletonIndex );
    if( !skeleton ){
        // �g���b�L���O���Ă���X�P���g�������Ȃ��Ȃ�A�X�V���Ȃ�
        return;
    }

    PlayerInfo& playerInfo = m_PlayerInfo[playerIndex];
    TbNuiColorPos colorPos;
    
    colorPos.resolutionType = m_NuiManager->GetColorFrame()->GetResolutionType();
    for( TbSint32 posIndex = 0; posIndex < TB_NUI_SKELETON_POS_COUNT; ++posIndex ) {
        TbVector4& pos = skeleton->trackedSkeleton->skeletonPos[ posIndex ];
        if( TB_SUCCEEDED( TbNuiManager::GetInstance().TransformSkeletonToColor( colorPos , pos ) ))
        {
            playerInfo.colorSkeleton[ posIndex ].SetX( static_cast<TbFloat32>(colorPos.x) );
            playerInfo.colorSkeleton[ posIndex ].SetY( static_cast<TbFloat32>(colorPos.y) );
        }
    }

    // ���_�o�b�t�@�ɏ�������
    if( IsEnableUpdateSkeletonVertexBuffer() )
    {
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 0 , TB_NUI_SKELETON_POS_HEAD , TB_NUI_SKELETON_POS_SHOULDER_C );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 1 , TB_NUI_SKELETON_POS_SHOULDER_C , TB_NUI_SKELETON_POS_SHOULDER_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 2 , TB_NUI_SKELETON_POS_SHOULDER_L , TB_NUI_SKELETON_POS_ELBOW_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 3 , TB_NUI_SKELETON_POS_ELBOW_L , TB_NUI_SKELETON_POS_WRIST_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 4 , TB_NUI_SKELETON_POS_WRIST_L , TB_NUI_SKELETON_POS_HAND_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 5 , TB_NUI_SKELETON_POS_SHOULDER_C , TB_NUI_SKELETON_POS_SHOULDER_R );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 6 , TB_NUI_SKELETON_POS_SHOULDER_R , TB_NUI_SKELETON_POS_ELBOW_R );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 7 , TB_NUI_SKELETON_POS_ELBOW_R , TB_NUI_SKELETON_POS_WRIST_R );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 8 , TB_NUI_SKELETON_POS_HAND_R , TB_NUI_SKELETON_POS_HAND_R );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 9 , TB_NUI_SKELETON_POS_SHOULDER_C , TB_NUI_SKELETON_POS_SPINE );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 10 , TB_NUI_SKELETON_POS_SPINE , TB_NUI_SKELETON_POS_HIP_C );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 11 , TB_NUI_SKELETON_POS_HIP_C , TB_NUI_SKELETON_POS_HIP_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 12 , TB_NUI_SKELETON_POS_HIP_L , TB_NUI_SKELETON_POS_KNEE_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 13 , TB_NUI_SKELETON_POS_KNEE_L , TB_NUI_SKELETON_POS_ANKLE_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 14 , TB_NUI_SKELETON_POS_ANKLE_L , TB_NUI_SKELETON_POS_FOOT_L );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 15 , TB_NUI_SKELETON_POS_HIP_C , TB_NUI_SKELETON_POS_HIP_R );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 16 , TB_NUI_SKELETON_POS_HIP_R , TB_NUI_SKELETON_POS_KNEE_R );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 17 , TB_NUI_SKELETON_POS_KNEE_R , TB_NUI_SKELETON_POS_ANKLE_R );
        writePlayerSkeletonPosToVertexBuffer( playerIndex , 18 , TB_NUI_SKELETON_POS_ANKLE_R , TB_NUI_SKELETON_POS_FOOT_R );
        playerInfo.skeletonVertexBuffer->Write(sizeof(playerInfo.skeletonVertex),playerInfo.skeletonVertex);
    }
}

/*!
 * ���i�ʒu���W�𒸓_�o�b�t�@�ɏ�������
 * @author teco
 * @since 2011.08.14
 */
void TbNuiSimpleManager::writePlayerSkeletonPosToVertexBuffer( TbSint32 index , TbSint32 jointIndex , TbNuiSkeletonPos pos1 , TbNuiSkeletonPos pos2 )
{
    const TbNuiSkeleton* skeleton = m_NuiManager->GetSkeleton( m_PlayerInfo[index].skeletonIndex );
    const TbDimensionS32& colorResolution = m_NuiManager->GetColorFrame()->GetResolution();
    PlayerInfo& playerInfo = m_PlayerInfo[index];
    if( skeleton && skeleton->trackedSkeleton ){
        {
            TbVector4& pos = playerInfo.colorSkeleton[ pos1 ];
            playerInfo.skeletonVertex[jointIndex*2].x = 2.0f * (pos.GetX() / static_cast<TbFloat32>(colorResolution.width) - 0.5f);
            playerInfo.skeletonVertex[jointIndex*2].y = 2.0f * (-pos.GetY() / static_cast<TbFloat32>(colorResolution.height) + 0.5f);
        }
        {
            TbVector4& pos = playerInfo.colorSkeleton[ pos2 ];
            playerInfo.skeletonVertex[jointIndex*2+1].x = 2.0f * (pos.GetX() / static_cast<TbFloat32>(colorResolution.width) - 0.5f);
            playerInfo.skeletonVertex[jointIndex*2+1].y = 2.0f * (-pos.GetY() / static_cast<TbFloat32>(colorResolution.height) + 0.5f);
        }
    }
}

}