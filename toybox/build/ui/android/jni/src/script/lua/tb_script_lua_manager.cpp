/*! 
 * Luaスクリプト
 * @author Miyake Shunsuke
 * @since 2010.12.22
 */

#include "tb_fwd.h"
#include "script/lua/tb_script_lua_manager.h"
#include "script/lua/tb_script_lua_vm.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */
TbScriptLuaManager::TbScriptLuaManager( TbUint32 max )
    : m_VirtualMachineList( max )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */
TbScriptLuaManager::~TbScriptLuaManager()
{
    TbSint32 vmNum = m_VirtualMachineList.GetCount();
    TbScriptLuaVM* vm = NULL;
    for( TbSint32 i = 0;  i < vmNum; ++i ){
        vm = m_VirtualMachineList.GetAt(i);
        TB_SAFE_DELETE( vm );
    }
}

/*!
 * 仮想マシン生成
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */
TbScriptLuaVM* TbScriptLuaManager::CreateVM( TbSint32 threadMax , TbSint32 topStackSize )
{
    if( m_VirtualMachineList.IsFull() ){
        return NULL;
    }
    TbScriptLuaVM* vm = TB_NEW TbScriptLuaVM( m_VirtualMachineList.GetCount() - 1 , threadMax , topStackSize );
    m_VirtualMachineList.PushBack( vm );
    return vm;
}   

/*!
 * 仮想マシン取得
 * @author Miyake Shunsuke
 * @since 2010.12.27
 */
TbScriptLuaVM* TbScriptLuaManager::GetVM( TbSint32 index )
{
    if( 0 <= index && index < m_VirtualMachineList.GetCount() ){
        return m_VirtualMachineList.GetAt( index );
    }
    return NULL;
}

}
