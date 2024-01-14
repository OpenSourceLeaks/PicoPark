﻿/*!
 * @file
 * @brief パッドの機種依存コード(WiiU)
 */

#ifndef _INCLDUED_TB_PAD_WIIU_H_
#define _INCLDUED_TB_PAD_WIIU_H_

namespace toybox
{

class TbPad;

class TbPadDepend
{
    friend class TbPad;
    typedef TbPadDepend Self;
public:
    // コンストラクタ
    TbPadDepend();
    // デストラクタ
    ~TbPadDepend();

public:

    // 更新
    void Update();

    // 接続パッド数
    TbSint32 GetPadCount() const { return PAD_MAX; }

private:

    static const TbSint32 PAD_MAX = 8;

private:

    // セットアップ
    void setup();

private:
    TbPadInfo           m_PadInfo[ PAD_MAX ];   // パッド情報
};

}

#endif