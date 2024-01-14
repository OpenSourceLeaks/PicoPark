/*!
 * プロセッサー
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_PROCESS_H_
#define _INCLUDED_TB_PROCESS_H_

namespace toybox
{

class TbProcess
{
public:

    // 起動
    static void Start( const char* applicationPath , const char* args );

private:
    TbProcess();
    ~TbProcess();
};

}

#endif