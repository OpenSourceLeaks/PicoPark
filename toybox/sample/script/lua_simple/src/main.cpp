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
 * �t���X�N���[��
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void mainLoop()
{
    // test�֐����Ăяo��
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
 * �G���g���[�|�C���g
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{
    // ������
    TbFrameworkCui::Initialize();

    // �X�N���v�g�ݒ�
    {
        TbScriptLuaManager* scriptManager = new TbScriptLuaManager( 1 );
        TbScriptLuaVM* vm = scriptManager->CreateVM( 1 , 10 );
        s_LuaNode = vm->GetNode(0);
        s_LuaNode->DoBuffer( code , TbStrCharCount(code) );
    }

    // ���C�����[�v
    TbFrameworkCui::StartMainLoop( NULL , mainLoop , NULL );
}

/*!
 * �I���|�C���g
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnExitPoint()
{
    // �I��
    TbFrameworkCui::Finalize();
}

}

