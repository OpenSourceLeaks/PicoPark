/*!
 * 巨大用入力
 * @author teco
 * @since 2015.09.14
 */

#include "fwd.h"
#include "sys_giant_input.h"
#include <input/tb_keyboard.h>
#include <input/tb_input_setting.h>
#include <input/tb_pad.h>
#include <script/lua/tb_script_lua.h>
#include <base/time/tb_tick.h>
#include <base/io/tb_file.h>
#include <base/io/stream/tb_stream_text_writer.h>
#include <base/io/stream/tb_file_stream.h>
#include <base/io/stream/tb_memory_stream.h>

namespace
{

enum
{
    GIANT_PAD_BEGIN , 
    GIANT_PAD_UP = GIANT_PAD_BEGIN , 
    GIANT_PAD_DOWN , 
    GIANT_PAD_RIGHT , 
    GIANT_PAD_LEFT , 
    GIANT_PAD_A , 
    GIANT_PAD_B , 
    GIANT_PAD_MAX ,
};

static const char* PAD_STR[] =
{
    "UP" , 
    "DOWN" , 
    "RIGHT" , 
    "LEFT" , 
    "A" , 
    "B" , 
};
TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(PAD_STR)==GIANT_PAD_MAX );

static const TbUint32 BUTTON_PAD[] = 
{
    TB_PAD_A_BIT     ,
    TB_PAD_B_BIT     ,
    TB_PAD_X_BIT     ,
    TB_PAD_Y_BIT     ,
    TB_PAD_UP_BIT    , 
//    TB_PAD_DOWN_BIT  ,
//    TB_PAD_LEFT_BIT  ,
//    TB_PAD_RIGHT_BIT ,
//    TB_PAD_START_BIT ,
};

static const TbUint32 BUTTON_PAD_INFO[] = 
{
    TB_PAD_A     ,
    TB_PAD_B     ,
    TB_PAD_X     ,
    TB_PAD_Y     ,
    TB_PAD_UP    , 
//    TB_PAD_DOWN_BIT  ,
//    TB_PAD_LEFT_BIT  ,
//    TB_PAD_RIGHT_BIT ,
//    TB_PAD_START_BIT ,
};

static const TbKeyType BUTTON_KEY[] = 
{
    TB_KEY_RETURN ,
    TB_KEY_BACK ,
    TB_KEY_UP ,
    TB_KEY_UP , 
    TB_KEY_DOWN ,
    TB_KEY_LEFT ,
    TB_KEY_RIGHT ,
    TB_KEY_X ,
    TB_KEY_Z ,
    TB_KEY_SPACE ,
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(BUTTON_KEY)==SysInput::BUTTON_MAX);

/*!
 * デバイスユニークIDからパッド番号取得
 * @author teco
 * @since 2014.10.02
 */
