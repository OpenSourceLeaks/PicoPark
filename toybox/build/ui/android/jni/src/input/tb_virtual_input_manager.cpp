/*!
 * 入力管理クラス
 * @author Miyake Shunsuke
 * @since 2011.05.18
 */

#include "tb_fwd.h"
#include "input/tb_virtual_input_manager.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.18
 */
TbVirtualInputManager::TbVirtualInputManager( TbUint32 inputMax )
    : m_Input( inputMax ) 
{
}
    
/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.18
*/
TbVirtualInputManager::~TbVirtualInputManager()
{
}

/*!
 * 入力オブジェクトの登録
 * @author Miyake Shunsuke
 * @since 2011.05.18
 */
TbVirtualInputId TbVirtualInputManager::RegistInput( TbVirtualInput* input )
{
    if( m_Input.IsFull() || !input ){
        return TB_VIRTUAL_INPUT_ID_INVALID;
    }
    return m_Input.Add( input );
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.05.18
 */
void TbVirtualInputManager::Update()
{
    TbPool<TbVirtualInput*>::Iterator ite = m_Input.Begin();
    TbPool<TbVirtualInput*>::Iterator endIte = m_Input.End();
    for( ; ite != endIte; ++ite ){
        (*ite)->Update();
    }
}

}
