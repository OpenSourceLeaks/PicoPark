/*!
 * ���i�r���[��
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */

#ifndef _INCLUDED_SKELETON_VIEWER_H_
#define _INCLUDED_SKELETON_VIEWER_H_

#include <nui/tb_nui_types.h>

class SkeletonViewer
{
    typedef SkeletonViewer Self;
public:
    enum ExportFileFormat {
        EXPORT_FILE_FORMAT_BMP , 
        EXPORT_FILE_FORMAT_TGA , 
        EXPORT_FILE_FORMAT_MAX , 
    };
public:
    // �R���X�g���N�^
    SkeletonViewer();
    // �f�X�g���N�^
    ~SkeletonViewer();
public:
    // �X�V
    void Update();
    // �J���[�p�e�N�X�`���t�@�C���o��
    TbResult ExportColorImage( ExportFileFormat format );
    // �v���C���[�X�P���g���C���f�b�N�X�擾
    TbSint32 GetPlayerSkeletonIndex() const { 
        return m_PlayerSkeletonIndex; 
    }
    // �v���C���[�J���[�}�X�N�\�ݒ�
    void SetEnableMaskPlayerColor( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_MASK_COLOR , isEnable ); 
    }
    // �v���C���[�J���[�}�X�N�\�擾
    TbBool IsEnableMaskPlayerColor() const { 
        return m_BitArray.Test( FLAG_MASK_COLOR ); 
    }
    // �v���C���[�[�x�}�X�N�\�ݒ�
    void SetEnableMaskPlayerDepth( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_MASK_DEPTH , isEnable ); 
    }
    // �v���C���[�[�x�}�X�N�\�擾
    TbBool IsEnableMaskPlayerDepth() const { 
        return m_BitArray.Test( FLAG_MASK_DEPTH ); 
    }
    // �J���[�p�e�N�X�`���擾
    toybox::TbTexture* GetColorTexture() { 
        return m_ColorTexture; 
    }
    // �[�x�p�e�N�X�`���擾
    toybox::TbTexture* GetDepthTexture() { 
        return m_DepthTexture; 
    }
    // ���i�p���_�o�b�t�@�擾
    toybox::TbVertexBuffer* GetSkeletonVertexBuffer( TbUint32* stride );
    // ���i�p���_�t�H�[�}�b�g�擾
    toybox::TbVertexFormat* GetSkeletonVertexFormat() { 
        return m_SkeletonVertexFormat; 
    }
private:

    enum {
        FLAG_MASK_COLOR , // �J���[�}�X�N
        FLAG_MASK_DEPTH , // �[�x�}�X�N
    };

    struct VertexType
    {
        TbFloat32 x , y , z;
        DWORD color;//toybox::TbColor color;
    };

    static const TbSint32 VERTEX_COUNT = 38;

private:

    // �v���C���[�̃X�P���g���C���f�b�N�X���X�V
    void updatePlayerSkeletonIndex();

    // �J���[�e�N�X�`���X�V
    void updateColorTexture();

    // �[�x�e�N�X�`���X�V
    void updateDepthTexture();

    // ���i�̒��_�o�b�t�@�X�V
    void updateSkeletonVertexBuffer();

    // ���i�ʒu���W�𒸓_�o�b�t�@�ɏ�������
    void writePlayerSkeletonPosToVertexBuffer( TbSint32 index , toybox::TbNuiSkeletonPos pos1 , toybox::TbNuiSkeletonPos pos2 );

private:
    TbUint32                m_Phase;
    toybox::TbBitArray32    m_BitArray;               // �r�b�g�z��
    toybox::TbNuiManager*   m_NuiManager;             // NUI�Ǘ��N���X
    TbUint32                m_PlayerSkeletonIndex;    // �v���C��[�X�P���g���C���f�b�N�X
    toybox::TbVertexBuffer* m_SkeletonVertexBuffer;   // ���i�p���_�o�b�t�@
    toybox::TbVertexFormat* m_SkeletonVertexFormat;   // ���i�p���_�t�H�[�}�b�g
    VertexType              m_SkeletonVertex[VERTEX_COUNT];
    toybox::TbTexture*      m_ColorTexture;           // �J���[�e�N�X�`��
    toybox::TbTexture*      m_DepthTexture;           // �[�x�e�N�X�`��
};

#endif
