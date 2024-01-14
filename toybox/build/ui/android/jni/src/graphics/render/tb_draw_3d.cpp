/*!
 * 2D描画
 * @author Miyake Shunsuke
 * @since 2010.12.03
 */

#include "tb_fwd.h"
#include "graphics/render/tb_draw_2d.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "graphics/math/tb_gr_matrix_util.h"
#include "graphics/render/tb_render_pass_manager.h"
#include "base/math/tb_matrix.h"

namespace toybox
{

static TbUint32 s_PassIndex = 0;

/*!
 * レンダリングパス設定
 * @author Miyake Shunsuke
 * @since 2011.03.20
 */
TbResult TbDraw3dSetRenderPass( TbUint32 index )
{
    s_PassIndex = index;
    return TB_S_OK;
}

/*!
 * レンダリングパス取得
 * @author Miyake Shunsuke
 * @since 2013.03.20
 */
TbUint32 TbDraw3dGetRenderPass()
{
    return s_PassIndex;
}

/*!
 * 箱描画
 * @author Miyake_Shunsuke
 * @since 2014.04.11
 */
TbResult TbDrawBox( const TbVector4& pos , TbFloat32 width , TbFloat32 height , TbFloat32 depth , const TbColor& color )
{
    const TbVertexXYZNormalColor VERTEX_BUFFER[24] = 
    {
        {-width/2.0f, -height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , color },
        {-width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , color },
        { width/2.0f, -height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , color }, // 頂点、法線、カラー、UV
        { width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , color }, 

        {-width/2.0f,  -height/2.0f, -depth/2.0f , 0.0f , -1.0f , 0.0f , color },
        {-width/2.0f,  -height/2.0f,  depth/2.0f , 0.0f , -1.0f , 0.0f , color },
        { width/2.0f,  -height/2.0f, -depth/2.0f , 0.0f , -1.0f , 0.0f , color }, // 頂点、法線、カラー、UV
        { width/2.0f,  -height/2.0f,  depth/2.0f , 0.0f , -1.0f , 0.0f , color }, 

        { -width/2.0f,  -height/2.0f, -depth/2.0f , -1.0f , 0.0f , 0.0f , color },
        { -width/2.0f,  -height/2.0f,  depth/2.0f , -1.0f , 0.0f , 0.0f , color },
        { -width/2.0f,   height/2.0f, -depth/2.0f , -1.0f , 0.0f , 0.0f , color }, // 頂点、法線、カラー、UV
        { -width/2.0f,   height/2.0f,  depth/2.0f , -1.0f , 0.0f , 0.0f , color }, 

        {-width/2.0f, -height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , color },
        {-width/2.0f,  height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , color },
        { width/2.0f, -height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , color }, // 頂点、法線、カラー、UV
        { width/2.0f,  height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , color }, 

        {-width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 1.0f , 0.0f , color },
        {-width/2.0f,  height/2.0f,  depth/2.0f , 0.0f , 1.0f , 0.0f , color },
        { width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 1.0f , 0.0f , color }, // 頂点、法線、カラー、UV
        { width/2.0f,  height/2.0f,  depth/2.0f , 0.0f , 1.0f , 0.0f , color }, 

        { width/2.0f,  -height/2.0f, -depth/2.0f , 1.0f , 0.0f , 0.0f , color },
        { width/2.0f,  -height/2.0f,  depth/2.0f , 1.0f , 0.0f , 0.0f , color },
        { width/2.0f,   height/2.0f, -depth/2.0f , 1.0f , 0.0f , 0.0f , color }, // 頂点、法線、カラー、UV
        { width/2.0f,   height/2.0f,  depth/2.0f , 1.0f , 0.0f , 0.0f , color }, 
    };

    const TbUint16 INDEX_BUFFER[] =
    {
        0 , 1 , 2 , 
        2 , 1 , 3 , 

        4 , 5 , 6 , 
        6 , 5 , 7 , 

        8 , 9 , 10 , 
        10 , 9 , 11 , 

        12 , 13 , 14 , 
        14 , 13 , 15 , 

        16 , 17 , 18 , 
        18 , 17 , 19 , 

        20 , 21 , 22 , 
        22 , 21 , 23 , 
    };
    
    TbRenderPass* renderPass = TbGetRenderPass(s_PassIndex);
    renderPass->SetShaderProgram(nullptr);
    renderPass->SetTexture(0,nullptr);
    renderPass->SetFixedShaderVertexFormat( &TbVertexFormat::XYZ_NORMAL_COLOR );
    TbMatrix world;
    world.SetTranslation(pos);
    renderPass->SetWorldMatrix(world);
    renderPass->DrawIndexedPrimitive( TB_PT_TRIANGLELIST , 
                                      0 , 
                                      24 ,
                                      INDEX_BUFFER,
                                      TB_INDEX_BUFFER_FORMAT_U16,
                                      VERTEX_BUFFER ,
                                      sizeof(TbVertexXYZNormalColor) ,
                                      12 );
    return TB_S_OK;
}

}