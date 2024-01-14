/*!
 * @file
 * @brief 言語設定取得
 */

#include "tb_fwd.h"
#include "base/system/tb_system_setting.h"

namespace toybox
{

TbLanguage TbSystemSetting::GetLanguage()
{
    nn::settings::LanguageCode code = nn::oe::GetDesiredLanguage();
    TbLanguage lang = TB_LANG_JP;
    if (::nn::util::Strncmp(code.string, "ja", sizeof(code.string)) == 0)
    {
        lang = TB_LANG_JP;
    }
    else
    {
        lang = TB_LANG_US;
    }
    return lang;
}

}
