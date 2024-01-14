/*!
 * タッチ入力管理クラス
 * @author teco
 */

#include "tb_fwd.h"
#include "input/tb_touch.h"
#include <base/io/tb_print.h>
#include <base/io/tb_log.h>
#include <base/math/tb_math_util.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbTouchDepend::TbTouchDepend( const TbTouchParameter& parameter )
    : m_TouchInfo()
{
    setup( parameter );
}

/*!
 * デストラクタ
 * @author teco
 */
TbTouchDepend::~TbTouchDepend(void) 
{
}

/*!
 * セットアップ
 * @author teco
 */
TbResult TbTouchDepend::setup( const TbTouchParameter& parameter )
{
    m_TouchInfo = Info();
    TB_ASSERT( parameter.window );
    m_TouchInfo.window = parameter.window;
    return TB_S_OK;
}

// 

/*!
 * コンストラクタ
 * @author teco
 */
TbTouch::TbTouch( TbTouchParameter* parameter , TbSint32 count )
    : m_Depend( *parameter )
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbTouch::~TbTouch(void)
{
}

/*!
 * 更新
 * @author teco
 */
void TbTouch::Update()
{
#if 1
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Depend.m_TouchInfo.pointInfo); ++i)
    {
       TbTouchDepend::PointInfo& info = m_Depend.m_TouchInfo.pointInfo[i];
       info.pos = info.futurePos;
       info.isPress = info.isFirst;
       info.isFirst = TB_FALSE;
    }
#endif
}

/*!
 * 有効なタッチ数
 * @author teco
 */
TbUint32 TbTouch::getCount() const
{
    return m_Depend.getCount();
}

/*! 
 * トリガーしたか
 * @author teco
 */
TbBool TbTouch::isPress(const TbUint32 index) const
{
    if( getCount() <= index ) {
        return TB_FALSE;
    }
    return m_Depend.m_TouchInfo.pointInfo[index].isPress;
}

/*!
 * 離したか
 * @author teco
 */
TbBool TbTouch::isRelease(const TbUint32 index) const
{
    TB_ASSERT(0);
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool TbTouch::isOn(const TbUint32 index) const
{
    if( getCount() <= index ) {
        return TB_FALSE;
    }
    return 0 <= m_Depend.m_TouchInfo.pointInfo[index].id;
}

/*!
 * 位置
 * @author teco
 */
TbResult TbTouch::getPos( TbVector2* outPos,const TbTouchPosType posType,const TbUint32 index) const
{
    if( getCount() <= index ) {
        return TB_E_FAIL;
    }
    if( !IsOn(index) ){
        return TB_E_FAIL;
    }
#if 1
    const TbTouchDepend::PointInfo& info = m_Depend.m_TouchInfo.pointInfo[index];
    switch( posType )
    {
    case TB_TOUCH_POS_TYPE_GLOBAL:
        break;
    case TB_TOUCH_POS_TYPE_WINDOW:
        break;
    case TB_TOUCH_POS_TYPE_WINDOW_RATIO:
        break;
    }
    return TB_S_OK;
#endif
}

}