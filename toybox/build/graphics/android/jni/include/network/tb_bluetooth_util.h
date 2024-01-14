/*!
 * Bluetoothユーティリティ
 * @author Miyake_Shunsuke
 */

#ifndef _INCLUDED_TB_BLUETOOTH_UTIL_H_
#define _INCLUDED_TB_BLUETOOTH_UTIL_H_

#include <base/container/tb_array.h>
#include <network/tb_network_types.h>
#include <network/tb_bluetooth_types.h>

namespace toybox
{

class TbBluetoothUtil
{
public:
    // 接続可能なデバイス検索
    static void SearchDevice( TbArray<TbBluetoothDeviceInfo>* dst );
};

}

#endif
