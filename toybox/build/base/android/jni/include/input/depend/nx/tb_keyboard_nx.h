/*!
 * @file
 * @brief キーボード入力
 * @author teco
 */

#ifndef _INCLDUED_TB_KEYBOARD_NX_H_
#define _INCLDUED_TB_KEYBOARD_NX_H_

namespace toybox
{

/*!
 * キーボード依存クラス
 * @author teco
 */
class TbKeyboardDepend
{
public:

    struct Info{
        enum {
            KEY_REPEAT_BIT_CURRENT = TB_BIT(0) ,  
            KEY_REPEAT_BIT_FIRST = TB_BIT(1) , 
        };
        TbUint8              keyRepeat[ TB_KEY_MAX ]; // キーリピート情報
        TbFloat32            keyRepeatTimer[ TB_KEY_MAX ]; // キーリピート情報
        TbUint8              keyBuf[2][ TB_KEY_MAX ]; // キー情報
        TbUint8              keyIndex;
    };

public:

    //コンストラクタ
    TbKeyboardDepend( const TbKeyboardParameter* parameter , const TbSint32 count );

    //デストラクタ
    virtual ~TbKeyboardDepend(void);

public:

    // 情報取得
    Info& GetInfo() {
        return m_Info;
    }

    // 情報取得
    const Info& GetInfo() const {
        return m_Info;
    }

private:

    enum{
        FLAG_ERROR,
        FLAG_READY,
        FLAG_ACTIVE,
    };

private:

    // セットアップ
    TbResult setup( const TbKeyboardParameter* parameter , const TbSint32 count );

    // 解放
    void release();

    // 更新
    void update();

private:

    TbBitArray32         m_BitArray;
    Info                 m_Info;    //!< 情報クラス

private:

    friend class TbKeyboard;

};

}

#endif