/*!
 * シェーダ管理クラスの共通処理
 * @author Miyake Shunsuke
 * @since 2014.04.02
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "graphics/render/3dapi/tb_shader_program.h"

namespace toybox
{

/*!
 * プログラム生成(パラメータ付き)
 * @author Miyake_Shunsuke
 * @since 2014.04.02
 */
TbShaderProgram* TbShaderManager::CreateProgram( const ProgramCreateParam& param ) {
    TbShaderId vertexShaderId = CreateShaderFromFile( TB_SHADER_TYPE_VERTEX ,
                                                        TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                        param.shaderFilePath[TB_SHADER_TYPE_VERTEX],
                                                        "main",
                                                        nullptr );
    TbShaderId pixelShaderId =  CreateShaderFromFile( TB_SHADER_TYPE_PIXEL ,
                                                        TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                        param.shaderFilePath[TB_SHADER_TYPE_PIXEL],
                                                        "main",
                                                        nullptr );
    BindVertexFormat( param.vertexFormat , vertexShaderId );
    TbShaderProgram* result = CreateProgram();
    result->SetShader(TB_SHADER_TYPE_VERTEX,vertexShaderId);
    result->SetShader(TB_SHADER_TYPE_PIXEL,pixelShaderId);
    result->Update();
    return result;
}

}