TbBool getPadIndexFromUniqId( TbUint32* index , const TbUint64 uniqId )
{
    if( 0 == uniqId ) {
        return TB_FALSE;
    }
    TbSint32 count = TbPad::GetPadCount();
    for( TbSint32 i = 0; i < count; ++i ) {
        if( TbPad::GetPadUniqId(i) == uniqId ) {
            if( index ) {
                *index = i;
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}

}

/*!
 * コンストラクタ
 * @author teco
 */
SysGiantInput::SysGiantInput()
    : m_DebugPhase(-1)
{
}

/*!
 * デストラクタ
 * @author teco
 */
SysGiantInput::~SysGiantInput()
{
}

/*!
 * 初期化
 * @author teco
 */
void SysGiantInput::Initialize()
{
#if TB_PLATFORM_IS_WIN
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);

    TbScriptLuaNode::Result res = node->DoFileSync("giant_setting.lua");
    if( TB_VERIFY( res == TbScriptLuaNode::RESULT_OK ) )
    {
        if( TbScriptLuaNode::RESULT_OK == node->GoToAbsolutely("padList") ){
            for( TbUint32 i = 0; i < GIANT_PAD_MAX; ++i ){
                TbUint32 id = 0;
                if( TbScriptLuaNode::RESULT_OK == node->TryToUint32ByIndex(id,i) ){
                    if( 0 != id ) {
                        TbPad::SetPadIndexByUniqId( CMN_PLAYER_MAX+i , id );
                    }
                }
            }
        }
    }
#endif
}

void SysGiantInput::Update()
{
    m_PadInfo.old = m_PadInfo.now;
    m_PadInfo.now = 0;
    m_PadInfo.repeat = 0;
#if TB_PLATFORM_IS_WIN

    for( TbSint32 i = 0; i < SysInput::BUTTON_MAX ; ++i ){
        TbBool isOn = TB_FALSE;
        TbSint32 playerIndex = getIndex(static_cast<SysInput::Button>(i));
        if( playerIndex >= 0 ){
            for( TbSint32 j = 0; j < TB_ARRAY_COUNT_OF(BUTTON_PAD); ++j )
            {
                if( TbPad::IsOn( BUTTON_PAD[j] , playerIndex ) ){
                    isOn = TB_TRUE;
                }
            }
        }
        if( isOn ){
            m_PadInfo.now |= TB_BIT( i );
            m_PadInfo.repeatMilliSec[ i ] += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds());
            TbFloat32 repeatTime = ( m_PadInfo.repeatFirst & TB_BIT( i ) ) ? TbInputSetting::GetRepeatMilliSec() : TbInputSetting::GetRepeatMilliSecFirst();;
            if( !( m_PadInfo.old & TB_BIT( i ) ) ){
                m_PadInfo.repeat |= TB_BIT( i );
            }else if( m_PadInfo.repeatMilliSec[ i ] > repeatTime ){
                m_PadInfo.repeat |= TB_BIT( i );
                m_PadInfo.repeatFirst |= TB_BIT( i );
                m_PadInfo.repeatMilliSec[ i ] -= repeatTime;
            }
        } else {
            m_PadInfo.repeatMilliSec[ i ] = 0.0f;
            m_PadInfo.repeatFirst &= ~TB_BIT(i);
        }
    }

#if TB_IS_DEBUG
    if( GIANT_PAD_BEGIN <= m_DebugPhase && m_DebugPhase <= GIANT_PAD_A )
    {
        TbSint32 count = TbPad::GetPadCount();
        for( TbSint32 i = 0; i < count; ++i ) {
            if( TbPad::IsPressAny(TB_PAD_ANY_BIT,i) ){
                TbUint64 id = TbPad::GetPadUniqId(i);
                TbPad::SetPadIndexByUniqId( CMN_PLAYER_MAX+m_DebugPhase , id );
                ++m_DebugPhase;
                break;
            }
        }
        if( GIANT_PAD_A < m_DebugPhase ){
            m_DebugPhase = -1;
            writeGiantSetting();
        }
    }
#endif
#endif
}

/*!
 * ボタンを押しているか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysGiantInput::IsOn( SysInput::Button button , TbUint32 playerIndex )
{
    if( 0 != playerIndex ) {
        return TB_FALSE;
    }
    TbUint32 buttonBit = TB_BIT(button);
    if(  ( m_PadInfo.now & buttonBit ) == buttonBit ) {
        return TB_TRUE;
    }
    if( TbKeyboard::IsOn( BUTTON_KEY[button] ) ) {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ボタンを押しているか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysGiantInput::IsOnRepeat( SysInput::Button button , TbUint32 playerIndex )
{
    if( 0 != playerIndex ) {
        return TB_FALSE;
    }
    TbUint32 buttonBit = TB_BIT(button);
    if( (m_PadInfo.repeat & TB_BIT(button)) == buttonBit ) {
        return TB_TRUE;
    }
    if( TbKeyboard::IsOnRepeat( BUTTON_KEY[button] ) ) {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*! 
 * ボタンを押したか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysGiantInput::IsPress( SysInput::Button button , TbUint32 playerIndex )
{
    if( 0 != playerIndex ) {
        return TB_FALSE;
    }
    TbUint32 buttonBit = TB_BIT(button);
    if(  ( m_PadInfo.now & buttonBit ) == buttonBit && ( m_PadInfo.old & buttonBit ) == 0 ) {
        return TB_TRUE;
    }
    if( TbKeyboard::IsPress( BUTTON_KEY[button] ) ) {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ボタンを話したか
 * @author teco
 */
