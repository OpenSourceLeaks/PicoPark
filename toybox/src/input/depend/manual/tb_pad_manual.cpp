/*!
 * @file
 * @brief パッド入力管理クラス
 * @author teco
 */

#include "tb_fwd.h"
#include "input/tb_pad.h"

namespace toybox
{

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
}

/*!
 * 更新処理
 * @author teco
 */
void TbPadDepend::Update()
{
}

/*!
 * セットアップ
 * @author teco
 */
void TbPadDepend::setup()
{
}
/*!
 * パッド情報取得
 * @author teco
 */
const TbPadInfo* TbPadDepend::getPadInfo( TbUint32 padIndex ) const
{
    if( padIndex < m_PadCount ){
        return m_PadInfo[ padIndex ].padInfo;
    }
    return nullptr;
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
    if( padInfo ){
        return padInfo->now;
    }
    return 0;
}

/*!
 * パッドボタンに、ゲームパッド固有のボタン番号を割り当てる
 * @author teco
 * @since 2011.08.28
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
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
    if( padInfo ){
        return padInfo->guid;
    }
    return 0;
}

/*!
 * パッドの製品ユニークID取得
 * @author teco
 */
TbUint64 TbPad::getPadProductUniqId( const TbUint32 padIndex ) const
{
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
    if( padInfo ){
        return padInfo->productGuid;
    }
    return 0;
}

/*!
 * パッドの製品ユニークID取得
 * @author teco
 */
const char* TbPad::getPadProductName( const TbUint32 padIndex ) const
{
    const TbPadInfo* padInfo = m_Depend.getPadInfo( padIndex );
    if( padInfo ){
        return padInfo->productName.GetBuffer();
    }
    return 0;
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