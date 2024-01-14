/*!
 * @file
 * @brief パッド入力
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_PAD_H_
#define _INCLDUED_TB_PAD_H_

#include <base/util/tb_singleton.h>
#include <input/tb_pad_types.h>

#include "./depend/tb_pad_depend.h"

namespace toybox
{
    
class TbWindow;

/*!
 * ゲームパッドクラス
 * @author Miyake Shunsuke
 */
class TbPad : public TbSingletonRef<TbPad>
{
public:

    //コンストラクタ
    explicit TbPad( TbWindow* window );

    //デストラクタ
    virtual ~TbPad(void);

public:

    //更新
    void Update();

public:

    // 有効なパッド数（接続済みかどうかは別）
    static TbSint32 GetPadCount() { 
        return GetInstance().getPadCount(); 
    }
    
    //トリガーしたか
    static TbBool IsPress(const TbUint32 buttonBit,const TbUint32 padIndex=0){
        return GetInstance().isPress(buttonBit,padIndex);
    }
    
    //どれか押しているか
    static TbBool IsPressAny(const TbUint32 buttonBit,const TbUint32 padIndex=0){
        return GetInstance().isPressAny(buttonBit,padIndex);
    }

    //離したか
    static TbBool IsRelease(const TbUint32 buttonBit,const TbUint32 padIndex=0){
        return GetInstance().isRelease(buttonBit,padIndex);
    }
    
    //押しているか
    static TbBool IsOn(const TbUint32 buttonBit,const TbUint32 padIndex=0){
        return GetInstance().isOn(buttonBit,padIndex);
    }
    
    //押しているか
    static TbBool IsOnPre(const TbUint32 buttonBit,const TbUint32 padIndex=0){
        return GetInstance().isOnPre(buttonBit,padIndex);
    }

    //どれか押しているか
    static TbBool IsOnAny(const TbUint32 buttonBit,const TbUint32 padIndex=0){
        return GetInstance().isOnAny(buttonBit,padIndex);
    }
    
    //押しているか
    static TbBool IsOnRepeat(const TbUint32 buttonBit,const TbUint32 padIndex=0){
        return GetInstance().isOnRepeat(buttonBit,padIndex);
    }
    
    //押されているボタン情報
    static TbBool GetOn(const TbUint32 padIndex=0){
        return GetInstance().getOn(padIndex);
    }
    
    //左アナログスティックの方向取得
    static const TbVector2& GetLeftStick(const TbUint32 padIndex=0){
        return GetInstance().getLeftStick(padIndex);
    }
    
    //右アナログスティックの方向取得
    static const TbVector2& GetRightStick(const TbUint32 padIndex=0){
        return GetInstance().getRightStick(padIndex);
    }
    
    //左トリガー取得
    static const TbUint8 GetLeftTrigger(const TbUint32 padIndex=0){
        return GetInstance().getLeftTrigger(padIndex);
    }
    
    //右トリガー取得
    static const TbUint8 GetRightTrigger(const TbUint32 padIndex=0){
        return GetInstance().getRightTrigger(padIndex);
    }
    
    // パッドボタンに、ゲームパッド固有のボタン番号を割り当てる
    static TbResult SetPadButtonSetting( const TbUint32 padIndex , const TbPadButton button , const TbUint32 deviceButton ){
        return GetInstance().setPadButtonSetting(padIndex,button,deviceButton);
    }
    
    // パッドボタンに、ゲームパッド固有のボタン番号を割り当てる
    static TbResult SetPadButtonSettingByProductUniqId( const TbUint32 productUniqId , const TbPadButton button , const TbUint32 deviceButton ) {
        return GetInstance().setPadButtonSettingByProductUniqId(productUniqId,button,deviceButton);
    }

    // 生のボタン数取得
    static TbUint32 GetRawButtonCount( const TbUint32 padIndex ){
        return GetInstance().getRawButtonCount(padIndex);
    }

    // 生のボタン押しているか
    static TbBool IsOnRaw( const TbUint32 button , const TbUint32 padIndex ){
        return GetInstance().isOnRaw(button,padIndex);
    }

