/*!
 * @file
 * @brief パッド入力管理クラス
 * @author teco
 */

#include "tb_fwd.h"
#include "input/tb_pad.h"

namespace toybox
{

namespace
{

static const TbUint32 BUTTON_BIT_TABLE[] =
{
    KPAD_BUTTON_UP ,  // TB_PAD_UP
    KPAD_BUTTON_DOWN , // TB_PAD_DOWN
    KPAD_BUTTON_LEFT , // TB_PAD_LEFT
    KPAD_BUTTON_RIGHT , // TB_PAD_RIGHT
    KPAD_BUTTON_A , // TB_PAD_START
    KPAD_BUTTON_B , // TB_PAD_SELECT
    KPAD_BUTTON_1 ,  // TB_PAD_A
    KPAD_BUTTON_2 ,  // TB_PAD_B
    0 ,  // TB_PAD_X
    0 ,  // TB_PAD_Y
    0 ,  // TB_PAD_L1
    0 ,  // TB_PAD_R1
    0 ,  // TB_PAD_L2
    0    // TB_PAD_R2
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(BUTTON_BIT_TABLE)==TB_PAD_BUTTON_MAX);

}

// depend

/*!
 * コンストラクタ
 * @author teco
 */
TbPadDepend::TbPadDepend()
    : m_PadInfo()
    , m_PadCount(0)
{
    setup();
}

/*!
 * デストラクタ
 * @author teco
 */
TbPadDepend::~TbPadDepend()
{
    KPADShutdown() ;
}

/*!
 * 更新処理
 * @author teco
 */
void TbPadDepend::Update()
{
    s32 err = KPAD_READ_ERR_NO_DATA;
    for( TbUint32 i = 0; i < WPAD_MAX_CONTROLLERS; ++i )
    {
        TbSint32 readCount = KPADReadEx( i, &m_KPads[i][ 0 ], TB_ARRAY_COUNT_OF(m_KPads[i]) , &err ) ;
        if ( readCount == 0 && err == KPAD_READ_ERR_NO_DATA ) {
            // 最後のデータだけは保持されている
            readCount = 1 ;
        }
        if( 0 < readCount ){
            m_PadInfo[i].now = 0;
            for( TbSint32 i = 0; i < TB_PAD_BUTTON_MAX ; ++i ){
                TbPadInfo& padInfo = m_PadInfo[i];
                
                if( m_KPads[i][ 0 ].hold & BUTTON_BIT_TABLE[i] ) {
                    TbBool isOn = TB_TRUE;
                    if( isOn ){
                        padInfo.now |= TB_BIT( i );
                        padInfo.repeatMilliSec[ i ] += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds());
                        TbFloat32 repeatTime = ( padInfo.repeatFirst & TB_BIT( i ) ) ? TbInputSetting::GetRepeatMilliSec() : TbInputSetting::GetRepeatMilliSecFirst();;
                        if( !( padInfo.old & TB_BIT( i ) ) ){
                            padInfo.repeat |= TB_BIT( i );
                        }else if( padInfo.repeatMilliSec[ i ] > repeatTime ){
                            padInfo.repeat |= TB_BIT( i );
                            padInfo.repeatFirst |= TB_BIT( i );
                            padInfo.repeatMilliSec[ i ] -= repeatTime;
                        }
                    } else {
                        padInfo.repeatMilliSec[ i ] = 0.0f;
                        padInfo.repeatFirst &= ~TB_BIT(i);
                    }
                }
            }
        }else{
            m_PadInfo[i].now = 0;
        }
    }
}

/*!
 * セットアップ
 * @author teco
 */
void TbPadDepend::setup()
{
    KPADInit() ;            // コントローラ
    KPADSetSamplingCallback( 0, samplingCallback ) ;

    //----- コントローラ初期設定
    KPADReset() ;
    KPADSetBtnRepeat( 0, repeat_delay_sec, repeat_pulse_sec ) ;

    WPADEnableURCC( TRUE );
    
    for( TbUint32 i = 0; i < WPAD_MAX_CONTROLLERS; ++i )
    {
        KPADSetButtonProcMode( i, KPAD_BUTTON_PROC_MODE_LOOSE ) 
    }
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
    const TbPadInfo* padInfo = &m_Depend.m_PadInfo[ padIndex ];
    if( padInfo ){
        return ( padInfo->now & buttonBit ) == buttonBit && ( padInfo->old & buttonBit ) == 0;
    }
    return TB_FALSE;
}

/*!
 * 離したか
 * @author teco
 */
TbBool TbPad::isRelease(const TbUint32 buttonBit,const TbUint32 padIndex) const
{
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
    return 0;
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