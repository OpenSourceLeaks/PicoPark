/*!
 * スレッドクラス(Win32依存)
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_THREAD_WIN32_H_
#define _INCLUDED_TB_THREAD_WIN32_H_

namespace toybox
{

class TbThreadDepend
{
public:
    // スレッド実行関数
    static void* ThreadProc(void*);
public:
    // コンストラクタ
    TbThreadDepend();
    // デストラクタ
    ~TbThreadDepend();
public:
    //! ハンドルを解放する(動いているスレッドが終了するわけではない)
    void Detach();
private:
    // ハンドル取得
    pthread_t& getHandle() {
        return m_Thread;
    }
private:
    pthread_t m_Thread;
private:
    friend class TbThread;
};

}

#endif
