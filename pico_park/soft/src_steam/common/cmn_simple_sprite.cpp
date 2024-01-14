#include "fwd.h"
#include "common/cmn_simple_sprite.h"
#include "root.h"

// �R���X�g���N�^
CmnSimpleSprite::CmnSimpleSprite()
    : Super()
    , m_Material(TbFrameworkSimple::RENDER_PASS_2D)
{
    SetMaterial(&m_Material);
    m_Material.SetTexture(Root::GetInstance().GetCommonTexture());
}

