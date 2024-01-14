/*!
 * 依存部分
 * @author Miyake_Shunsuke
 * @since 2012.05.10
 */

#ifndef _INCLUDED_TB_NETWORK_MANAGER_STD_H_
#define _INCLUDED_TB_NETWORK_MANAGER_STD_H_

namespace toybox
{

class TbNetworkManagerDepend
{
    friend class TbNetworkManager;
public:
    
    // コンストラクタ
    TbNetworkManagerDepend()
        : m_ReadFileDescMax(0)
        , m_ReadFileDescSetResultCount(0)
        , m_WriteFileDescMax(0)
        , m_WriteFileDescSetResultCount(0)
        , m_ExceptFileDescMax(0)
        , m_ExceptFileDescSetResultCount(0)
    {
        FD_ZERO(&m_ReadFileDescSet);
        FD_ZERO(&m_ReadFileDescSetResult);
        FD_ZERO(&m_WriteFileDescSet);
        FD_ZERO(&m_WriteFileDescSetResult);
        FD_ZERO(&m_ExceptFileDescSet);
        FD_ZERO(&m_ExceptFileDescSetResult);
    }
    
    // デストラクタ
    ~TbNetworkManagerDepend() {}

public:

    // 読み込み用ファイルディスクセット取得
    const fd_set& GetReadFileDescSetReslt() const {
        return m_ReadFileDescSetResult;
    }

    // 読み込み用ファイルディスクセットのFD数取得
    TbSint32 GetReadFileDescSetResltCount() const {
        return m_ReadFileDescSetResultCount;
    }

    // 書き込み用ファイルディスクセット取得
    const fd_set& GetWriteFileDescSetReslt() const {
        return m_WriteFileDescSetResult;
    }

    // 書き込み用ファイルディスクセットのFD数取得
    TbSint32 GetWriteFileDescSetResltCount() const {
        return m_WriteFileDescSetResultCount;
    }

    // 書き込み用ファイルディスクセット取得
    const fd_set& GetExceptFileDescSetReslt() const {
        return m_ExceptFileDescSetResult;
    }

    // 書き込み用ファイルディスクセットのFD数取得
    TbSint32 GetExceptFileDescSetResltCount() const {
        return m_ExceptFileDescSetResultCount;
    }

private:
    
    TbUint32    m_ReadFileDescMax;
    fd_set      m_ReadFileDescSet;       
    fd_set      m_ReadFileDescSetResult;
    TbSint32    m_ReadFileDescSetResultCount;

    TbUint32    m_WriteFileDescMax;
    fd_set      m_WriteFileDescSet;       
    fd_set      m_WriteFileDescSetResult;
    TbSint32    m_WriteFileDescSetResultCount;

    TbUint32    m_ExceptFileDescMax;
    fd_set      m_ExceptFileDescSet;       
    fd_set      m_ExceptFileDescSetResult;
    TbSint32    m_ExceptFileDescSetResultCount;

};

}

#endif
