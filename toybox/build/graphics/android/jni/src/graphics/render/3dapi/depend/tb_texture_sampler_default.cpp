/*!
 * @file
 * @brief 
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture_sampler.h"

#if (TB_3DAPI_IS_DX9) || (TB_3DAPI_IS_GL) || (TB_3DAPI_IS_S3D)

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.04.03
 */
TbTextureSampler::TbTextureSampler()
    : m_IsInitialized(TB_FALSE)
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.04.03
 */
TbTextureSampler::~TbTextureSampler()
{
}

/*!
* 初期化
* @author Miyake Shunsuke
*/
void TbTextureSampler::Initialize(const TbTextureSamplerParam& param)
{
    m_Param = param;
    m_IsInitialized = TB_TRUE;
}

/*!
* 後始末
* @author Miyake Shunsuke
*/
void TbTextureSampler::Finalize()
{
}

}

#endif
