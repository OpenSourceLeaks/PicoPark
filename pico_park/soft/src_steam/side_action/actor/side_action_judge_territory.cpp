/*!
 * スイッチ
 * @author teco
 */

#include "fwd.h"
#include "side_action_judge_territory.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action/side_action_types.h"
#include <base/math/tb_random.h>

/*!
 * コンストラクタ
 * @author teco
 */
SideActionJudgeTerritory::SideActionJudgeTerritory( TbUint32 tileW , TbUint32 tileH )
    : Super()
    , m_Phase(PHASE_IDLE)
    , m_PhaseSec(0.0f)
    , m_JudgeCount(0)
    , m_JudgeWindow(UiMessageWindow::TYPE_NORMAL)
{
    m_TerritoryMap.Initialize(tileW,tileH);
    m_SpriteHookerBack.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawBack));
    m_SpriteHookerBack.GetSprite().SetZ(CMN_SPRITE_Z_BACK_1);
    m_SpriteHookerFront.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawFront));
    m_SpriteHookerFront.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);

    AddComponent( &m_SpriteHookerBack );
    AddComponent( &m_SpriteHookerFront );

    // 判定ウィンドウ
    m_JudgeWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionJudgeTerritory::~SideActionJudgeTerritory()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionJudgeTerritory::OnPostAction( TbFloat32 deltatime )
{
    Super::OnPostAction(deltatime);
    switch( m_Phase )
    {
    case PHASE_IDLE:
        {
            updateTerritory();
        }
        break;
#if 0
    case PHASE_JUDGE:
        {
            if( !IsWaitingResult() ){
                break;
            }
            if( m_PhaseSec < 1.0f ) {
                break;
            }

            SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
            TbUint32 playerCount = stage->GetPlayerCount();
            if( m_JudgeCount < playerCount ){
                setPhase(m_Phase+1);
            }else{
                setPhase(PHASE_RESULT);
            }
        }
        break;
    case PHASE_JUDGE+1:
        {
            SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
            TbUint32 playerIndex = stage->GetPlayer(m_JudgeCount)->GetPlayerIndex();
            m_JudgeWindow.SetPlayerBit(TB_BIT(playerIndex));
            m_JudgeWindow.SetMessage("RESULT:000");
            m_JudgeWindow.SetEnableCloseButton(TB_FALSE);
            m_JudgeWindow.Open();
            m_JudgeWindow.SetEnableFit(TB_FALSE);
            setPhase(m_Phase+1);
        }
        break;
    case PHASE_JUDGE+2:
        {
            TbBool isEnd = TB_FALSE;
            TbUint32 value = 0;
            if( m_PhaseSec < 2.0f ) {
                value = TbRandom(999);;
            }else{
                SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
                TbUint32 playerIndex = stage->GetPlayer(m_JudgeCount)->GetPlayerIndex();
                value = getTerritory(playerIndex);
                isEnd = TB_TRUE;
            }
            TbStaticString64 message;
            message.SetFormatedString("RESULT:%3u",value);
            m_JudgeWindow.SetMessage(message.GetBuffer());

            if( isEnd ){
                ++m_JudgeCount;
                setPhase(PHASE_JUDGE);
            }
        }
        break;
#else
    case PHASE_JUDGE:
        {
            if( !IsWaitingResult() ){
                break;
            }
            if( m_PhaseSec < 1.5f ) {
                break;
            }
            SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
            TbUint32 playerCount = stage->GetPlayerCount();
            TbSint32 count = -1;
            TbSint32 maxCount = m_Result[playerCount-1].count;
            while( ( count < 0 ||
                        count == m_Result[m_JudgeCount].count ) &&
                        maxCount != m_Result[m_JudgeCount].count )
            {
                if( count < 0 ){
                    count = m_Result[m_JudgeCount].count;
                }
                deleteTerrirory( m_Result[m_JudgeCount].playerIndex );
                ++m_JudgeCount;
            }
            setPhase(m_Phase);
            if( maxCount == m_Result[m_JudgeCount].count )
            {
                if( m_JudgeCount == playerCount-1 ){
                    SetPrizePlayerIndex(m_Result[m_JudgeCount].playerIndex);
                }else{
                    SetPrizePlayerIndex(-1);
                }
                setPhase(PHASE_RESULT);
            }
        }
        break;
#endif
    }
    m_PhaseSec += deltatime;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionJudgeTerritory::NotifyEvent( TbUint32 event , void* param )
{
    Super::NotifyEvent(event,param);
    if( SIDE_ACTION_EVENT_SWITCH_ON != event ) {
        return 0;
    }

    // 順位確定
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    TbUint32 playerCount = stage->GetPlayerCount();
    for( TbUint32 i = 0; i < playerCount; ++i )
    {
        const SideActionPlayer* player = stage->GetPlayer(i);
        m_Result[i].index = i;
        m_Result[i].playerIndex = player->GetPlayerIndex();
        m_Result[i].count = getTerritory(player->GetPlayerIndex());
    }
    TbInsertionSort(m_Result,m_Result+playerCount);

    setPhase(PHASE_JUDGE);
    return 0;
}

/*!
 * 領域更新
 * @author teco
 */
void SideActionJudgeTerritory::updateTerritory()
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    const ActionMap& map = stage->GetMap();
    TbFloat32 chipSize = static_cast<TbFloat32>(map.GetChipSize());

    TbUint32 playerCount = stage->GetPlayerCount();
    for( TbUint32 i = 0; i < playerCount; ++i )
    {
        const SideActionPlayer* player = stage->GetPlayer(i);
#if 0
        const TbRectF32& rect = player->GetCollision()->GetGlobalRect();
        TbSint32 beginX = rect.x/chipSize;
        TbSint32 beginY = rect.y/chipSize;
        TbSint32 endX = (rect.x+rect.width)/chipSize;
        TbSint32 endY = (rect.y+rect.height)/chipSize;
        for( TbSint32 x = beginX; x <= endX; ++x ) {
            for( TbSint32 y = beginY; y <= endY; ++y ) {
                m_TerritoryMap.Set(x,y,i);
            }
        }
#else
        const TbVector4& pos = player->GetPos();
        TbSint32 x = pos.GetX()/chipSize;
        TbSint32 y = pos.GetY()/chipSize;
        m_TerritoryMap.Set(x,y,player->GetPlayerIndex());
#endif
    }
}

