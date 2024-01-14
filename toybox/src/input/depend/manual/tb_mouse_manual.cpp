/*!
 * @file
 * @brief パッド入力管理クラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "input/tb_mouse.h"
#include "base/io/tb_print.h"
#include <graphics/window/tb_window.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbMouseDepend::TbMouseDepend( TbMouseParameter* parameter , TbSint32 count )
{
    setup( parameter );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbMouseDepend::~TbMouseDepend(void) 
{
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 * @since 2010.10.04
 */
TbResult TbMouseDepend::setup( TbMouseParameter* parameter )
{
    if( !parameter ){
        return TB_E_FAIL;
    }
    m_MouseInfo.window = parameter->window;
    return TB_S_OK;
}

// 

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbMouse::TbMouse( TbMouseParameter* parameter , TbSint32 count )
    : m_Depend( parameter , count )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbMouse::~TbMouse(void)
{
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
void TbMouse::Update()
{
   TbMouseDepend::Info* info = m_Depend.getInfo(0);
   info->prePos = info->pos;
   info->pos = info->futurePos;
   info->old = info->now;
   info->now = info->future;
}

/*!
 * 有効なマウス数
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbUint32 TbMouse::getCount() const
{
    return 1;
}

/*! 
 * トリガーしたか
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbBool TbMouse::isPress(const TbUint32 buttonBit,const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        return ( info->now & buttonBit ) == buttonBit && ( info->old & buttonBit ) == 0;
    }
    return TB_FALSE;
}

/*! 
 * どれかトリガーしたか
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbBool TbMouse::isPressAny(const TbUint32 buttonBit,const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        return ( ( info->now ^ info->old ) & info->now ) & buttonBit;
    }
    return TB_FALSE;
}

/*!
 * 離したか
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbBool TbMouse::isRelease(const TbUint32 buttonBit,const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        return ( info->now & buttonBit ) == 0 && ( info->old & buttonBit ) == buttonBit;
    }
    return TB_FALSE;
}

/*!
 * どれか離したか
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbBool TbMouse::isReleaseAny(const TbUint32 buttonBit,const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        return ( ( info->now ^ info->old ) & info->old ) & buttonBit;
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbBool TbMouse::isOn(const TbUint32 buttonBit,const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        return ( info->now & buttonBit ) == buttonBit;
    }
    return TB_FALSE;
}

/*!
 * どれか押しているか
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbBool TbMouse::isOnAny(const TbUint32 buttonBit,const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        return !!( info->now & buttonBit );
    }
    return TB_FALSE;
}

/*!
 * 押されている情報取得
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbUint32 TbMouse::getOn(const TbUint32 index)
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        return info->now;
    }
    return 0;
}

/*!
 * 位置
 * @author Miyake Shunsuke
 * @since 2011.02.24
 */
TbResult TbMouse::getPos( TbVector2* outPos,const TbMousePosType posType,const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ){
        switch( posType )
        {
        case TB_MOUSE_POS_TYPE_GLOBAL:
            TB_ASSERT(0);
            break;
        case TB_MOUSE_POS_TYPE_WINDOW:
            *outPos = info->pos;
            break;
        case TB_MOUSE_POS_TYPE_WINDOW_RATIO:
            outPos->Clear();
            if( 0 != info->window->GetWidth() && 0 != info->window->GetHeight() )
            {
                outPos->Set( info->pos.GetX() / static_cast<TbFloat32>(info->window->GetWidth()) ,
                             info->pos.GetY() / static_cast<TbFloat32>(info->window->GetHeight()) );
            }
            break;
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
}
    
/*!
 * 動いているか
 * @author teco
 */
TbBool TbMouse::isMoving(const TbUint32 index , TbVector2* outMov ) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if( info ) {
        if (info->prePos != info->pos) {
            if (outMov) {
                TbVector2 mov = info->pos - info->prePos;
                outMov->Set( mov.GetX() , mov.GetY() );
            }
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
* ホイールの移動量取得
* @author teco
*/
TbSint32 TbMouse::getWheelDelta(const TbUint32 index) const
{
    const TbMouseDepend::Info* info = m_Depend.getInfo(index);
    if (info) {
    //    return info->wheelDelta;
    }
    return 0;
}


}