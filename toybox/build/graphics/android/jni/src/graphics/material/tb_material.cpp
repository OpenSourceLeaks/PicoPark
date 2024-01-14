/*!
 * マテリアル
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */

#include <tb_fwd.h>
#include <graphics/material/tb_material.h>
#include <graphics/render/3dapi/tb_render_device.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/render/tb_render_pass.h>

namespace toybox
{

/*!
 * シェーダプログラムを有効化
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbMaterial::ActivateProgram( TbUint32 passIndex )
{
    TbRenderPass* pass = TbGetRenderPass(passIndex);
    if( pass ){
        pass->SetShaderProgram( m_Program );
    }
}

}
