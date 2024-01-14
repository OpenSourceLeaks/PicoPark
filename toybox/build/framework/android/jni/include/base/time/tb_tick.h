/*!
 * @file
 * @brief チック値クラス
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TICK_H_
#define _INCLUDED_TB_TICK_H_

namespace toybox
{

/*!
 * タイマークラス
 */
class TbTick
{
private:

    //コンストラクタ
    TbTick();
    
    //デストラクタ
    ~TbTick();

public:

    // 毎フレームにかかった時間を計測します。
    static void Update();

    // カウント毎に必要な時間
    static void SetFrameMilliSecondsPerCount( TbFloat64 milliSeconds ) {
        s_FrameMilliSecondPerCount = milliSeconds;
    }

    // フレームスピード( カウント )
    static TbUint32 GetFrameSpeedCount() {
        return s_FrameCount;
    }

    // フレームスピード( カウント )
    static TbFloat32 GetFrameSpeedCountF32() {
        return static_cast<TbFloat32>(s_FrameCount);
    }

    // 前フレームでかかった時間取得
    static TbFloat64 GetFrameSpeedMilliSeconds() {
        return s_FrameSpeedMilliSecond;
    }

    // 前フレームでかかった時間取得
    static TbFloat64 GetFrameSpeedSeconds() {
        return s_FrameSpeedMilliSecond * 0.001;
    }

    // m秒取得
    static TbFloat64 GetMilliSeconds();

    // μ秒取得
    static TbFloat64 GetMicroSeconds();

public: // デバッグ用(リテール版では機能しません)

    // ブレークポイントを張って時間がかかる場合にカウント値に制限をつける場合
    static void SetDebugFrameSpeedCountMax( TbUint32 max ) {
        s_DebugFrameSpeedCountMax = max;
    }

    // フレームスピードカウントを固定値にする
    static void SetDebugFrameSpeedCount( TbUint32 count ) {
        s_DebugFrameSpeedCount = count;
    }

    // フレームスピードカウント固定値を有効にする
    static void SetEnableActiveDebugFrameSpeedCount( TbBool isActive ) {
        s_IsActiveDebugFrameSpeedCount = isActive;
    }

    // フレームスピードカウント固定値を有効にするかどうか
    static TbBool IsEnableActiveDebugFrameSpeedCount() {
        return s_IsActiveDebugFrameSpeedCount;
    }

private:
    static TbFloat64     s_PreFrameMilliSecond;
    static TbFloat64     s_FrameSpeedMilliSecond;
    static TbFloat64     s_FrameMilliSecondPerCount;
    static TbUint32      s_FrameCount;
    static TbUint32      s_DebugFrameSpeedCountMax;
    static TbUint32      s_DebugFrameSpeedCount;
    static TbBool        s_IsActiveDebugFrameSpeedCount;
};

}

#endif