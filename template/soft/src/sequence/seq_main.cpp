/*!
 * タイトルシーケンス
 * @author teco
 */

#include "fwd.h"
#include "seq_main.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>
#include <input/tb_pad.h>
#include <input/tb_mouse.h>

/*!
 * コンストラクタ
 * @author teco
 */
SeqMain::SeqMain( TbUint32 id )
    : Super(SEQ_MAIN,"SeqMain")
    , m_Phase(0)
    , m_PhaseTime(0.0f)
{
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqMain::~SeqMain()
{
}

/*!
 * 更新
 * @author teco
 */
void SeqMain::OnUpdate()
{
}

/*!
 * 描画関数
 * @author teco
 */
void SeqMain::OnDraw()
{
}
