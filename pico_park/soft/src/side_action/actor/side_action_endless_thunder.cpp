/*!
* プレイヤー用レーザー
* @author teco
*/

#include "fwd.h"
#include "side_action_endless_thunder.h"
#include "common/cmn_util.h"

/*!
* コンストラクタ
* @author teco
*/
SideActionEndlessThunder::SideActionEndlessThunder()
    : Super()
    , m_CurrentThunder(0)
    , m_Dir(CMN_DIR_RIGHT)
{
    TbFill<CrActionActor*>(m_Thunder,nullptr);
}

/*!
* デストラクタ
* @author teco
*/
SideActionEndlessThunder::~SideActionEndlessThunder()
{
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Thunder); ++i)
    {
        TB_SAFE_RELEASE(m_Thunder[i]);
    }
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionEndlessThunder::Setup(const CrActorFactory::UserData& userData)
{
    m_Dir = static_cast<CmnDir>(userData.AsSint32(0));
}

/*!
* 開始時
* @author teco
*/
void SideActionEndlessThunder::OnEnter(void* userData)
{
    CrStage* stage = GetStage();
    CrActorFactory::CreateParam createParam;
    createParam.typeName = "Thunder";
    createParam.name = "";
    createParam.pos.Set(GetPosX(),GetPosY());
    createParam.userData.SetCount(2);
    createParam.userData.SetS32(0,m_Dir);   // 方向
    createParam.userData.SetS32(1,TB_TRUE); // 貫通
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Thunder); ++i )
    {
        CrActionActor* actor = static_cast<CrActionActor*>(stage->CreateActor(createParam));
        if (actor) {
            actor->Retain();
        }
        m_Thunder[i] = actor;
        TbVector2 offset = CmnUtil::GetDirVector(m_Dir);
        offset *= SideActionThunder::LASER_LEN;
        createParam.pos += offset;
    }
}

/*!
* ロジック更新
* @author teco
*/
void SideActionEndlessThunder::OnUpdateAction(TbFloat32 deltatime)
{
    CrStage* stage = GetStage();
    TbFloat32 scrollX = stage->GetScrollX();
    TbVector4 pos = m_Thunder[!m_CurrentThunder]->GetPos();

    TbBool isSwitch = TB_FALSE;
    switch (m_Dir)
    {
    case CMN_DIR_RIGHT:
        if (pos.GetX() < scrollX) {
            isSwitch = TB_TRUE;
        }
        break;
    default:
        TB_ASSERT(!"unsupported yet");
        break;
    }
    if (isSwitch)
    {
        TbVector2 offset = CmnUtil::GetDirVector(m_Dir);
        offset *= SideActionThunder::LASER_LEN;
        m_Thunder[m_CurrentThunder]->AddPos(offset*2);
        m_CurrentThunder = !m_CurrentThunder;
    }
}
