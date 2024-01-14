/*!
 * Luaスクリプトの仮想マシン
 * @author Miyake Shunsuke
 * @since 2010.12.22
 */
#ifndef _INCLUDED_TB_SCRIPT_LUA_VM_
#define _INCLUDED_TB_SCRIPT_LUA_VM_

#include <script/lua/tb_script_lua_node.h>

namespace toybox
{

/*!
 * Lua仮想マシン
 * @author Miyake Shunsuke
 * @since 2010.12.25
 */
class TbScriptLuaVM
{
private:
    // コンストラクタ(スレッドの格納先だけ確保してスレッドはメインスレッド一つのみ作成)
    TbScriptLuaVM( TbSint32 index , TbSint32 threadMax , TbSint32 topStackSize );
    // デストラクタ
    ~TbScriptLuaVM();
public:
    //! インデックス取得
    TbSint32 GetIndex(){ return m_Index; }
    // スレッド操作用ノード取得
    TbScriptLuaNode* GetNode( TbSint32 threadIndex );
    // スレッド作成
    TbScriptLuaNode* CreateThread();
    // toLua用関数バインド
    void BindForToLua( int (*func)(lua_State*) );
public:
    TbSint32 m_Index;                         //!< インデックス
    TbSint32 m_TopStackSize;                  //!< 内部スタック用
    TbArray<TbScriptLuaNode*> m_NodeList;     //!< スレッド別ノード
private:
    friend class TbScriptLuaManager;
};

}

#endif