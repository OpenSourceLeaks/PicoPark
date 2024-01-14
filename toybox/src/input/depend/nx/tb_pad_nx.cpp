/*!
 * @file
 * @brief パッド入力管理クラス
 * @author teco
 */

#include "tb_fwd.h"
#include "input/tb_pad.h"
#include <input/tb_input_setting.h>
#include <base/time/tb_tick.h>

#include <nn/hid/hid_Npad.h>
#include <nn/hid/hid_NpadJoy.h>
#include <nn/hid/hid_NpadFullKey.h>
#include <nn/hid/hid_NpadHandheld.h>

namespace toybox
{

namespace
{

// 使用する Npad の ID を定義
static const nn::hid::NpadIdType NPAD_ID[] =
{
    nn::hid::NpadId::No1,
    nn::hid::NpadId::No2,
    nn::hid::NpadId::No3,
    nn::hid::NpadId::No4,
    nn::hid::NpadId::No5,
    nn::hid::NpadId::No6,
    nn::hid::NpadId::No7,
    nn::hid::NpadId::No8,
};

void updateFullKey( TbPadInfo* info , const nn::hid::NpadButtonSet& buttons )
{
    if (buttons.Test<nn::hid::NpadButton::Plus>() ||
        buttons.Test<nn::hid::NpadButton::Minus>())
    {
        info->now |= TB_PAD_START_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::A>())
    {
        info->now |= TB_PAD_A_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::B>())
    {
        info->now |= TB_PAD_B_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::X>())
    {
        info->now |= TB_PAD_X_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::Y>())
    {
        info->now |= TB_PAD_Y_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::L>())
    {
        info->now |= TB_PAD_L1_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::R>())
    {
        info->now |= TB_PAD_R1_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::Up>())
    {
        info->now |= TB_PAD_UP_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::Down>())
    {
        info->now |= TB_PAD_DOWN_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::Left>())
    {
        info->now |= TB_PAD_LEFT_BIT;
    }
    if (buttons.Test<nn::hid::NpadButton::Right>())
    {
        info->now |= TB_PAD_RIGHT_BIT;
    }
}

}

// depend

/*!
 * コンストラクタ
 * @author teco
 */
TbPadDepend::TbPadDepend()
    : m_PadInfo()
    , m_ConnectedPadCount(0)
{
    setup();
}

/*!
 * デストラクタ
 * @author teco
 */
TbPadDepend::~TbPadDepend()
{
}

/*!
 * 更新処理
 * @author teco
 */
void TbPadDepend::Update()
{
    typedef ::nn::util::BitFlagSet<64, nn::hid::NpadButton>::Flag<24> ButtonLSL;
    typedef ::nn::util::BitFlagSet<64, nn::hid::NpadButton>::Flag<25> ButtonLSR;
    typedef ::nn::util::BitFlagSet<64, nn::hid::NpadButton>::Flag<26> ButtonRSL;
    typedef ::nn::util::BitFlagSet<64, nn::hid::NpadButton>::Flag<27> ButtonRSR;

    // Dual認識したら横持とに強制変換
#if 1
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PadInfo); ++i)
    {
        nn::hid::NpadStyleSet style = nn::hid::GetNpadStyleSet(NPAD_ID[i]);
        if (style.Test<nn::hid::NpadStyleJoyDual>())
        {
            nn::hid::SetNpadJoyAssignmentModeSingle(NPAD_ID[i]);
        }
    }