TbBool SysGiantInput::IsRelease( SysInput::Button button , TbUint32 playerIndex )
{
    if( 0 != playerIndex ) {
        return TB_FALSE;
    }
    TbUint32 buttonBit = TB_BIT(button);
    if(  ( m_PadInfo.now & buttonBit ) == 0 && ( m_PadInfo.old & buttonBit ) == buttonBit ) {
        return TB_TRUE;
    }
    if( TbKeyboard::IsRelease( BUTTON_KEY[button] ) ) {
        return TB_TRUE;
    }
    return TB_FALSE;
}

TbSint32 SysGiantInput::getIndex( SysInput::Button button )
{
    TbSint32 index = -1;
    switch( button )
    {
    case SysInput::BUTTON_UP:
        index = CMN_PLAYER_MAX + GIANT_PAD_UP;
        break;
    case SysInput::BUTTON_DOWN:
        index = CMN_PLAYER_MAX + GIANT_PAD_DOWN;
        break;
    case SysInput::BUTTON_LEFT:
        index = CMN_PLAYER_MAX + GIANT_PAD_LEFT;
        break;
    case SysInput::BUTTON_RIGHT:
        index = CMN_PLAYER_MAX + GIANT_PAD_RIGHT;
        break;
    case SysInput::BUTTON_DECIDE:
    case SysInput::BUTTON_JUMP:
        index = CMN_PLAYER_MAX + GIANT_PAD_A;
        break;
    case SysInput::BUTTON_ROTATE_R:
        index = CMN_PLAYER_MAX + GIANT_PAD_A;
        break;
    case SysInput::BUTTON_ROTATE_L:
        index = CMN_PLAYER_MAX + GIANT_PAD_B;
        break;
    }
    return index;
}

/*!
 * デバッグメニューセットアップ
 * @author teco
 */
void SysGiantInput::SetupDebugMenu( TbDebugMenu* debugMenu )
{
#if TB_PLATFORM_IS_WIN
    TbDebugMenuItemButton* button = debugMenu->CreateItemButton("Setup Giant Input");
    button->SetClickedCallback(TbCreateMemFunc(this,&Self::onClickedSetupInput));

    TbDebugMenuItemAlias::AliasInfo aliasInfo[] = 
    {
        { -1 , "NULL" } , 
        { GIANT_PAD_UP , "UP" } , 
        { GIANT_PAD_DOWN , "DOWN" } , 
        { GIANT_PAD_RIGHT , "RIGHT" } , 
        { GIANT_PAD_LEFT , "LEFT" } , 
        { GIANT_PAD_A , "A" } , 
        { GIANT_PAD_B , "B" } , 
    };
    TbDebugMenuItemAlias* alias = debugMenu->CreateItemAlias("Setupping Button");
    alias->SetAliasList(aliasInfo,TB_ARRAY_COUNT_OF(aliasInfo));
    alias->Bind(&m_DebugPhase);
#endif
}

/*!
 * 入力セットアップ
 * @author teco
 */
void     SysGiantInput::onClickedSetupInput()
{
    m_DebugPhase = GIANT_PAD_BEGIN;
}

/*!
 * 設定書き出し
 * @author teco
 */
void SysGiantInput::writeGiantSetting()
{
#if TB_PLATFORM_IS_WIN
    TbFile file("giant_setting.lua",TB_FILE_OPERATE_WRITE,TB_FILE_SYNC,TB_FILE_PATH_ID_PRESET_ROOT);
    if( TB_SUCCEEDED(file.Open()) ){
        static const TbUint32 BUFSIZE = 1024*64;
        TbChar8* buf = new TbChar8[BUFSIZE]; // 最大64K
        TbMemoryStream stream(buf,BUFSIZE);
        TbStreamTextWriter writer(&stream);
        writer.WriteString("padList=\n");
        writer.WriteString("{\n");

        // キーコンフィグ
        for( TbUint32 i = 0; i < GIANT_PAD_MAX; ++i ) 
        {
            TbStaticString128 str;
            TbUint32 id = TbPad::GetPadUniqId(CMN_PLAYER_MAX+i);
            str.SetFormatedString("    %u,\n",id);
            writer.WriteString(str.GetBuffer());
        }

        writer.WriteString("}\n\0");
        writer.WriteEOFChar8();

        TbSizeT fileSize = TbStrCharCount(buf);
        file.Write(buf,fileSize);
        TB_SAFE_DELETE_ARRAY(buf);
    }
#endif
}
