/*!
 * シェーダ管理
 * @author teco
 */

#ifndef _INCLUDED_SYS_SHADER_MANAGER_H_
#define _INCLUDED_SYS_SHADER_MANAGER_H_

#include <graphics/render/3dapi/tb_color.h>
#include <graphics/render/3dapi/tb_shader_manager.h>
#include <graphics/render/3dapi/tb_shader_program.h>
#include <graphics/render/3dapi/tb_constant_buffer.h>

enum SysShaderType
{
    SYS_SHADER_PLAYER , 
    SYS_SHADER_MAX
};

enum SysConstantBufferType
{
    SYS_CONSTANT_BUFFER_MATRIX , 
    SYS_CONSTANT_BUFFER_MAX
};

class SysShaderManager : public TbSingletonRef<SysShaderManager>
{
public:

    // コンストラクタ
    SysShaderManager();

    // デストラクタ
    ~SysShaderManager() {
        Finalize();
    }

public:

    // 初期化
    void Initialize();

    // 後始末
    void Finalize();

    // リロード
    void Reload();

public:

    // プログラム取得
    TbShaderProgram* GetProgram(SysShaderType type) {
        return m_Program[type];
    }

    // ConstantBuffer取得
    TbConstantBuffer* GetConstantBuffer(SysConstantBufferType type) {
        return m_ConstantBuffer[SYS_CONSTANT_BUFFER_MATRIX];
    }

private:

    enum VerteXFormatType
    {
        VERTEX_FORMAT_XYZ_COLOR_TEX0SIZE2 ,
        VERTEX_FORMAT_MAX
    };

private:

    TbShaderProgram*    m_Program[SYS_SHADER_MAX];
    TbVertexFormat*     m_VertexFormat[VERTEX_FORMAT_MAX];
    TbConstantBuffer*   m_ConstantBuffer[SYS_CONSTANT_BUFFER_MAX];

};

#endif
