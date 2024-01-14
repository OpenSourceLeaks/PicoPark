/*!
 * アクター
 * @author teco
 * @since 2013.09.04
 */

#include "tb_fwd.h"
#include "crayon/actor/cr_sprite_actor.h"

using namespace toybox;

namespace crayon
{

/*!
 * コンストラクタ
 * @author teco
 */
CrSpriteActor::CrSpriteActor()
    : Super()
{
    SetEnableResetOffsetDrawPos(TB_TRUE);
    m_Components.SetCapacity(5);
    m_Components.SetEnableSetCapacityAuto(TB_TRUE);
    AddComponent(&m_Sprite);
    SetEnableScroll(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 */
CrSpriteActor::~CrSpriteActor()
{
}

/*!
 * シーンに入った
 * @author teco
 */
void CrSpriteActor::OnEnter( void* userData )
{
    CrStage* stage = GetStage();
    if( TB_VERIFY(stage) ) {
        TbVector2 pos( GetPos().GetX() , GetPos().GetY() );
        TbVector2 drawPos = GetDrawPos();
        TbForIterator( it , m_Components ){
            (*it)->OnEnter( stage , pos , 0.0f );
            (*it)->OnUpdatePos(pos,drawPos);
       }
    }
}

/*! 
 * シーンから出た
 * @author teco
 */
void CrSpriteActor::OnExit()
{
    TbForIterator( it , m_Components ){
        (*it)->OnExit();
    }
}

/*!
 * 描画位置
 * @author teco
 */
TbVector2 CrSpriteActor::GetDrawPos()
{
    TbVector2 result(GetPosX(),GetPosY());
    result += m_OffsetDrawPos;
    if( IsEnableScroll() )
    {
        CrStage* stage = GetStage();
        if( stage ) {
            result.Add(-stage->GetScrollX(),-stage->GetScrollY());
        }
    }
    return result;
}

}