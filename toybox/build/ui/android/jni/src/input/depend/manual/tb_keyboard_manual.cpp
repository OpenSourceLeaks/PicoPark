/*!
 * @file
 * @brief キーボード入力管理クラス
 * @author teco
 */

#include "tb_fwd.h"
#include "base/time/tb_tick.h"
#include "input/tb_keyboard.h"
#include "input/tb_input_setting.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbKeyboardDepend::TbKeyboardDepend( const TbKeyboardParameter* parameter , const TbSint32 count )
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbKeyboardDepend::~TbKeyboardDepend(void) 
{
}

/*!
 * 更新
 * @author teco
 */
void TbKeyboardDepend::update()
{
    m_Info.keyIndex = !m_Info.keyIndex;
    for( TbSint32 i = 0; i < TB_KEY_MAX; ++i ){
        m_Info.keyBuf[m_Info.keyIndex][i] = m_Info.buffer[ i ] ? 1U : 0U;
        m_Info.keyRepeat[i] &= TbKeyboardDepend::Info::KEY_REPEAT_BIT_FIRST;
        TbFloat32 repeatTime = ( m_Info.keyRepeat[i] & TbKeyboardDepend::Info::KEY_REPEAT_BIT_FIRST ) ? TbInputSetting::GetRepeatMilliSec() : TbInputSetting::GetRepeatMilliSecFirst();
        if( m_Info.keyBuf[m_Info.keyIndex][i] ){
            m_Info.keyRepeatTimer[i] += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds());
            if( !m_Info.keyBuf[!m_Info.keyIndex][i] ){
                m_Info.keyRepeat[i] |= TbKeyboardDepend::Info::KEY_REPEAT_BIT_CURRENT;
            }else if( repeatTime < m_Info.keyRepeatTimer[ i ] ){
                m_Info.keyRepeat[i] |= TbKeyboardDepend::Info::KEY_REPEAT_BIT_CURRENT | TbKeyboardDepend::Info::KEY_REPEAT_BIT_FIRST;
                m_Info.keyRepeatTimer[ i ] -= repeatTime;
            }
        }else{
            m_Info.keyRepeatTimer[i] = 0.0f;
            m_Info.keyRepeat[i] = 0;
        }
    }
}

// 

/*!
 * コンストラクタ
 * @author teco
 */
TbKeyboard::TbKeyboard( const TbKeyboardParameter* parameter , const TbSint32 count )
    : m_Depend( parameter , count )
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbKeyboard::~TbKeyboard(void)
{
}

/*!
 * 更新
 * @author teco
 */
void TbKeyboard::Update()
{
    m_Depend.update();
}

/*!
 * 有効なマウス数
 * @author teco
 */
TbSint32 TbKeyboard::getCount() const
{
    return 1;
}

/*! 
 * トリガーしたか
 * @author teco
 */
TbBool TbKeyboard::isPress(const TbKeyType keyType,const TbUint32 index) const
{
    if( 0 == index && 0 <= keyType && keyType < TB_KEY_MAX ){
        const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
        TB_ASSERT( info );
        if( info ){
            return info->keyBuf[info->keyIndex][ keyType ] && !info->keyBuf[!info->keyIndex][ keyType ];
        }
    }
    return TB_FALSE;
}

/*!
 * 離したか
 * @author teco
 */
TbBool TbKeyboard::isRelease(const TbKeyType keyType,const TbUint32 index) const
{
    if( 0 == index && 0 <= keyType && keyType < TB_KEY_MAX ){
        const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
        TB_ASSERT( info );
        if( info ){
            return info->keyBuf[!info->keyIndex][ keyType ] && !info->keyBuf[info->keyIndex][ keyType ];
        }
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool TbKeyboard::isOn(const TbKeyType keyType,const TbUint32 index) const
{
    if( 0 == index && 0 <= keyType && keyType < TB_KEY_MAX ){
        const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
        TB_ASSERT( info );
        if( info ){
            return info->keyBuf[info->keyIndex][ keyType ];
        }
    }
    return TB_FALSE;
}

/*!
 * 押しているか(リピート)
 * @author teco
 */
TbBool TbKeyboard::isOnRepeat(const TbKeyType keyType,const TbUint32 index) const
{
    if( 0 == index && 0 <= keyType && keyType < TB_KEY_MAX ){
        const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
        TB_ASSERT( info );
        if( info ){
            return ( info->keyRepeat[keyType] & TbKeyboardDepend::Info::KEY_REPEAT_BIT_CURRENT ) > 0;
        }
    }
    return TB_FALSE;
}

/*!
 * リピートリセット
 * @author teco
 */
void TbKeyboard::resetRepeat(const TbKeyType keyType,const TbUint32 index)
{
    if( 0 == index && 0 <= keyType && keyType < TB_KEY_MAX ){
        TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
        if( TB_VERIFY(info) ){
            if( info->keyBuf[keyType] ) {
                info->keyRepeat[keyType] = 0;
                info->keyRepeatTimer[keyType] = 0.0f;
            }
        }
    }
}

}