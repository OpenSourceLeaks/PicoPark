/*!
 * @file
 * @brief パッドの機種依存コード(Emscripten)
 */

#ifndef _INCLDUED_TB_PAD_EMSCRIPTEN_H_
#define _INCLDUED_TB_PAD_EMSCRIPTEN_H_

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
    TbSint32 GetPadCount() const { return m_PadCount; }

private:

    static const TbSint32 PAD_MAX = 32;

    struct PadInfo
    {
        PadInfo() : index( 0 ) , padInfo( NULL ) {}
        TbSint32 index;
        const TbPadInfo* padInfo;
    };

private:

    // セットアップ
    void setup();

    // パッド情報取得
    const TbPadInfo* getPadInfo( TbUint32 padIndex ) const;

    // ユニークIDによってインデックスの値を設定
    void setPadIndexByUniqId( const TbUint32 padIndex , const TbUint64 uniqId);

private:
    PadInfo             m_PadInfo[ PAD_MAX ];   // パッド情報
    TbUint32            m_PadCount;           // 接続済みパッド数
};

}

#endif