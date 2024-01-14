/*!
 * Window管理クラス(NX版)
 * シングルウィンドウなので、実質一つだけ管理
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/window/tb_window_manager.h"
#include "graphics/window/tb_window.h"

namespace toybox
{


/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
TbWindowManager::TbWindowManager()
    : m_BitArray()
    , m_WindowList(1)
    , m_FocusWindow( NULL )
    , m_Depend()
{
    m_BitArray.SetBit( FLAG_ERROR , setup() == TB_S_OK );
}

/*!
 * デストラクタ
 * @author teco
 */
TbWindowManager::~TbWindowManager()
{
    DeleteWindowAll();
    nn::vi::CloseDisplay(m_Depend.display);
    nn::vi::Finalize();
}

/*!
 * ウィンドウクラスをセットアップ
 * @author teco
 */
TbResult TbWindowManager::setup()
{
    nn::vi::Initialize();
    nn::Result result = nn::vi::OpenDefaultDisplay(&m_Depend.display);
    NN_ABORT_UNLESS_RESULT_SUCCESS(result);
    NN_UNUSED(result);
    return TB_S_OK;
}

/*!
 * 更新
 */
TbBool TbWindowManager::Update() 
{
    return TB_TRUE;
}

}