/*!
 * 領土取得
 * @author teco
 */
TbUint32 SideActionJudgeTerritory::getTerritory( TbUint32 playerIndex )
{
    TbUint32 result = 0;

    const TbDimensionS32& size = m_TerritoryMap.GetSize();
    for( TbSint32 y = 0; y < size.height; ++y )
    {
        for( TbSint32 x = 0; x < size.width; ++x )
        {
            TbSint32 value = m_TerritoryMap.Get(x,y);
            if( playerIndex == value )
            {
                ++result;
            }
        }
    }
    return result;
}

/*!
 * 領土削減
 * @author teco
 */
void SideActionJudgeTerritory::deleteTerrirory( TbUint32 playerIndex )
{
    const TbDimensionS32& size = m_TerritoryMap.GetSize();
    for( TbSint32 y = 0; y < size.height; ++y )
    {
        for( TbSint32 x = 0; x < size.width; ++x )
        {
            TbSint32 value = m_TerritoryMap.Get(x,y);
            if( playerIndex == value )
            {
                m_TerritoryMap.Set(x,y,-1);
            }
        }
    }
}

/*!
 * 前景描画
 * @author teco
 * @since 2014.08.05
 */
void SideActionJudgeTerritory::onDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    static const TbRectF32 UV_RECT(CMN_TEX_UV_RECT(496,0,16,16));

    ActionStage* stage = GetActionStage();
    const ActionMap& map = stage->GetMap();
    TbFloat32 chipSize = static_cast<TbFloat32>(map.GetChipSize());
    const TbRectF32 RECT(0.0f,0.0f,chipSize,chipSize);

    const TbDimensionS32& size = m_TerritoryMap.GetSize();
    for( TbSint32 y = 0; y < size.height; ++y )
    {
        for( TbSint32 x = 0; x < size.width; ++x )
        {
            TbSint32 value = m_TerritoryMap.Get(x,y);
            if( 0 <= value )
            {
                TbColor color = CmnUtil::GetPlayerColor(SysInput::GetPlayerPadIndex(value));
                TbVector2 drawPos( x*chipSize , y*chipSize );
                TbMatrix44 pose;
                pose.SetTranslation(drawPos.GetX(),drawPos.GetY(),0.0f);
                material->Draw(RECT,UV_RECT,color,pose);                
            }
        }
    }
}

/*!
 * 前景描画
 * @author teco
 * @since 2014.08.05
 */
void SideActionJudgeTerritory::onDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
}
