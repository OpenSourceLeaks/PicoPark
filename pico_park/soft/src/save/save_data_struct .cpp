/*!
 * セーブデータ 
 */

#include "fwd.h"
#include "save_data_struct.h"
#include <crayon/input/cr_input.h>
#include <base/system/tb_system_setting.h>

void SaveInitialize(SaveDataHeader* header)
{
    TbMemClear(*header);
    header->version = SAVE_DATA_VERSION;
    header->size    = sizeof(SaveData);
}

void SaveInitialize(SaveDataSetting* data)
{
    TbMemClear(*data);
    data->volume = 10;
    if (TbSystemSetting::GetLanguage() != TB_LANG_JP)
    {
        data->bitArray |= SaveDataSetting::BIT_CHANGE_JUMP_BUTTON;
        CrInput::SetInputLayout(CR_INPUT_LAYOUT_B_DECIDE);
    }else {
        CrInput::SetInputLayout(CR_INPUT_LAYOUT_A_DECIDE);
    }
}

void SaveInitialize(SaveDataStage* data)
{
    TbMemClear(*data);
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(data->data); ++i) {
        data->data[i].bestTime = SaveDataStage::BEST_TIME_INVALID;
    }
}

void SaveInitialize(SaveDataCoopWorld* data)
{
    TbMemClear(*data);
}

void SaveInitialize(SaveDataCoop* data)
{
    TbMemClear(*data);
}

void SaveInitialize(SaveData* data)
{
    SaveInitialize(&data->header);
    SaveInitialize(&data->setting);
    SaveInitialize(&data->coop);
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(data->stage); ++i) {
        SaveInitialize(&data->stage[i]);
    }
}

