/*!
 * パケット受信バッファ
 * @author teco
 */

#include <tb_fwd.h>

#include <ws2bth.h>
#include "network/tb_bluetooth_util.h"

namespace toybox
{

/*!
 * 接続可能なデバイス検索
 * @author teco
 */
void TbBluetoothUtil::SearchDevice( TbArray<TbBluetoothDeviceInfo>* dst )
{
    dst->Clear();
    WSAQUERYSET wsaq;
    HANDLE hLookup;
    union {
        CHAR buf[5000];
        double __unused; // ensure proper alignment
    };
    LPWSAQUERYSET pwsaResults = (LPWSAQUERYSET) buf;
    DWORD dwSize = sizeof(buf);
    BOOL bHaveName;
    TbMemClear(wsaq);

    wsaq.dwSize = sizeof(wsaq);
    wsaq.dwNameSpace = NS_BTH;
    wsaq.lpcsaBuffer = NULL;

    int _ret = WSALookupServiceBegin (&wsaq, LUP_CONTAINERS, &hLookup);
    if ( _ret != ERROR_SUCCESS )
    {
        wprintf(L"WSALookupServiceBegin failed %d\r\n", GetLastError());
        return;
    }
    TbMemClear(pwsaResults, sizeof(WSAQUERYSET));
    pwsaResults->dwSize = sizeof(WSAQUERYSET);
    pwsaResults->dwNameSpace = NS_BTH;
    pwsaResults->lpBlob = NULL;
    while (ERROR_SUCCESS == WSALookupServiceNext (hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR, &dwSize, pwsaResults))
    {
        TB_ASSERT(pwsaResults->dwNumberOfCsAddrs == 1);
        BTH_ADDR b = ((SOCKADDR_BTH *)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;
        bHaveName = pwsaResults->lpszServiceInstanceName && *(pwsaResults->lpszServiceInstanceName);

        if ( bHaveName ){
            TbBluetoothDeviceInfo deviceInfo;
            deviceInfo.deviceName = pwsaResults->lpszServiceInstanceName;
            deviceInfo.id = b;
            dst->PushBack(deviceInfo);
        }
    }

    WSALookupServiceEnd(hLookup);
}

}
