/*!
 * アクターファクトリー
 * @author teco
 */

#include "tb_fwd.h"
#include "crayon/actor/cr_actor_factory.h"

using namespace toybox;

namespace crayon
{

/*!
 * コンストラクタ
 * @author teco
 */
CrActorFactory::CrActorFactory()
    : m_CreateIndex(0)
    , m_PlayerCount(1)
{
    m_CreateParam.SetEnableSetCapacityAuto(TB_TRUE);
    m_CreateParam.SetCapacity(100);
}

/*!
 * デストラクタ
 * @author teco
 */
CrActorFactory::~CrActorFactory()
{
}
    
/*!
 * 初期化
 * @author teco
 */
void CrActorFactory::Initialize( TbScriptLuaNode* node )
{
    enum {
        PARAM_CREATE_THRESHOLD , 
        PARAM_PLAYER_COUNT , 
        PARAM_TYPE_NAME , 
        PARAM_NAME ,
        PARAM_X , 
        PARAM_Y ,
        PARAM_USERDATA
    };
    m_CreateParam.Clear();

    // テーブル取得
    if( !(TbScriptLuaNode::RESULT_OK==node->GoToChild("createTable")) ) {
        return;
    }

    TbSint32 count = 0;
    while(TbScriptLuaNode::RESULT_OK == node->GoToChild(count)){
        CreateParam param;
        TbMemClear(param);
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToFloat32ByIndex(param.createThreshold,PARAM_CREATE_THRESHOLD)) ){
            break;
        }
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToSint32ByIndex(param.playerCount,PARAM_PLAYER_COUNT)) ){
            break;
        }
        const char* name = NULL;
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToStringByIndex(&name,PARAM_TYPE_NAME)) ){
            break;
        }
        param.typeName = name;
        if( TbScriptLuaNode::RESULT_OK==node->TryToStringByIndex(&name,PARAM_NAME) ){
            param.name = param.typeName + name;
        }else{
            param.name = param.typeName;
        }
        TbFloat32 x = 0.0f;
        TbFloat32 y = 0.0f;
        node->TryToFloat32ByIndex(x,PARAM_X);
        node->TryToFloat32ByIndex(y,PARAM_Y);
        param.pos.Set(x,y);

        // オプション(最大五個までの可変長)
        TbUint32 userDataCount = 0;
        node->TryToTableLength(userDataCount);
        if( userDataCount > PARAM_USERDATA ) {
            userDataCount -= (PARAM_USERDATA);
        }else{
            userDataCount = 0;
        }

        param.userData.SetCount(userDataCount);
        for( TbUint32 i = 0; i < userDataCount && i < UserData::USERDATA_MAX; ++i ){
            node->GoToChild(PARAM_USERDATA+i);
            if( TbScriptLuaNode::TYPE_STRING == node->GetType() ) {
                const char* str = nullptr;
                node->TryToString(&str);
                param.userData.SetString(i,str);
            }else{
                TbFloat32 value = 0.0f;
                node->TryToFloat32(value);
                param.userData.SetF32(i,value);
            }
            node->GoToParent(1);
        }

        m_CreateParam.PushBack(param);
        node->GoToParent(1);
        ++count;
    }
    node->GoToParent(1);
}

/*!
 * 更新
 * @author teco
 */
void CrActorFactory::Update( TbFloat32 value )
{
    for( TbArray<CreateParam>::Iterator ite = m_CreateParam.Begin()+m_CreateIndex , endIte = m_CreateParam.End();
         ite != endIte; ++ite ) 
    {
        if( ite->createThreshold <= value ){

            TbBool isEnable = 0 == ite->playerCount;
            if( !isEnable ) {
                if (10 <= ite->playerCount) 
                {
                    // 2桁の時は、2桁目以上1桁目以下
                    TbUint32 min = ite->playerCount/10;
                    TbUint32 max = ite->playerCount%10;
                    isEnable = min <= m_PlayerCount && m_PlayerCount <= max;
                }else{
                    if( 0 < ite->playerCount ){ // 正の場合はその人数以上の時有効
                        isEnable = ite->playerCount <= m_PlayerCount;
                    }else{ // 負の場合はその人数以下の場合有効
                        isEnable = m_PlayerCount <= -ite->playerCount;
                    }
                }
            }
            if( isEnable )
            {
                if( m_CreateFunc.IsValid() ){
                    m_CreateFunc( *ite );
                }
            }
            ++m_CreateIndex;
        }else{
            break;
        }
    }
}

/*!
 * スキップ
 * @author teco
 */
void CrActorFactory::Skip( TbFloat32 value )
{
    for( TbArray<CreateParam>::Iterator ite = m_CreateParam.Begin()+m_CreateIndex , endIte = m_CreateParam.End();
         ite != endIte; ++ite ) 
    {
        if( ite->createThreshold < value ){
            ++m_CreateIndex;
        }else{
            break;
        }
    }
}

}
