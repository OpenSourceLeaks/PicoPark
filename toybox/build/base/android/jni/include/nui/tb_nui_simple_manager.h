/*!
 * NUI�̃V���v���Ǘ��N���X
 * @author teco
 */

#ifndef _INCLUDED_TB_NUI_SIMPLE_MANAGER_H_
#define _INCLUDED_TB_NUI_SIMPLE_MANAGER_H_

#include <nui/tb_nui_types.h>
#include <nui/tb_nui_manager.h>
#include <base/util/tb_singleton.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector4.h>
#include <graphics/render/3dapi/tb_texture.h>
#include <graphics/render/3dapi/tb_vertex_buffer.h>
#include <graphics/render/3dapi/tb_vertex_format.h>

namespace toybox
{

class TbNuiSimpleManager : public TbSingletonRef<TbNuiSimpleManager>
{
    typedef TbNuiSimpleManager Self;

public:

    enum ExportFileFormat {
        EXPORT_FILE_FORMAT_BMP , 
        EXPORT_FILE_FORMAT_TGA , 
        EXPORT_FILE_FORMAT_MAX , 
    };

public:
    
    // �R���X�g���N�^
    TbNuiSimpleManager( TbUint32 playerMax );

    // �f�X�g���N�^
    virtual ~TbNuiSimpleManager();

public:

    // �X�V
    void Update();

public:

    // ���邳�ݒ�
    void SetBright( TbSint32 bright ) {
        m_Bright = bright;
    }

    // ���邳�擾
    TbSint32 GetBright() const {
        return m_Bright;
    }

public:

    // �J���[�p�e�N�X�`���擾
    toybox::TbTexture* GetColorTexture( TbSizeT historyIndex = 0 ) { 
        TbSizeT index = (m_ColorTextureFrontIndex + historyIndex)%TB_ARRAY_COUNT_OF(m_ColorTexture);
        return m_ColorTexture[index];
    }

    // �J���[�pUV�擾
    TbRectF32 GetColorUV() const {
        return m_ColorUV;
    }

    // �[�x�p�e�N�X�`���擾
    toybox::TbTexture* GetDepthTexture() { 
        return m_DepthTexture; 
    }

    // �[�x�pUV�擾
    TbRectF32 GetDepthUV() const {
        return m_DepthUV;
    }

    // ���i�p���_�o�b�t�@�擾
    toybox::TbVertexBuffer* GetSkeletonVertexBuffer( TbUint32 playerIndex , TB_OUT TbUint32* stride );

    // ���i�p���_�o�b�t�@���X�V���邩
    void SetEnableUpdateSkeletonVertexBuffer( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_SKELETON_VERTEX,isEnable);
    }

    // ���i�p���_�o�b�t�@���X�V�\��
    TbBool IsEnableUpdateSkeletonVertexBuffer() const {
        return m_BitArray.Test(FLAG_ENABLE_SKELETON_VERTEX);
    }

    // ���i�p���_�t�H�[�}�b�g�擾
    const toybox::TbVertexFormat* GetSkeletonVertexFormat() { 
        return &TbVertexFormat::XYZ_COLOR; 
    }

public:

    // �v���C���[���擾
    TbUint32 GetPlayerSkeletonCount() const { 
        return m_PlayerSkeletonCount; 
    }

    // �ő�v���C���[�X�P���g�����擾
    TbUint32 GetPlayerSkeletonMax() const { 
        return m_PlayerSkeletonMax; 
    }

    // �ő�v���C���[�X�P���g�����ݒ�
    void SetPlayerSkeletonMax( TbUint32 max );

    // �v���C���[�X�P���g���C���f�b�N�X�擾
    TbSint32 GetPlayerSkeletonIndex( TbUint32 playerIndex ) const;

    // �v���C���[���g���b�L���O���Ă���
    TbBool IsTrackingPlayer( TbUint32 playerIndex ) const {
        return playerIndex < m_PlayerSkeletonCount;
    }

