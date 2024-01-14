/*!
 * @file
 * @brief プロセッサー
 */

#include "tb_fwd.h"
#include "base/system/tb_processor.h"

namespace toybox
{

TbUint32 TbProcessor::GetCurrentId()
{
    return nn::os::GetCurrentProcessorNumber();
}

}
