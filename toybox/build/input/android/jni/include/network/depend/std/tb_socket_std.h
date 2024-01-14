/*!
 * 依存部分
 * @author Miyake_Shunsuke
 * @since 2012.05.10
 */

#ifndef _INCLUDED_TB_SOCKET_STD_H_
#define _INCLUDED_TB_SOCKET_STD_H_

namespace toybox
{

class TbSocketDepend
{
    friend class TbSocket;
public:
    
    // コンストラクタ
    TbSocketDepend() : m_Socket(INVALID_SOCKET) {}
    
    // デストラクタ
    ~TbSocketDepend() {}

public:

    void SetSocket( const SOCKET& socket ) {
        m_Socket = socket;
    }
    
    const SOCKET& GetSocket() { 
        return m_Socket;
    }

private:

    SOCKET        m_Socket;

};

}

#endif
