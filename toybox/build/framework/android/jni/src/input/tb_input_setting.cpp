/*!
 * 入力全般に関連する設定項目クラス
 * @author Miyake Shunsuke
 * @since 2011.09.17
 */
#include "tb_fwd.h"
#include "input/tb_input_setting.h"

namespace toybox
{

static TbFloat32 s_RepeatMilliSecFirst = 1000.0f;
static TbFloat32 s_RepeatMilliSec = 320.0f;

/*!
 * リピート間隔時間(初回)設定
 * @author Miyake Shunsuke
 * @since 2011.09.17
 */
void TbInputSetting::SetRepeatMilliSecFirst( TbFloat32 milliSec )
{
    s_RepeatMilliSecFirst = milliSec;
}

/*!
 * リピート間隔時間(初回)取得
 * @author Miyake Shunsuke
 * @since 2011.09.17
 */
TbFloat32 TbInputSetting::GetRepeatMilliSecFirst()
{
    return s_RepeatMilliSecFirst;
}

/*!
 * リピート間隔時間(初回)設定
 * @author Miyake Shunsuke
 * @since 2011.09.17
 */
void TbInputSetting::SetRepeatMilliSec( TbFloat32 milliSec )
{
    s_RepeatMilliSec = milliSec;
}

/*!
 * リピート間隔時間(初回)取得
 * @author Miyake Shunsuke
 * @since 2011.09.17
 */
TbFloat32 TbInputSetting::GetRepeatMilliSec()
{
    return s_RepeatMilliSec;
}

}
