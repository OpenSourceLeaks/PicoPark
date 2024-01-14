/*!
 * アクター
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "actor_factory.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.18
 */
ActorFactory::ActorFactory()
    : m_CreateIndex(0)
{
    m_CreateParam.SetEnableSetCapacityAuto(TB_TRUE);
    m_CreateParam.SetCapacity(100);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.10.18
 */
ActorFactory::~ActorFactory()
{
}
    
/*!
 * 初期化
 * @author teco
 * @since 2013.10.18
 */
void ActorFactory::Initialize( TbScriptLuaNode* node )
{
    enum {
        PARAM_PLAYER_COUNT , 
#if !CMN_BUILD_ENABLE_OLD
        PARAM_SCROLL ,
#endif
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
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToSint32ByIndex(param.playerCount,PARAM_PLAYER_COUNT)) ){
            break;
        }
#if !CMN_BUILD_ENABLE_OLD
        if (!TB_VERIFY(TbScriptLuaNode::RESULT_OK == node->TryToFloat32ByIndex(param.scroll, PARAM_SCROLL))) {
            break;
        }
#endif
        const char* name = NULL;
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToStringByIndex(&name,PARAM_TYPE_NAME)) ){
            break;
        }
        param.typeName = name;
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToStringByIndex(&name,PARAM_NAME)) ){
            break;
        }
        param.name = param.typeName + name;
        TbFloat32 x,y;
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToFloat32ByIndex(x,PARAM_X)) ){
            break;
        }
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToFloat32ByIndex(y,PARAM_Y)) ){
            break;
        }
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
 * @since 2013.10.18
 */
void ActorFactory::Update( TbFloat32 scroll )
{
    TbSint32 playerCount = Root::GetInstance().GetPlayerCount();
    for( TbArray<CreateParam>::Iterator ite = m_CreateParam.Begin()+m_CreateIndex , endIte = m_CreateParam.End();
         ite != endIte; ++ite ) 
    {
        if( ite->scroll <= scroll ){
            TbBool isEnable = 0 == ite->playerCount;
            if( !isEnable ) {
                if( 0 < ite->playerCount ){
                    isEnable = ite->playerCount <= playerCount;
                }else{
                    isEnable = playerCount <= -ite->playerCount;
                }
            }
            if( isEnable )
            {
                if( m_CreateFunc.IsValid() ){
                    m_CreateFunc( ite->typeName.GetBuffer() , ite->name.GetBuffer() , ite->pos , ite->userData );
                }
            }
            ++m_CreateIndex;
        }else{
            break;
        }
    }
}
