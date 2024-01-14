/*!
 * スクロールレンジ
 * @author teco
 */

#include "fwd.h"
#include "side_action_scroll_range.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionScrollLimitRange::SideActionScrollLimitRange( TbFloat32 width)
    : m_Width(width)
    , m_Phase(0)
{
};

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionScrollLimitRange::~SideActionScrollLimitRange()
{
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionScrollLimitRange::OnUpdateAction(TbFloat32 deltatime)
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TB_RESQUE(stage);

    const TbRangeF32& playerRange = stage->GetPlayerRangeX();
    switch (m_Phase)
    {
    case 0:
        if (GetPosX() < playerRange.end) {
            stage->SetScrollLimit(GetPosX());
            ++m_Phase;
        }
        break;
    case 1:
        if (GetPosX()+m_Width < playerRange.begin) {
            stage->ResetScrollLimit();
            stage->RequestScroll(GetPosX() + m_Width);
            ++m_Phase;
        }
        break;
    default:
        break;
    }
}

