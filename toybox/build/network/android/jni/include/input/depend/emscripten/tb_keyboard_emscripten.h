/*!
 * Emscripten用キーボード入力
 * @author teco
 */

#ifndef _INCLDUED_TB_KEYBOARD_EMSCRIPTEN_H_
#define _INCLDUED_TB_KEYBOARD_EMSCRIPTEN_H_

namespace toybox
{

class TbKeyboardDepend
{
public:

    struct Info
    {
        static const TbSizeT BUFFER_SIZE = 256;
        enum {
            KEY_REPEAT_BIT_CURRENT = TB_BIT(0) ,  
            KEY_REPEAT_BIT_FIRST = TB_BIT(1) , 
        };
        
        Info()
        {
            TbMemClear(keyRepeat);
            TbMemClear(keyRepeatTimer);
            TbMemClear(keyBuf[0]);
            TbMemClear(keyBuf[1]);
            TbMemClear(buffer);
            keyIndex = 0;
        }
        
        TbUint8              keyRepeat[ TB_KEY_MAX ]; // キーリピート情報
        TbFloat32            keyRepeatTimer[ TB_KEY_MAX ]; // キーリピート情報
        TbUint8              keyBuf[2][ TB_KEY_MAX ]; // キー情報
        TbUint8              keyIndex;
        TbBool               buffer[BUFFER_SIZE];
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

    // セットアップ
    TbResult setup( const TbKeyboardParameter* parameter , const TbSint32 count );

    // 更新
    void update();
    
    // イベントコールバック
    static EM_BOOL onEvent(int action, const EmscriptenKeyboardEvent *e, void *userData);

private:

    Info                 m_Info;  //!< 情報クラス

private:

    friend class TbKeyboard;

};

}

#endif