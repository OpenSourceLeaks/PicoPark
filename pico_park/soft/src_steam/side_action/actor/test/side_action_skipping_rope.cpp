/*!
 * 縄跳び
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_skipping_rope.h"
#include "side_action/stage/side_action_stage.h"

#include <input/tb_pad.h>
#include <input/tb_input_setting.h>

#include <base/math/tb_geometry_util.h>

static const TbSint32 PIXEL_SIZE = 8;

static TbSint32 HEIGHT_LIMIT = 10;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.28
 */
SideActionSkippingRope::SideActionSkippingRope()
    : Super()
{
    m_SpriteHooker.SetDrawFunc( TbCreateMemFunc(this,&Self::draw) );
    m_Start.x = 0;
    m_Start.y = 0;
    m_End.x = 10;
    m_End.y = 10;
    TbInputSetting::SetRepeatMilliSecFirst(66.0f);
    TbInputSetting::SetRepeatMilliSec(66.0f);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.28
 */
SideActionSkippingRope::~SideActionSkippingRope()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.28
 */
void SideActionSkippingRope::OnUpdate( TbFloat32 deltatime )
{
    if( TbPad::IsOn(TB_PAD_L1_BIT) ){
        if( TbPad::IsOnRepeat(TB_PAD_LEFT_BIT) ){
            m_Start.x -= 1;
        } else if( TbPad::IsOnRepeat(TB_PAD_RIGHT_BIT) ){
            m_Start.x += 1;
        }
        if( TbPad::IsOnRepeat(TB_PAD_UP_BIT) ){
            m_Start.y -= 1;
        } else if( TbPad::IsOnRepeat(TB_PAD_DOWN_BIT) ){
            m_Start.y += 1;
        }
    }
    if( TbPad::IsOn(TB_PAD_R1_BIT) ){
        if( TbPad::IsOnRepeat(TB_PAD_LEFT_BIT) ){
            m_End.x -= 1;
        } else if( TbPad::IsOnRepeat(TB_PAD_RIGHT_BIT) ){
            m_End.x += 1;
        }
        if( TbPad::IsOnRepeat(TB_PAD_UP_BIT) ){
            m_End.y -= 1;
        } else if( TbPad::IsOnRepeat(TB_PAD_DOWN_BIT) ){
            m_End.y += 1;
        }
    }
    if( TbPad::IsOn(TB_PAD_B_BIT) ){
        if( TbPad::IsOnRepeat(TB_PAD_UP_BIT) ){
            HEIGHT_LIMIT -= 1;
        } else if( TbPad::IsOnRepeat(TB_PAD_DOWN_BIT) ){
            HEIGHT_LIMIT += 1;
        }
    }
}
    
/*!
 * シーンに入った
 * @author teco
 * @since 2013.09.28
 */
void SideActionSkippingRope::OnEnter( void* userData )
{
    SideActionStage* stage = reinterpret_cast<SideActionStage*>(userData);
    if( TB_VERIFY(stage) ) {
        m_SpriteHooker.Enter( &stage->GetSpriteScene() );
    }
}
    
/*!
 * シーンから出た
 * @author teco
 * @since 2013.09.28
 */
void SideActionSkippingRope::OnExit()
{
    m_SpriteHooker.Exit();
}

/*!
 * 描画
 * @author teco
 * @since 2013.09.28
 */
void SideActionSkippingRope::draw( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    static TbPointS32 buffer[1000];
    TbUint32 count = TbGetEllipsePointArray(buffer,
                                            TB_ARRAY_COUNT_OF(buffer),
                                            m_Start,
                                            m_End.x,
                                            m_End.y,
                                            m_End.y >= 0 ? -(TB_PI_ANG32>>1) : (TB_PI_ANG32>>1),
                                            m_End.y >= 0 ? TB_PI_ANG32>>1 : TB_PI_ANG32+(TB_PI_ANG32>>1),
                                            m_End.x,
                                            HEIGHT_LIMIT
                                            );
    for( TbUint32 i = 0; i < count; ++i ){
        TbVector4 pos = GetPos();
        pos.Add( static_cast<TbFloat32>(buffer[i].x * PIXEL_SIZE) ,
                 static_cast<TbFloat32>(buffer[i].y * PIXEL_SIZE) ,
                 0.0f );
        TbRectF32 rect = TbRectF32( pos.GetX() , 
                                    pos.GetY() , 
                                    static_cast<TbFloat32>(PIXEL_SIZE) , 
                                    static_cast<TbFloat32>(PIXEL_SIZE) );
        TbRectF32 uvRect;
        material->Draw( rect , uvRect , TbColor::Blue() , TbMatrix::IDENTITY );
    }
//    TbUint32 count = TbGetLine

}