#endif

    m_ConnectedPadCount = 0;
    // 必要に応じて入力状態を取得します。
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_PadInfo); ++i)
    {
        TbPadInfo& padInfo = m_PadInfo[i];
        padInfo.old = padInfo.now;
        padInfo.now = 0;
        padInfo.repeat = 0;
        nn::hid::NpadIdType padId = NPAD_ID[i];
        nn::hid::NpadStyleSet style;
        nn::hid::AnalogStickState analogStick;

        if ( i == TB_ARRAY_COUNT_OF(m_PadInfo)-1 )
        {
            // 8PはHandHeld接続を許容
            nn::hid::NpadHandheldState state = {};
            nn::hid::GetNpadState(&state, nn::hid::NpadId::Handheld);
            if (state.attributes.Test<nn::hid::NpadAttribute::IsConnected>())
            {
                padId = nn::hid::NpadId::Handheld;
            }
        }

        //現在有効な操作形態(NpadStyleSet)を取得
        style = nn::hid::GetNpadStyleSet(padId);
        if (style.Test<nn::hid::NpadStyleHandheld>())
        {
            nn::hid::NpadHandheldState state = {};
            nn::hid::GetNpadState(&state, padId);
            if (!state.attributes.Test<nn::hid::NpadAttribute::IsConnected>())
            {
                padInfo.flag &= ~TB_BIT(TbPadInfo::FLAG_ACTIVE);
                continue;
            }
            analogStick.y = -state.analogStickL.y;
            analogStick.x = state.analogStickL.x;
            updateFullKey(&padInfo, state.buttons);
        }
        else if (style.Test<nn::hid::NpadStyleJoyLeft>())
        {
            nn::hid::NpadJoyLeftState state = {};
            nn::hid::GetNpadState(&state, padId);
            if (!state.attributes.Test<nn::hid::NpadAttribute::IsConnected>())
            {
                padInfo.flag &= ~TB_BIT(TbPadInfo::FLAG_ACTIVE);
                continue;
            }
            analogStick.x = -state.analogStickL.y;
            analogStick.y = -state.analogStickL.x;
            if (state.buttons.Test<nn::hid::NpadButton::Minus>()) {
                padInfo.now |= TB_PAD_START_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::Down>())
            {
                padInfo.now |= TB_PAD_A_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::Left>())
            {
                padInfo.now |= TB_PAD_B_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::Right>())
            {
                padInfo.now |= TB_PAD_X_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::Up>())
            {
                padInfo.now |= TB_PAD_Y_BIT;
            }
            if (state.buttons.Test<ButtonLSL>())
            {
                padInfo.now |= TB_PAD_L1_BIT;
            }
            if (state.buttons.Test<ButtonLSR>())
            {
                padInfo.now |= TB_PAD_R1_BIT;
            }
        }
        else if (style.Test<nn::hid::NpadStyleJoyRight>())
        {
            nn::hid::NpadJoyRightState state = {};
            nn::hid::GetNpadState(&state, padId);
            if (!state.attributes.Test<nn::hid::NpadAttribute::IsConnected>())
            {
                padInfo.flag &= ~TB_BIT(TbPadInfo::FLAG_ACTIVE);
                continue;
            }
            analogStick.x = state.analogStickR.y;
            analogStick.y = state.analogStickR.x;
            if (state.buttons.Test<nn::hid::NpadButton::Plus>()) {
                padInfo.now |= TB_PAD_START_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::X>())
            {
                padInfo.now |= TB_PAD_A_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::A>())
            {
                padInfo.now |= TB_PAD_B_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::Y>())
            {
                padInfo.now |= TB_PAD_X_BIT;
            }
            if (state.buttons.Test<nn::hid::NpadButton::B>())
            {
                padInfo.now |= TB_PAD_Y_BIT;
            }

            if (state.buttons.Test<ButtonRSL>())
            {
                padInfo.now |= TB_PAD_L1_BIT;
            }
            if (state.buttons.Test<ButtonRSR>())
            {
                padInfo.now |= TB_PAD_R1_BIT;
            }

        }
        else if (style.Test<nn::hid::NpadStyleJoyDual>())
        {
            nn::hid::NpadJoyDualState state = {};
            nn::hid::GetNpadState(&state, padId);
            if (!state.attributes.Test<nn::hid::NpadAttribute::IsConnected>())
            {
                padInfo.flag &= ~TB_BIT(TbPadInfo::FLAG_ACTIVE);
                continue;
            }
            analogStick.y = -state.analogStickL.y;
            analogStick.x = state.analogStickL.x;
            updateFullKey(&padInfo, state.buttons);
        }
        else if (style.Test<nn::hid::NpadStyleFullKey>())
        {
            nn::hid::NpadFullKeyState state = {};
            nn::hid::GetNpadState(&state, padId);
            if (!state.attributes.Test<nn::hid::NpadAttribute::IsConnected>())
            {
                padInfo.flag &= ~TB_BIT(TbPadInfo::FLAG_ACTIVE);
                continue;
            }
            analogStick.y = -state.analogStickL.y;
            analogStick.x = state.analogStickL.x;
            updateFullKey(&padInfo, state.buttons);
        }
        else {
            continue;
        }
        ++m_ConnectedPadCount;
        padInfo.flag |= TB_BIT(TbPadInfo::FLAG_ACTIVE);
        padInfo.leftStick.Set(static_cast<TbFloat32>(analogStick.x) / static_cast<TbFloat32>(nn::hid::AnalogStickMax),
            static_cast<TbFloat32>(analogStick.y) / static_cast<TbFloat32>(nn::hid::AnalogStickMax));
        if (0.5f < padInfo.leftStick.GetX()) {
            padInfo.now |= TB_PAD_RIGHT_BIT;
        }
        else if (padInfo.leftStick.GetX() < -0.5f) {
            padInfo.now |= TB_PAD_LEFT_BIT;
        }
        if (0.5f < padInfo.leftStick.GetY()) {
            padInfo.now |= TB_PAD_DOWN_BIT;
        }
        else if (padInfo.leftStick.GetY() < -0.5f) {
            padInfo.now |= TB_PAD_UP_BIT;
        }

        for (TbSint32 i = 0; i < TB_PAD_BUTTON_MAX; ++i) {
            TbBool isOn = padInfo.now & TB_BIT(i);
            if (isOn) {
                padInfo.now |= TB_BIT(i);
                padInfo.repeatMilliSec[i] += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds());
                TbFloat32 repeatTime = (padInfo.repeatFirst & TB_BIT(i)) ? TbInputSetting::GetRepeatMilliSec() : TbInputSetting::GetRepeatMilliSecFirst();;
                if (!(padInfo.old & TB_BIT(i))) {
                    padInfo.repeat |= TB_BIT(i);
                }
                else if (padInfo.repeatMilliSec[i] > repeatTime) {
                    padInfo.repeat |= TB_BIT(i);
                    padInfo.repeatFirst |= TB_BIT(i);
                    padInfo.repeatMilliSec[i] -= repeatTime;
                }
            }
            else {
                padInfo.repeatMilliSec[i] = 0.0f;
                padInfo.repeatFirst &= ~TB_BIT(i);
            }
        }
    }

    
}

