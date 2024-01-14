/*! 
 * Luaスクリプト
 * @author Miyake Shunsuke
 * @since 2010.12.22
 */
#ifndef _INCLUDED_TB_SCRIPT_LUA_MANAGER_H_
#define _INCLUDED_TB_SCRIPT_LUA_MANAGER_H_

#include <base/container/tb_array.h>
#include <base/util/tb_singleton.h>
#include <script/lua/tb_script_lua_fwd.h>

namespace toybox
{

class TbScriptLuaVM;

/*!
 * Luaスクリプトクラス
 * @author Miyake Shunsuke
 * @since 2010.12.25
 */
class TbScriptLuaManager : public TbSingletonRef<TbScriptLuaManager>
{
public:
    static const TbSint32 TOP_STACK_SIZE_DEFAULT = 5;
public:
    // コンストラクタ
    explicit TbScriptLuaManager( TbUint32 vmMax );
    // デストラクタ
    ~TbScriptLuaManager();
public:
    // 仮想マシン生成
    TbScriptLuaVM* CreateVM( TbSint32 threadMax , TbSint32 topStackSize = TOP_STACK_SIZE_DEFAULT );
    // 仮想マシン取得
    TbScriptLuaVM* GetVM( TbSint32 index );
public:
    TbArray<TbScriptLuaVM*> m_VirtualMachineList; // 仮想マシンリスト
};

}

#endif
