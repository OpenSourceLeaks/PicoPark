#include "fwd.h"
#include <script/lua/tb_script_lua.h>
#include <framework/tb_framework_cui.h>

namespace
{
using namespace toybox;

TbScriptLuaNode*       s_LuaNode = NULL;

const char* code = " \
function printf(...) \
 io.write(string.format(...)) \
end \
function test() \
 print(\"aaa\") \
end \
\
t = { name = \"lua\", length = 3, sense = \"moon\" } \
printf(\"Hello\") \
";

const char* code2 = " \
function printf(...) \
 io.write(string.format(...)) \
end \
function test() \
 print(\"aaa\") \
end \
\
a = 3.0 \
t = { name = \"bbb\", length = 3, sense = \"moon\" } \
printf(\"Hello\") \
";

/*!
 * フルスクリーン
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void mainLoop()
{
    // test関数を呼び出す
    s_LuaNode->GoToAbsolutely("t._parent.t.name");
//    s_LuaNode->GoToAbsolutely("a");
    s_LuaNode->PrintDebugInfo();

    if( 0 ){
        s_LuaNode->DoBuffer( code2 , TbStrCharCount(code2) );
    }

//    s_LuaNode->Try
//    TbScriptLuaReturnParam retParam(0);
//    s_LuaNode->Call( retParam );
}

}

namespace toybox
{

/*!
 * エントリーポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{
    // 初期化
    TbFrameworkCui::Initialize();

    // スクリプト設定
    {
        TbScriptLuaManager* scriptManager = new TbScriptLuaManager( 1 );
        TbScriptLuaVM* vm = scriptManager->CreateVM( 1 , 10 );
        s_LuaNode = vm->GetNode(0);
        s_LuaNode->DoBuffer( code , TbStrCharCount(code) );
    }

    // メインループ
    TbFrameworkCui::StartMainLoop( NULL , mainLoop , NULL );
}

/*!
 * 終了ポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnExitPoint()
{
    // 終了
    TbFrameworkCui::Finalize();
}

}

