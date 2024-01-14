/*!
 * 多数決用入力
 * @author teco
 * @since 2011.09.14
 */

#ifndef _INCLUDED_SYS_UNION_INPUT_H_
#define _INCLUDED_SYS_UNION_INPUT_H_

#include "sys_input.h"
#include <base/container/tb_array.h>

class SysMajorityInput
{
public:
    
    // コンストラクタ
    SysMajorityInput();
    
    // デストラクタ
    ~SysMajorityInput();
    
public:
    
    // 初期化
    void Initialize( TbUint32 count , TbFloat32 rate );
    
    // 更新
    void Update( TbFloat32 deltatime );

public:

    // ボタンを押しているか
    TbBool IsOn( SysInput::Button button  ) const;

    // ボタンを押したか
    TbBool IsPress( SysInput::Button button ) const;

    // ボタンを話したか
    TbBool IsRelease( SysInput::Button button ) const;

    // 現在の入力比率取得
    TbFloat32 GetCurrentInputRate( SysInput::Button button ) const {
        return m_CurrentRate[button];
    }

private:
    
    // 入力更新
    void update( SysInput::Button button , TbFloat32 deltatime);


private:

    struct InputParam
    {
        InputParam() { TbFill(time,0.0f); }
        TbFloat32 time[SysInput::BUTTON_MAX]; // 入力してからの時間
    };

private:

    TbBitArray32        m_Now;                                  //現フレーム入力情報
    TbBitArray32        m_Old;                                  //前フレーム入力情報
    TbArray<InputParam> m_InputParam;                           // 入力情報
    TbFloat32           m_InputRate;                            // 入力成立割合
    TbFloat32           m_CurrentRate[SysInput::BUTTON_MAX];    // 入力している比率
    TbUint32            m_InputCount;                           // 入力受付数

};

#endif
