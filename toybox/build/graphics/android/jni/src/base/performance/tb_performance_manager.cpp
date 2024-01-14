/*!
 * @file
 * @brief パフォーマンス管理
 */

#include <tb_fwd.h>
#include "base/performance/tb_performance_manager.h"
#include "base/time/tb_tick.h"

namespace toybox
{

/*!
 * @brief コンストラクタ
 */
TbPerforcemanceManger::TbPerforcemanceManger()
{
}

/*!
 * @brief デストラクタ
 */
TbPerforcemanceManger::~TbPerforcemanceManger()
{
}

/*!
 * @brief 名前設定
 */
void TbPerforcemanceManger::SetName( TbUint32 index , const char* name )
{
    if( index < TB_ARRAY_COUNT_OF(m_Info))
    {
        m_Info[index].name = name;
    }
}

/*!
 * @brief 計測
 */
void TbPerforcemanceManger::Begin( TbUint32 index )
{
    if (index < TB_ARRAY_COUNT_OF(m_Info))
    {
        m_Info[index].beginMsec = TbTick::GetMilliSeconds();
    }
}

/*!
 * @brief 終了
 */
void TbPerforcemanceManger::End( TbUint32 index)
{
    if (index < TB_ARRAY_COUNT_OF(m_Info))
    {
        TbFloat32 msec = TbTick::GetMilliSeconds();
        m_Info[index].result = msec - m_Info[index].beginMsec;
    }
}

}
