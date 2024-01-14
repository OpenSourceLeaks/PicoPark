/*!
 * プロセッサー
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_PROCESSOR_H_
#define _INCLUDED_TB_PROCESSOR_H_

namespace toybox
{

class TbProcessor
{
public:

    // 現在のCPU番号
    static TbUint32 GetCurrentId();

private:
    TbProcessor();
    ~TbProcessor();
};

}

#endif