/*!
 * セットアップ
 * @author teco
 */
void TbPadDepend::setup()
{
    nn::hid::InitializeNpad();

    // 使用する操作形態を設定
    nn::hid::SetSupportedNpadStyleSet(nn::hid::NpadStyleJoyLeft::Mask | 
                                      nn::hid::NpadStyleJoyRight::Mask |
                                      nn::hid::NpadStyleJoyDual::Mask |
                                      nn::hid::NpadStyleHandheld::Mask |
                                      nn::hid::NpadStyleFullKey::Mask 
    );

    // 使用する Npad を設定
    nn::hid::NpadIdType ids[TB_ARRAY_COUNT_OF(NPAD_ID)+1];
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(NPAD_ID); ++i)
    {
        ids[i] = NPAD_ID[i];
    }
    ids[TB_ARRAY_COUNT_OF(NPAD_ID)] = nn::hid::NpadId::Handheld;
    nn::hid::SetSupportedNpadIdType(ids, TB_ARRAY_COUNT_OF(ids));
}

//

/*!
 * コンストラクタ
 * @author teco
 */
TbPad::TbPad( TbWindow* window  )
    : m_Depend()
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbPad::~TbPad(void)
{
}

/*!
 * 更新処理
 * @author teco
 */
void TbPad::Update()
{
    m_Depend.Update();
}

/*!
 * 有効なパッド数
 * @author teco
 * @since 2010.10.13
 */
TbSint32 TbPad::getPadCount() const
{
    return m_Depend.GetPadCount();
}

/*!
 * トリガーしたか
 * @author teco
 */
TbBool TbPad::isPress(const TbUint32 buttonBit,const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TB_FALSE;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return ( padInfo->now & buttonBit ) == buttonBit && ( padInfo->old & buttonBit ) == 0;
    }
    return TB_FALSE;
}

/*!
* トリガーしたか
* @author Miyake Shunsuke
*/
TbBool TbPad::isPressAny(const TbUint32 buttonBit, const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TB_FALSE;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[padIndex];
    if (padInfo) {
        TbUint32 onBit = padInfo->now & buttonBit;
        return (padInfo->old & onBit) != onBit;
    }
    return TB_FALSE;
}

/*!
 * 離したか
 * @author teco
 */
