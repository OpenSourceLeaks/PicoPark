/*!
 * システム設定(プラットフォームの本体設定など
 * @author teco
 */

#ifndef _INCLUDED_TB_SYSTEM_SETTING_H_
#define _INCLUDED_TB_SYSTEM_SETTING_H_

namespace toybox
{

enum TbLanguage
{
    TB_LANG_US,
    TB_LANG_GB,
    TB_LANG_JP,
};

class TbSystemSetting
{
public:

    // 言語設定取得
    static TbLanguage GetLanguage();

private:

};

}

#endif