/*!
 * スレッドクラス(WiiU依存)
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_THREAD_WIIU_H_
#define _INCLUDED_TB_THREAD_WII

namespace toybox
{

class TbThreadDepend
{
public:
    // スレッド実行関数
    static int ThreadProc(int intArg, void *ptrArg);
public:
    // コンストラクタ
    TbThreadDepend();
    // デストラクタ
    ~TbThreadDepend();
public:
    //! ハンドル取得
    OSThread&  GetHandle() { 
        return m_Handle; 
    }
    //! ハンドルを解放する(動いているスレッドが終了するわけではない)
    void Detach() {
        if( m_IsCreated ){
           // スレッドが終了するわけではなく、スレッドとの切り離しを行うだけ
           OSDetachThread (&m_Handle);
           m_IsCreated = TB_FALSE;
        }
    }
private:
    OSThread   m_Handle;
    TbBool     m_IsCreated;
    TbUint8*   m_Stack;
private:
    friend class TbThread;
};

}

#endif
