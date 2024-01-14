/*!
 * Nui�p�V���G�b�g�̗֊s���`��t�B���^�[
 * @author Miyake Shunsuke
 * @since 2011.10.21
 */

#ifndef _INCLUDED_GR_FILETER_NUI_OUTLINE_H_
#define _INCLUDED_GR_FILETER_NUI_OUTLINE_H_

#include <graphics/render/3dapi/tb_3dapi.h>
#include <base/math/tb_geometry.h>

namespace toybox
{
    class TbTexture;
    class TbTextureSampler;
    class TbVertexBuffer;
};

class RenderFilterNuiOutline
{
public:

    // �R���X�g���N�^
    RenderFilterNuiOutline( const toybox::TbRectF32& uv  );

    // �f�X�g���N�^
    ~RenderFilterNuiOutline();

public:

    // �`��
    TbResult Render( toybox::TbTexture* texture , const toybox::TbRectF32& uv );

private:

    toybox::TbTextureSampler*   m_TextureSampler;           // �e�N�X�`���T���v��
    toybox::TbShaderConstHandle m_ConstHandleTexWidth;
    toybox::TbShaderConstHandle m_ConstHandleTexHeight;
    toybox::TbShaderConstHandle m_ConstHandleOffsetMatrix;

private:    // �����_�����O�p�X1�p ( �ڂ����V���G�b�g���o )

    toybox::TbShaderId       m_VertexShaderIdSilhouette; // ���_�V�F�[�_
    toybox::TbShaderId       m_PixelShaderIdSilhouette;  // �s�N�Z���V�F�[�_
    toybox::TbVertexFormat*  m_VertexFormatSilhouette;   // ���_�t�H�[�}�b�g
    toybox::TbVertexBuffer*  m_VertexBufferSilhouette;   // ���_�o�b�t�@
    toybox::TbTexture*       m_RenderTextureSilhouette;  // �����_�[�^�[�Q�b�g 
    toybox::TbShaderProgram* m_ProgramSilhouette;

private: // �����_�����O�p�X2�p ( �֊s���𒊏o���ăJ���[�o�b�t�@�ƍ���  )

    toybox::TbShaderId       m_VertexShaderIdOutline; // ���_�V�F�[�_
    toybox::TbShaderId       m_PixelShaderIdOutline;  // �s�N�Z���V�F�[�_
    toybox::TbVertexFormat*  m_VertexFormatOutline;   // ���_�t�H�[�}�b�g
    toybox::TbVertexBuffer*  m_VertexBufferOutline;   // ���_�o�b�t�@
    toybox::TbShaderProgram* m_ProgramOutline;

};

#endif
