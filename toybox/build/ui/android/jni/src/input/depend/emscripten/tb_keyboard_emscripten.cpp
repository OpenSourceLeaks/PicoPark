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

EM_BOOL TbKeyboardDepend::onEvent(int action, const EmscriptenKeyboardEvent *e, void *userData)
{
    if( !userData ) {
        return 1;
    }
    TbKeyboardDepend* depend = reinterpret_cast<TbKeyboardDepend*>(userData);
    if (action == EMSCRIPTEN_EVENT_KEYDOWN) {
        depend->m_Info.buffer[e->which] = TB_TRUE;
    }else if (action == EMSCRIPTEN_EVENT_KEYUP){
        depend->m_Info.buffer[e->which] = TB_FALSE;
    }
#if 0
    TbPrintf("keyEvent=%d,%s,%s,%s,%d,%d,%d,%d\n",
              action,
              e->key,
              e->code,
              e->charValue,
              e->ctrlKey,
              e->shiftKey,
              e->keyCode,
              e->which);
#endif
    return 1;
}

/*!
 * コンストラクタ
 * @author teco
 */
TbKeyboardDepend::TbKeyboardDepend( const TbKeyboardParameter* parameter , const TbSint32 count )
{
    setup( parameter , count );
}

/*!
 * デストラクタ
 * @author teco
 */
TbKeyboardDepend::~TbKeyboardDepend(void) 
{
    emscripten_set_keydown_callback(nullptr,0,1,nullptr);
    emscripten_set_keyup_callback(nullptr,0,1,nullptr);
}

/*!
 * セットアップ
 * @author teco
 */
TbResult TbKeyboardDepend::setup( const TbKeyboardParameter* parameter , const TbSint32 count )
{
    emscripten_set_keydown_callback(nullptr,this,1,onEvent);
    emscripten_set_keyup_callback(nullptr,this,1,onEvent);
    return TB_S_OK;
}

/*!
 * 更新
 * @author teco
 */
void TbKeyboardDepend::update()
{
    m_Info.keyIndex = !m_Info.keyIndex;
    // 変換テーブル(負数は未サポート)
    static const TbSint16 CONVER_TABLE[ TB_KEY_MAX ] = 
    {
        // アルファベット
        65 , // TB_KEY_A,
        66 , // TB_KEY_B,
        67 , // TB_KEY_C,
        68 , // TB_KEY_D,
        69 , // TB_KEY_E,
        70 , // TB_KEY_F,
        71 , // TB_KEY_G,
        72 , // TB_KEY_H,
        73 , // TB_KEY_I,
        74 , // TB_KEY_J,
        75 , // TB_KEY_K,
        76 , // TB_KEY_L,
        77 , // TB_KEY_M,
        78 , // TB_KEY_N,
        79 , // TB_KEY_O,
        80 , // TB_KEY_P,
        81 , // TB_KEY_Q,
        82 , // TB_KEY_R,
        83 , // TB_KEY_S,
        84 , // TB_KEY_T,
        85 , // TB_KEY_U,
        86 , // TB_KEY_V,
        87 , // TB_KEY_W,
        88 , // TB_KEY_X,
        89 , // TB_KEY_Y,
        90 , // TB_KEY_Z,
        // 数字
        49 , // TB_KEY_1,
        50 , // TB_KEY_2,
        51 , // TB_KEY_3,
        52 , // TB_KEY_4,
        53 , // TB_KEY_5,
        54 , // TB_KEY_6,
        55 , // TB_KEY_7,
        56 , // TB_KEY_8,
        57 , // TB_KEY_9,
        48 , // TB_KEY_0,
        // 特殊文字
        -1 , // TB_KEY_MINUS,
        -1 , // TB_KEY_EQUAL,
        32 , // TB_KEY_SPACE,
        27 , // TB_KEY_ESCAPE ,
        9 , // TB_KEY_TAB ,
        8 , // TB_KEY_BACK ,
        13, // TB_KEY_RETURN ,
        -1 , // TB_KEY_F1 ,
        -1 , // TB_KEY_F2 ,
        -1 , // TB_KEY_F3 ,
        -1 , // TB_KEY_F4 ,
        -1 , // TB_KEY_F5 ,
        -1 , // TB_KEY_F6 ,
        -1 , // TB_KEY_F7 ,
        -1 , // TB_KEY_F8 ,
        -1 , // TB_KEY_F9 ,
        -1 , // TB_KEY_F10 ,
        -1 , // TB_KEY_F11 ,
        -1 , // TB_KEY_F12 ,
        -1, // TB_KEY_F13 ,
        -1, // TB_KEY_F14 ,
        -1 , // TB_KEY_F15 ,
        -1 , // TB_KEY_PAGE_UP ,
        -1 , // TB_KEY_PAGE_DOWN ,
        17 , // TB_KEY_CONTROL_L ,
        -1 , // TB_KEY_CONTROL_R ,
        16 , // TB_KEY_SHIFT_L ,
        -1 , // TB_KEY_SHIFT_R ,
        18 , // TB_KEY_ALT_L ,
        -1  , // TB_KEY_ALT_R ,
        38 , // TB_KEY_UP ,
        40 , // TB_KEY_DOWN ,
        39 , // TB_KEY_RIGHT ,
        37 , // TB_KEY_LEFT ,
        97 , // TB_KEY_NUMPAD_1 ,
        98 , // TB_KEY_NUMPAD_2 ,
        99 , // TB_KEY_NUMPAD_3 ,
        100 , // TB_KEY_NUMPAD_4 ,
        101 , // TB_KEY_NUMPAD_5 ,
        102 , // TB_KEY_NUMPAD_6 ,
        103 , // TB_KEY_NUMPAD_7 ,
        104 , // TB_KEY_NUMPAD_8 ,
        105 , // TB_KEY_NUMPAD_9 ,
        96 , // TB_KEY_NUMPAD_0 ,
        -1 , // TB_KEY_MULTIPLY ,
        -1 , // TB_KEY_SUBSTRACT ,
        -1 , // TB_KEY_ADD ,
        -1 , // TB_KEY_DECIMAL ,
        -1 , // TB_KEY_NUMPAD_ENTER ,
        -1 , // TB_KEY_NUMPAD_DEVIDE ,
        // TB_KEY_MAX ,
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVER_TABLE) == TB_KEY_MAX );
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(CONVER_TABLE); ++i ){
        if( CONVER_TABLE[ i ] < 0 ){
            continue;
        }
        m_Info.keyBuf[m_Info.keyIndex][i] = m_Info.buffer[ CONVER_TABLE[ i ] ] ? 1U : 0U;
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