    // �J���[�o�b�t�@�Ƀ}�b�s���O���ꂽ���i���W�擾
    const toybox::TbVector4* GetPlayerSkeletonColorPos( TbUint32 playerIndex ) const;

    // �J���[�e�N�X�`���X�V�\
    void SetEnableUpdateColorTexture( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_COLOR_TEXTURE,isEnable);
    }

    // �J���[�e�N�X�`���X�V�\
    TbBool IsEnableUpdateColorTexture() const {
        return m_BitArray.Test(FLAG_ENABLE_COLOR_TEXTURE);
    }

    // �[�x�e�N�X�`���X�V�\
    void SetEnableUpdateDepthTexture( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_DEPTH_TEXTURE,isEnable);
    }

    // �[�x�e�N�X�`���X�V�\
    TbBool IsEnableUpdateDepthTexture() const {
        return m_BitArray.Test(FLAG_ENABLE_DEPTH_TEXTURE);
    }


    // �J���[�e�N�X�`���Ƀ}�X�N�\�ݒ�
    void SetEnableColorPlayerMask( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_COLOR_MASK , isEnable ); 
    }

    // �J���[�e�N�X�`���Ƀv���C���[�}�X�N�\�擾
    TbBool IsEnableColorPlayerMask() const { 
        return m_BitArray.Test( FLAG_COLOR_MASK ); 
    }

    // �[�x�e�N�X�`���Ƀv���C���[�}�X�N�\�ݒ�
    void SetEnableDepthPlayerMask( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_DEPTH_MASK , isEnable ); 
    }

    // �[�x�e�N�X�`���Ƀv���C���[�}�X�N�\�擾
    TbBool IsEnableDepthPlayerMask() const { 
        return m_BitArray.Test( FLAG_DEPTH_MASK ); 
    }

    // �[�x�p�}�X�N�J���[�ݒ�
    void SetDepthMaskColor( const TbColorU32& color ) {
        m_DepthMaskColor = color;
    }

    // �[�x�}�X�N�J���[�L���ݒ�
    void SetEnableDepthMaskColor( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_DEPTH_MASK_COLOR , isEnable ); 
    }

    // �[�x�}�X�N�J���[�L���ݒ�
    TbBool IsEnableDepthMaskColor() const {
        return m_BitArray.Test(FLAG_ENABLE_DEPTH_MASK_COLOR);
    }

    // �v���C���[�ʃ}�X�N�\�ݒ�
    void SetEnablePlayerMask( TbUint32 playerIndex , TbBool isEnable );

    // �v���C���[�ʃ}�X�N�\�ݒ�
    TbBool IsEnablePlayerMask( TbUint32 playerIndex );

