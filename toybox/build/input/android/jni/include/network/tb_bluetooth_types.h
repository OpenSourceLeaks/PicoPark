/*!
 * Bluetooth型
 * @author Miyake_Shunsuke
 */

#ifndef _INCLUDED_TB_BLUETOOTH_TYPES_H_
#define _INCLUDED_TB_BLUETOOTH_TYPES_H_

namespace toybox
{

struct TbBluetoothDeviceInfo
{
    TbBluetoothDeviceInfo() : id(0) {}
    TbStaticString512 deviceName;
    TbUint64          id;
};

}

#endif