TbBool TbPad::isRelease(const TbUint32 buttonBit,const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TB_FALSE;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return ( padInfo->now & buttonBit ) == 0 && ( padInfo->old & buttonBit ) == buttonBit;
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool TbPad::isOn(const TbUint32 buttonBit,const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TB_FALSE;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return ( padInfo->now & buttonBit ) == buttonBit;
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool TbPad::isOnPre(const TbUint32 buttonBit,const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TB_FALSE;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return ( padInfo->old & buttonBit ) == buttonBit;
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool TbPad::isOnAny(const TbUint32 buttonBit,const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TB_FALSE;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return ( padInfo->now & buttonBit );
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool TbPad::isOnRepeat(const TbUint32 buttonBit,const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TB_FALSE;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return ( padInfo->repeat & buttonBit );
    }
    return TB_FALSE;
}

/*!
 * 左アナログスティックの方向取得
 * @author teco
 */
const TbVector2& TbPad::getLeftStick(const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TbVector2::ZERO;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return padInfo->leftStick;
    }
    return TbVector2::ZERO;
}

/*!
 * 右アナログスティックの方向取得
 * @author teco
 */
const TbVector2& TbPad::getRightStick(const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return TbVector2::ZERO;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return padInfo->rightStick;
    }
    return TbVector2::ZERO;
}

/*!
 * 左トリガー取得
 */
const TbUint8 TbPad::getLeftTrigger(const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return 0;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return padInfo->leftTrigger;
    }
    return 0;
}

/*!
 * 右トリガー取得
 */
const TbUint8 TbPad::getRightTrigger(const TbUint32 padIndex) const
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return 0;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return padInfo->rightTrigger;
    }
    return 0;
}

/*!
 * 押されている情報取得
 * @author teco
 */
TbUint32          TbPad::getOn(const TbUint32 padIndex)
{
    if (!(0 <= padIndex&&padIndex < TB_ARRAY_COUNT_OF(m_Depend.m_PadInfo))) {
        return 0;
    }
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return padInfo->now;
    }
    return 0;
}

/*!
 * パッドボタンに、ゲームパッド固有のボタン番号を割り当てる
 * @author teco
 */
TbResult TbPad::setPadButtonSetting( const TbUint32 padIndex , const TbPadButton button , const TbUint32 deviceButton )
{
    TbResult result = TB_E_FAIL;
    return result;
}

/*!
 * パッドボタンに、ゲームパッド固有のボタン番号を割り当てる
 * @author teco
 */
TbResult TbPad::setPadButtonSettingByProductUniqId( const TbUint32 productId , const TbPadButton button , const TbUint32 deviceButton )
{
    TbResult result = TB_S_OK;
    return result;
}


/*!
 * 生のボタン数取得
 * @author teco
 */
TbUint32 TbPad::getRawButtonCount( const TbUint32 padIndex )
{
    TbUint32 result = 0;
    return result;
}

/*!
 * 生のボタン数取得
 * @author teco
 */
TbBool TbPad::isOnRaw( const TbUint32 index , const TbUint32 padIndex )
{
    TbBool result = TB_FALSE;
    return result;
}

/*!
 * 生のボタン数取得
 * @author teco
 */
TbBool TbPad::isPressRaw( const TbUint32 index , const TbUint32 padIndex )
{
    TbBool result = TB_FALSE;
    return result;
}

/*!
 * パッドのユニークID取得
 * @author teco
 */
TbUint64 TbPad::getPadUniqId( const TbUint32 padIndex ) const
{
    return 0;
}

/*!
 * パッドの製品ユニークID取得
 * @author teco
 */
TbUint64 TbPad::getPadProductUniqId( const TbUint32 padIndex ) const
{
    return 0;
}

/*!
 * パッドの製品ユニークID取得
 * @author teco
 */
const char* TbPad::getPadProductName( const TbUint32 padIndex ) const
{
    return "";
}

/*!
 * ユニークIDによってインデックスの値を設定
 * パッドの製品ユニークID取得
 * @author teco
 */
void TbPad::setPadIndexByUniqId( const TbUint32 padIndex , const TbUint64 uniqId)
{
}

/*!
 * 登録されている製品数取得
 * @author teco
 */
TbUint32 TbPad::getPadProductCount() const
{
    return 0;
}

/*!
 * 製品設定取得
 * @author teco
 */
const TbPadButtonSetting* TbPad::getPadProductSetting( TbUint32 productIndex )
{
    return nullptr;
}


}