/*!
* @file
* @brief キーボード入力管理クラス
* @author teco
*/

#include "tb_fwd.h"
#include "base/time/tb_tick.h"
#include "input/tb_keyboard.h"
#include "input/tb_input_setting.h"

#include <nn/hid/hid_Keyboard.h>

namespace toybox
{

    /*!
    * コンストラクタ
    * @author teco
    */
    TbKeyboardDepend::TbKeyboardDepend(const TbKeyboardParameter* parameter, const TbSint32 count)
    {
        nn::hid::InitializeKeyboard();
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
        TbBool buf[TB_KEY_MAX];
        TbFillZero(buf);

        // 2. 必要に応じて入力状態を取得します。
        nn::hid::KeyboardState state = {};
        nn::hid::GetKeyboardState(&state);
        if (state.keys.Test<nn::hid::KeyboardKey::A>()){
            buf[TB_KEY_A] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::B>()) {
            buf[TB_KEY_B] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::C>()) {
            buf[TB_KEY_C] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D>()) {
            buf[TB_KEY_D] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::E>()) {
            buf[TB_KEY_E] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::F>()) {
            buf[TB_KEY_F] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::G>()) {
            buf[TB_KEY_G] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::H>()) {
            buf[TB_KEY_H] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::I>()) {
            buf[TB_KEY_I] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::J>()) {
            buf[TB_KEY_J] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::K>()) {
            buf[TB_KEY_K] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::L>()) {
            buf[TB_KEY_L] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::M>()) {
            buf[TB_KEY_M] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::N>()) {
            buf[TB_KEY_N] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::O>()) {
            buf[TB_KEY_O] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::P>()) {
            buf[TB_KEY_P] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::Q>()) {
            buf[TB_KEY_Q] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::R>()) {
            buf[TB_KEY_R] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::S>()) {
            buf[TB_KEY_S] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::T>()) {
            buf[TB_KEY_T] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::U>()) {
            buf[TB_KEY_U] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::V>()) {
            buf[TB_KEY_V] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::W>()) {
            buf[TB_KEY_W] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::X>()) {
            buf[TB_KEY_X] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::Y>()) {
            buf[TB_KEY_Y] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::Z>()) {
            buf[TB_KEY_Z] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::Return>()) {
            buf[TB_KEY_RETURN] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::Space>()) {
            buf[TB_KEY_SPACE] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::LeftShift>()) {
            buf[TB_KEY_SHIFT_L] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::RightShift>()) {
            buf[TB_KEY_SHIFT_R] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::LeftControl>()) {
            buf[TB_KEY_CONTROL_L] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::RightControl>()) {
            buf[TB_KEY_CONTROL_R] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::Backspace>()) {
            buf[TB_KEY_BACK] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::UpArrow>()) {
            buf[TB_KEY_UP] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::DownArrow>()) {
            buf[TB_KEY_DOWN] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::LeftArrow>()) {
            buf[TB_KEY_LEFT] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::RightArrow>()) {
            buf[TB_KEY_RIGHT] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D0>()) {
            buf[TB_KEY_0] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D1>()) {
            buf[TB_KEY_1] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D2>()) {
            buf[TB_KEY_2] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D3>()) {
            buf[TB_KEY_3] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D4>()) {
            buf[TB_KEY_4] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D5>()) {
            buf[TB_KEY_5] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D6>()) {
            buf[TB_KEY_6] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D7>()) {
            buf[TB_KEY_7] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D8>()) {
            buf[TB_KEY_8] = TB_TRUE;
        }
        if (state.keys.Test<nn::hid::KeyboardKey::D9>()) {
            buf[TB_KEY_9] = TB_TRUE;
        }

        m_Info.keyIndex = !m_Info.keyIndex;
        for (TbSint32 i = 0; i < TB_KEY_MAX; ++i) {
            m_Info.keyBuf[m_Info.keyIndex][i] = buf[i];
            m_Info.keyRepeat[i] &= TbKeyboardDepend::Info::KEY_REPEAT_BIT_FIRST;
            TbFloat32 repeatTime = (m_Info.keyRepeat[i] & TbKeyboardDepend::Info::KEY_REPEAT_BIT_FIRST) ? TbInputSetting::GetRepeatMilliSec() : TbInputSetting::GetRepeatMilliSecFirst();
            if (m_Info.keyBuf[m_Info.keyIndex][i]) {
                m_Info.keyRepeatTimer[i] += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds());
                if (!m_Info.keyBuf[!m_Info.keyIndex][i]) {
                    m_Info.keyRepeat[i] |= TbKeyboardDepend::Info::KEY_REPEAT_BIT_CURRENT;
                }
                else if (repeatTime < m_Info.keyRepeatTimer[i]) {
                    m_Info.keyRepeat[i] |= TbKeyboardDepend::Info::KEY_REPEAT_BIT_CURRENT | TbKeyboardDepend::Info::KEY_REPEAT_BIT_FIRST;
                    m_Info.keyRepeatTimer[i] -= repeatTime;
                }
            }
            else {
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
    TbKeyboard::TbKeyboard(const TbKeyboardParameter* parameter, const TbSint32 count)
        : m_Depend(parameter, count)
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
    TbBool TbKeyboard::isPress(const TbKeyType keyType, const TbUint32 index) const
    {
        if (0 == index && 0 <= keyType && keyType < TB_KEY_MAX) {
            const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
            TB_ASSERT(info);
            if (info) {
                return info->keyBuf[info->keyIndex][keyType] && !info->keyBuf[!info->keyIndex][keyType];
            }
        }
        return TB_FALSE;
    }

    /*!
    * 離したか
    * @author teco
    */
    TbBool TbKeyboard::isRelease(const TbKeyType keyType, const TbUint32 index) const
    {
        if (0 == index && 0 <= keyType && keyType < TB_KEY_MAX) {
            const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
            TB_ASSERT(info);
            if (info) {
                return info->keyBuf[!info->keyIndex][keyType] && !info->keyBuf[info->keyIndex][keyType];
            }
        }
        return TB_FALSE;
    }

    /*!
    * 押しているか
    * @author teco
    */
    TbBool TbKeyboard::isOn(const TbKeyType keyType, const TbUint32 index) const
    {
        if (0 == index && 0 <= keyType && keyType < TB_KEY_MAX) {
            const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
            TB_ASSERT(info);
            if (info) {
                return info->keyBuf[info->keyIndex][keyType];
            }
        }
        return TB_FALSE;
    }

    /*!
    * 押しているか(リピート)
    * @author teco
    */
    TbBool TbKeyboard::isOnRepeat(const TbKeyType keyType, const TbUint32 index) const
    {
        if (0 == index && 0 <= keyType && keyType < TB_KEY_MAX) {
            const TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
            TB_ASSERT(info);
            if (info) {
                return (info->keyRepeat[keyType] & TbKeyboardDepend::Info::KEY_REPEAT_BIT_CURRENT) > 0;
            }
        }
        return TB_FALSE;
    }

    /*!
    * リピートリセット
    * @author teco
    */
    void TbKeyboard::resetRepeat(const TbKeyType keyType, const TbUint32 index)
    {
        if (0 == index && 0 <= keyType && keyType < TB_KEY_MAX) {
            TbKeyboardDepend::Info* info = &m_Depend.GetInfo();
            if (TB_VERIFY(info)) {
                if (info->keyBuf[keyType]) {
                    info->keyRepeat[keyType] = 0;
                    info->keyRepeatTimer[keyType] = 0.0f;
                }
            }
        }
    }

}