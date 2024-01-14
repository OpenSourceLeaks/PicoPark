/*!
 * プレイヤー入力用
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_CR_PLAYER_INPUT_H_
#define _INCLUDED_CR_PLAYER_INPUT_H_

#include "crayon/input/cr_input.h"
#include <input/tb_bit_input.h>

namespace crayon
{

class CrPlayerCustomInput
{
public:

    // 定期処理
    virtual void Update( float deltasec ) {}

    // ボタンを押しているか
    virtual TbBool IsOn( CrInput::Button button , TbUint32 playerIndex ) const = 0;

    // ボタンを押しているか
    virtual TbBool IsOnRepeat( CrInput::Button button , TbUint32 playerIndex ) const = 0;

    // ボタンを押したか
    virtual TbBool IsPress( CrInput::Button button , TbUint32 playerIndex) const = 0;

    // ボタンを話したか
    virtual TbBool IsRelease( CrInput::Button button , TbUint32 playerIndex) const = 0;

};

class CrPlayerInput
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
    CrPlayerInput();

    // デストラクタ
    ~CrPlayerInput();

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
    void SetCustom( CrPlayerCustomInput* custom ) {
        m_Custom = custom;
    }

    // ボタンを押しているか
    TbBool IsOn( CrInput::Button button , TbUint32 playerIndex ) const;

    // ボタンを押しているか
    TbBool IsOnRepeat( CrInput::Button button , TbUint32 playerIndex ) const;

    // ボタンを押したか
    TbBool IsPress( CrInput::Button button , TbUint32 playerIndex) const;

    // ボタンを話したか
    TbBool IsRelease( CrInput::Button button , TbUint32 playerIndex) const;

public:

    // 定期処理
    void Update( float deltasec );

private:

    Mode                    m_Mode;
    CrPlayerCustomInput*    m_Custom;
    TbBool                  m_IsPause;

};

}

#endif
