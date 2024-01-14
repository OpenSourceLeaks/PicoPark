/*!
 * 仮想入力管理クラス
 * @author Miyake Shunsuke
 * @since 2011.05.05
 */

#ifndef _INCLUDED_TB_VIRTUAL_INPUT_MANAGER_H_
#define _INCLUDED_TB_VIRTUAL_INPUT_MANAGER_H_

#include <base/container/tb_pool.h>
#include <input/tb_virtual_input.h>
#include <base/util/tb_singleton.h>

namespace toybox
{

/*!
 * 仮想入力
 */
class TbVirtualInputManager : public TbSingletonRef<TbVirtualInputManager>
{
public:

    // コンストラクタ
    explicit TbVirtualInputManager( TbUint32 inputMax );
    
    // デストラクタ
    ~TbVirtualInputManager();

private: // コピーと代入禁止

    TB_DISALLOW_COPY_AND_ASSIGN(TbVirtualInputManager);

public: // 外部向け関数

    // 入力オブジェクト取得
    const TbVirtualInput* GetInput( TbVirtualInputId id )
    {
        TbVirtualInput** result = m_Input.GetAt(id);
        if( result ){
            return *result;
        }
        return NULL;
    }

    // 入力オブジェクトの登録
    TbVirtualInputId RegistInput( TbVirtualInput* input );

    // 更新
    void Update();

private:

    TbPool<TbVirtualInput*> m_Input;  // 入力クラス

};

/*!
 * F32型入力データを取得
 * @author Miyake Shunsuke
 * @since 2011.05.05
 */
inline TbFloat32 TbVirtualInputGetF32( TbVirtualInputId id , TbSint32 index )
{
    TbVirtualInputManager& inputManager = TbVirtualInputManager::GetInstance();
    const TbVirtualInput* input = inputManager.GetInput(id);
    return input ? input->GetF32( index ) : 0.0f;
}

/*!
 * S32型入力データを取得
 * @author Miyake Shunsuke
 * @since 2011.05.05
 */
inline TbSint32 TbVirtualInputGetS32( TbVirtualInputId id , TbSint32 index )
{
    TbVirtualInputManager& inputManager = TbVirtualInputManager::GetInstance();
    const TbVirtualInput* input = inputManager.GetInput(id);
    return input ? input->GetS32( index ) : 0;
}

/*!
 * 押しているかどうかを取得
 * @author Miyake Shunsuke
 * @since 2011.05.05
 */
inline TbBool TbVirtualInputIsOnFromIndex( TbVirtualInputId id , TbUint32 index )
{
    TbVirtualInputManager& inputManager = TbVirtualInputManager::GetInstance();
    const TbVirtualInput* input = inputManager.GetInput(id);
    return input ? input->IsOnFromIndex(index) : TB_FALSE;
}

/*!
 * 押しているかどうかを取得
 * @author Miyake Shunsuke
 * @since 2011.05.05
 */
inline TbBool TbVirtualInputIsPressFromIndex( TbVirtualInputId id , TbUint32 index )
{
    TbVirtualInputManager& inputManager = TbVirtualInputManager::GetInstance();
    const TbVirtualInput* input = inputManager.GetInput(id);
    return input ? input->IsPressFromIndex(index) : TB_FALSE;
}

/*!
 * 押しているかどうかを取得
 * @author Miyake Shunsuke
 * @since 2011.05.05
 */
inline TbBool TbVirtualInputIsReleaseFromIndex( TbVirtualInputId id , TbUint32 index )
{
    TbVirtualInputManager& inputManager = TbVirtualInputManager::GetInstance();
    const TbVirtualInput* input = inputManager.GetInput(id);
    return input ? input->IsReleaseFromIndex(index) : TB_FALSE;
}

}

#endif
