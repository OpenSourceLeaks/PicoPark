/*!
 * スクロールレンジ
 * @author teco
 */

#include "fwd.h"
#include "side_action_scroll_range.h"
#include "side_action_key.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionScrollLimitRange::SideActionScrollLimitRange( TbFloat32 width , TbBool isTriggerLeft )
    : m_Width(width)
    , m_Phase(0)
    , m_IsTriggerLeft(isTriggerLeft)
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
        {
            TbBool start = TB_FALSE;
            if (m_IsTriggerLeft)
            {
                if (GetPosX() < playerRange.begin) {
                    start = TB_TRUE;
                }
            }
            else
            {
                if (GetPosX() < playerRange.end) {
                    start = TB_TRUE;
                }
            }
            if (start) {
                stage->SetScrollType(SideActionStage::SCROLL_AUTO);
                ++m_Phase;
            }
        }
        break;
    case 1:
        {
            CrActor* actor = GetActionStage()->FindActor("Key");
            if (actor)
            {
                SideActionKey* key = TbDynamicCast<SideActionKey*>(actor);
                if (key && key->IsPlayerAttached()) {
                    stage->SetScrollType(SideActionStage::SCROLL_NORMAL);
                    ++m_Phase;
                }
            }
        }
        break;
    default:
        break;
    }
}

