/*!
 * プレイヤー入力用
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_PLAYER_INPUT_H_
#define _INCLUDED_SYS_PLAYER_INPUT_H_

#include "sys_input.h"
#include <input/tb_bit_input.h>

class SysPlayerCustomInput
{
public:

    // 定期処理
    virtual void Update( float deltasec ) {}

    // ボタンを押しているか
    virtual TbBool IsOn( SysInput::Button button , TbUint32 playerIndex ) const = 0;

    // ボタンを押しているか
    virtual TbBool IsOnRepeat( SysInput::Button button , TbUint32 playerIndex ) const = 0;

    // ボタンを押したか
    virtual TbBool IsPress( SysInput::Button button , TbUint32 playerIndex) const = 0;

    // ボタンを話したか
    virtual TbBool IsRelease( SysInput::Button button , TbUint32 playerIndex) const = 0;

};

class SysPlayerInput
{
public:

    enum Mode
    {
        MODE_DEFAULT , 
        MODE_GIANT , 
        MODE_HYBRID ,
    };

public:

    // コンストラクタ
    SysPlayerInput();

    // デストラクタ
    ~SysPlayerInput();

public:
    
    // モード設定
    void SetMode( Mode mode );

    // ポーズ設定
    void SetPause( TbBool isPause ) {
        m_IsPause = isPause;
    }

    // ポーズ取得
    TbBool IsPause() const {
        return m_IsPause;
    }

    // カスタムインプット設定
    void SetCustom( SysPlayerCustomInput* custom ) {
        m_Custom = custom;
    }

    // ボタンを押しているか
    TbBool IsOn( SysInput::Button button , TbUint32 playerIndex ) const;

    // ボタンを押しているか
    TbBool IsOnAny( TbUint32 playerIndex ) const;

    // ボタンを押しているか
    TbBool IsOnRepeat( SysInput::Button button , TbUint32 playerIndex ) const;

    // ボタンを押したか
    TbBool IsPress( SysInput::Button button , TbUint32 playerIndex) const;

    // ボタンを話したか
    TbBool IsRelease( SysInput::Button button , TbUint32 playerIndex) const;

public:

    // 定期処理
    void Update( float deltasec );

private:

    Mode                    m_Mode;
    SysPlayerCustomInput*   m_Custom;
    TbBool                  m_IsPause;

};

#endif
