/*!
 * 入力全般に関連する設定項目クラス
 * @author Miyake Shunsuke
 * @since 2011.09.17
 */

#ifndef _INCLUDED_TB_INPUT_SETTING_H_
#define _INCLUDED_TB_INPUT_SETTING_H_

namespace toybox
{

class TbInputSetting
{
public:

    //! リピート間隔時間(初回)設定
    static void SetRepeatMilliSecFirst( TbFloat32 milliSec );

    //! リピート間隔時間(初回)取得
    static TbFloat32 GetRepeatMilliSecFirst();

    //! リピート間隔時間(初回)取得
    static TbFloat32 GetRepeatSecFirst() {
        return GetRepeatMilliSecFirst() * 0.001f;
    }

    //! リピート間隔時間(初回)設定
    static void SetRepeatMilliSec( TbFloat32 milliSec );

    //! リピート間隔時間(初回)取得
    static TbFloat32 GetRepeatMilliSec();

    //! リピート間隔時間(初回)取得
    static TbFloat32 GetRepeatSec() {
        return GetRepeatMilliSec() * 0.001f;
    }

private:
    TbInputSetting();
    ~TbInputSetting();
};

}

#endif
