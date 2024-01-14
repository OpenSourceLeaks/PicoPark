#include "fwd.h"
#include "common/cmn_simple_sprite.h"
#include "root.h"

// コンストラクタ
CmnSimpleSprite::CmnSimpleSprite()
    : Super()
    , m_Material(TbFrameworkSimple::RENDER_PASS_2D)
{
    SetMaterial(&m_Material);
    m_Material.SetTexture(Root::GetInstance().GetCommonTexture());
}

