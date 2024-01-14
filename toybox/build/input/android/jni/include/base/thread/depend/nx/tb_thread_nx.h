/*!
 * スレッドクラス(NX依存)
 * @author teco
 */
 
#ifndef _INCLUDED_TB_THREAD_NX_H_
#define _INCLUDED_TB_THREAD_NX_H_

namespace toybox
{

class TbThreadDepend
{
public:
    // スレッド実行関数
    static void ThreadProc(void*);

    TbThreadDepend() : stack(nullptr) , isCreated(TB_FALSE){}

    nn::os::ThreadType  thread;
    char*               stack;
    TbBool              isCreated;
};

}

#endif