    // 生のボタン押したか
    static TbBool IsPressRaw( const TbUint32 button , const TbUint32 padIndex ){
        return GetInstance().isPressRaw(button,padIndex);
    }

public:

    // パッドのユニークID取得
    static TbUint64 GetPadUniqId( const TbUint32 padIndex = 0 ) {
        return GetInstance().getPadUniqId(padIndex);
    }

    // パッドの製品ユニークID取得
    static TbUint64 GetPadProductUniqId( const TbUint32 padIndex = 0 ) { 
        return GetInstance().getPadProductUniqId(padIndex);
    }

    // パッドの製品名取得
    static const char* GetPadProductName( const TbUint32 padIndex = 0 ) {
        return GetInstance().getPadProductName(padIndex);
    }

    // 登録されている製品数取得
    static TbUint32 GetPadProductCount() {
        return GetInstance().getPadProductCount();
    }

    // 製品設定取得
    static const TbPadButtonSetting* GetPadProductSetting( TbUint32 productIndex ) {
        return GetInstance().getPadProductSetting(productIndex);
    }
    
    // ユニークIDによってインデックスの値を設定
    static void SetPadIndexByUniqId( const TbUint32 padIndex , const TbUint64 uniqId) {
        GetInstance().setPadIndexByUniqId(padIndex,uniqId);
    }

private:

    // パッド数
    TbSint32 getPadCount() const;
    //トリガーしたか
    TbBool isPress(const TbUint32 buttonBit,const TbUint32 padIndex) const;
    //トリガーしたか
    TbBool isPressAny(const TbUint32 buttonBit,const TbUint32 padIndex) const;
    //離したか
    TbBool isRelease(const TbUint32 buttonBit,const TbUint32 padIndex) const;
    //押しているか
    TbBool isOn(const TbUint32 buttonBit,const TbUint32 padIndex) const;
    //押しているか
    TbBool isOnPre(const TbUint32 buttonBit,const TbUint32 padIndex) const;
    //どれか押しているか
    TbBool isOnAny(const TbUint32 buttonBit,const TbUint32 padIndex) const;
    //押しているか(リピート)
    TbBool isOnRepeat(const TbUint32 buttonBit,const TbUint32 padIndex) const;
    //左アナログスティックの方向取得
    const TbVector2& getLeftStick(const TbUint32 padIndex) const;
    //右アナログスティックの方向取得
    const TbVector2& getRightStick(const TbUint32 padIndex) const;
    //左トリガー取得
    const TbUint8 getLeftTrigger(const TbUint32 padIndex) const;
    //右トリガー取得
    const TbUint8 getRightTrigger(const TbUint32 padIndex) const;
    //押されている情報取得
    TbUint32          getOn(const TbUint32 padIndex);
    // パッドボタンに、ゲームパッド固有のボタン番号を割り当てる
    TbResult setPadButtonSetting( const TbUint32 padIndex , const TbPadButton button , const TbUint32 rawButtonIndex );
    // パッドボタンに、ゲームパッド固有のボタン番号を割り当てる
    TbResult setPadButtonSettingByProductUniqId( const TbUint32 productUniqId , const TbPadButton button , const TbUint32 rawButtonIndex );
    // 生のボタン数取得
    TbUint32 getRawButtonCount( const TbUint32 padIndex );
    // 生のボタン押しているか
    TbBool isOnRaw( const TbUint32 button , const TbUint32 padIndex );
    // 生のボタン押したか
    TbBool isPressRaw( const TbUint32 button , const TbUint32 padIndex );
    // 登録されている製品数取得
    TbUint32 getPadProductCount() const;
    // 製品設定取得
    const TbPadButtonSetting* getPadProductSetting( TbUint32 productIndex );
    // パッドのユニークID取得
    TbUint64 getPadUniqId( const TbUint32 padIndex ) const;
    // パッドの製品ユニークID取得
    TbUint64 getPadProductUniqId( const TbUint32 padIndex ) const;
    // パッドの製品名取得
    const char* getPadProductName( const TbUint32 padIndex ) const;
    // ユニークIDによってインデックスの値を設定
    void setPadIndexByUniqId( const TbUint32 padIndex , const TbUint64 uniqId);

private:

    TbPadDepend m_Depend; // 依存コード

};

}

#endif