public: // �}�X�N�␳

    // �}�X�N�␳�f�o�b�O�\��
    void SetEnableDebugMaskCorrect( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_DEBUG_CORRECT_MASK, isEnable ); 
    }

    // �}�X�N�␳�f�o�b�O�\��
    TbBool IsEnableDebugMaskCorrect() const { 
        return m_BitArray.Test( FLAG_ENABLE_DEBUG_CORRECT_MASK ); 
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectLeftLoopX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftLoop.x = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectLeftLoopX() const {
        return m_PlayerMaskCorrectParam.leftLoop.x;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectLeftLoopY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftLoop.y = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectLeftLoopY() const {
        return m_PlayerMaskCorrectParam.leftLoop.y;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectLeftUnitX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftUnit.x = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectLeftUnitX() const {
        return m_PlayerMaskCorrectParam.leftUnit.x;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectLeftUnitY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftUnit.y = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectLeftUnitY() const {
        return m_PlayerMaskCorrectParam.leftUnit.y;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectRightLoopX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightLoop.x = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectRightLoopX() const {
        return m_PlayerMaskCorrectParam.rightLoop.x;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectRightLoopY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightLoop.y = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectRightLoopY() const {
        return m_PlayerMaskCorrectParam.rightLoop.y;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectRightUnitX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightUnit.x = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectRightUnitX() const {
        return m_PlayerMaskCorrectParam.rightUnit.x;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectRightUnitY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightUnit.y = value;
    }

    // �}�X�N�␳�p�����[�^
    TbSint32 GetMaskCorrectRightUnitY() const {
        return m_PlayerMaskCorrectParam.rightUnit.y;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectBeginRatio( TbFloat32 ratio ) {
        m_PlayerMaskCorrectParam.beginRatio = ratio;
    }

    // �}�X�N�␳�p�����[�^
    TbFloat32 GetMaskCorrectBeginRatio() const {
        return m_PlayerMaskCorrectParam.beginRatio;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectCheckYBeginRatio( TbFloat32 ratio ) {
        m_PlayerMaskCorrectParam.checkYBeginRatio = ratio;
    }

    // �}�X�N�␳�p�����[�^
    TbFloat32 GetMaskCorrectCheckYBeginRatio() const {
        return m_PlayerMaskCorrectParam.checkYBeginRatio;
    }

    // �}�X�N�␳�p�����[�^
    void SetMaskCorrectErrorOffset( TbFloat32 offset ) {
        m_PlayerMaskCorrectParam.errorOffset = offset;
    }

    // �}�X�N�␳�p�����[�^
    TbFloat32 GetMaskCorrectErrorOffset() const {
        return m_PlayerMaskCorrectParam.errorOffset;
    }

public: // �f�o�b�O

    // �J���[�Ɛ[�x���}�b�s���O�ł��Ȃ��̈�f�o�b�O�L��
    void SetEnableDebugUnmapRegion( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_DEBUG_UNMAP_REGION, isEnable ); 
    }

    // �J���[�Ɛ[�x���}�b�s���O�ł��Ȃ��̈�f�o�b�O�L��
    TbBool IsEnableDebugUnmapRegion() const { 
        return m_BitArray.Test( FLAG_ENABLE_DEBUG_UNMAP_REGION ); 
    }

    // �J���[�e�N�X�`���ւ̃}�X�N�̕␳��L��
    void SetEnableMaskCorrect( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_CORRECT_MASK, isEnable ); 
    }

    // �J���[�e�N�X�`���ւ̃}�X�N�̕␳��L��
    TbBool IsEnableMaskCorrect() const { 
        return m_BitArray.Test( FLAG_ENABLE_CORRECT_MASK ); 
    }

public:

    // �J���[�p�e�N�X�`���t�@�C���o��
    TbResult ExportColorImage( ExportFileFormat format );

private:

    enum {
        FLAG_ENABLE_COLOR_TEXTURE            , // �J���[�e�N�X�`�����p
        FLAG_ENABLE_DEPTH_TEXTURE            , // �[�x�[�e�N�X�`�����p

        FLAG_COLOR_MASK                      , // �J���[�}�X�N
        FLAG_DEPTH_MASK                      , // �[�x�}�X�N

        FLAG_ENABLE_CORRECT_MASK             , // �}�X�N�␳
        FLAG_ENABLE_DEBUG_CORRECT_MASK       , // �}�X�N�␳�f�o�b�O�`��
        FLAG_ENABLE_DEBUG_UNMAP_REGION       , // �[�x�ƃJ���[�Ń}�b�s���O�ł��Ȃ��̈�

        FLAG_ENABLE_DEPTH_MASK_COLOR         , // �[�x�ւ̃}�X�N�J���[�L��

        FLAG_VERTEX_TO_DEPTH                 , // ���_�͐[�x�o�b�t�@�Ƀ}�b�s���O���� (�f�t�H���g�̓J���[)
        FLAG_ENABLE_SKELETON_VERTEX          , // ���i�̒��_�o�b�t�@���X�V���邩
        FLAG_READY                           , // ��������
    };

    static const TbSint32 VERTEX_COUNT = 38;
    static const TbUint32 COLOR_TEXTURE_HISTORY_COUNT = 1; //!< �J���[�e�N�X�`���̗���

    // �v���C���[���
    struct PlayerInfo
    {
        TbSint32            skeletonIndex;
        TbBool              isEnableMask;
        TbVector4           colorSkeleton[TB_NUI_SKELETON_POS_COUNT];
        TbVertexBuffer*     skeletonVertexBuffer;              // ���i�p���_�o�b�t�@
        TbVertexXYZColor    skeletonVertex[VERTEX_COUNT];      // ���i�p���_
    };

    struct PlayerMaskCorrectParam
    {
        PlayerMaskCorrectParam() 
            : beginRatio(0.8f)
            , checkYBeginRatio(0.0f)
            , errorOffset(0.8f)
        {}
        TbPointS32 leftUnit;
        TbPointS32 leftLoop;
        TbPointS32 rightUnit;
        TbPointS32 rightLoop;
        TbFloat32  beginRatio;
        TbFloat32  checkYBeginRatio;
        TbFloat32  errorOffset;
        TbPointS32 depthUnit;
        TbPointS32 depthLoop;
    };

private:

    // �v���C���[�̃X�P���g���C���f�b�N�X���X�V
    void updatePlayerSkeletonIndex();

    // �J���[�e�N�X�`���X�V
    void updateColorTexture();

    // �[�x�e�N�X�`���X�V
    void updateDepthTexture();
    
    // �J���[�o�b�t�@�ɂ����鍜�i���W�X�V
    void updateColorSkeleton( TbSint32 playerIndex );

    // ���i�ʒu���W�𒸓_�o�b�t�@�ɏ�������
    void writePlayerSkeletonPosToVertexBuffer( TbSint32 index , TbSint32 jointIndex , TbNuiSkeletonPos pos1 , TbNuiSkeletonPos pos2 );

    // ���̈ʒu�`��\��
    TbBool isEnableDrawHeadPointByError( const TbNuiDepthPos& depthPos , TbBool isLeft , TbBool isEnableIgnoreY );
    TbBool isEnableDrawDepthHeadPointByError( const TbNuiDepthPos& depthPos , TbBool isLeft , TbBool isEnableIgnoreY );

    // �g���b�L���O�X�P���g��
    TbBool isEnableTrackingPlayerSkeleotn( TbSint32 skeletonIndex ){
        return 0 <= skeletonIndex && 0 < (m_PlayerSkeletonIndexBit & TB_BIT(skeletonIndex));
    }

private:

    TbBitArray32            m_BitArray;                                  // �r�b�g�z��
    TbNuiManager*           m_NuiManager;                                // NUI�Ǘ��N���X
    TbTexture*              m_ColorTexture[COLOR_TEXTURE_HISTORY_COUNT]; // �J���[�e�N�X�`��
    TbUint32                m_ColorTextureFrontIndex;                    // �擪�C���f�b�N�X
    TbRectF32               m_ColorUV;
    TbTexture*              m_DepthTexture;                              // �[�x�e�N�X�`��
    TbRectF32               m_DepthUV;
    TbSint32                m_Bright;                                    // ���邳
    TbColorU32              m_DepthMaskColor;                            // �[�x�}�X�N�J���[
    PlayerMaskCorrectParam  m_PlayerMaskCorrectParam;                      // �G���[���e�p�����[�^

    TbUint32                m_PlayerSkeletonCount;                       // �v���C���[��
    TbUint32                m_PlayerSkeletonMax;                         // �v���C���[�ő吔
    TbUint32                m_PlayerSkeletonIndexBit;                    // �v���C���[�r�b�g�l
    PlayerInfo              m_PlayerInfo[TB_NUI_TRACKED_SKELETON_MAX];   // �v���C���[�ʏ��
};

}

#endif
