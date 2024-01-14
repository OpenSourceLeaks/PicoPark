/*!
 * Luaスクリプトの仮想マシン
 * @author Miyake Shunsuke
 * @since 2010.12.22
 */

#include "tb_fwd.h"
#include "script/lua/tb_script_lua_vm.h"
#include "script/lua/tb_script_lua_node.h"

namespace toybox
{

/*! 
 * コンストラクタ(スレッドの格納先だけ確保してスレッドはメインスレッド一つのみ作成)
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVM::TbScriptLuaVM( TbSint32 index , TbSint32 threadMax , TbSint32 topStackSize )
    : m_Index( index )
    , m_TopStackSize( topStackSize )
    , m_NodeList( threadMax < 0 ? 1 : threadMax  )
{
    for( TbUint32 i = 0; i < threadMax; ++i ) {
        CreateThread();
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaVM::~TbScriptLuaVM()
{
    TbSint32 nodeNum = m_NodeList.GetCount();
    TbScriptLuaNode* node = NULL;
    for( TbSint32 i = 0;  i < nodeNum; ++i ){
        node = m_NodeList.GetAt(i);
        TB_SAFE_DELETE( node );
    }
}

/*! 
 * スレッド操作用ノード取得
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode* TbScriptLuaVM::GetNode( TbSint32 threadIndex )
{
    TbScriptLuaNode* result = NULL;
    if( 0 <= threadIndex && threadIndex < m_NodeList.GetCount() ){
        result = m_NodeList.GetAt( threadIndex );
    }
    return result;
}

/*! 
 * スレッド作成
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
TbScriptLuaNode* TbScriptLuaVM::CreateThread()
{
    TbScriptLuaNode* result = NULL;
    if( !m_NodeList.IsFull() ){
        TbScriptLuaNode* node = TB_NEW TbScriptLuaNode( m_NodeList.GetCount() , m_TopStackSize );
        m_NodeList.PushBack( node );
    }
    return result;
}

/*! 
 * toLua用関数バインド
 * @author Miyake Shunsuke
 * @since 2010.12.28
 */
void TbScriptLuaVM::BindForToLua(int (*func)(lua_State*))
{
   TbScriptLuaNode* baseNode = m_NodeList.GetAt(0);
   func( baseNode->m_State );
}

}
