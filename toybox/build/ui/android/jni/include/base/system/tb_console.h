/*!
 * コンソール
 * @author Miyake Shunsuke
 * @since 2011.08.26
 */

#ifndef _INCLUDED_TB_CONSOLE_H_
#define _INCLUDED_TB_CONSOLE_H_

namespace toybox
{

class TbConsole
{
public:

    // コンソール生成
    static TbResult Create();

    // コンソール生成されている
    static TbBool   IsCreated();

    // コンソール削除
    static TbResult Delete();

    // 出力
    static void Printf( const TbChar8 *fmt , ... );

private:
    TbConsole();
    ~TbConsole();
};

}

#endif