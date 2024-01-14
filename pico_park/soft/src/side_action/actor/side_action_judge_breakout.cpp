/*!
 * スイッチ
 * @author teco
 */

#include "fwd.h"
#include "side_action_judge_breakout.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"
#include <base/math/tb_random.h>
#include "common/cmn_util.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionJudgeBreakout::SideActionJudgeBreakout( TbUint32 tileW , TbUint32 tileH )
    : Super()
    , m_Phase(PHASE_IDLE)
    , m_PhaseSec(0.0f)
    , m_Type(TYPE_BREAKOUT)
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
    SetFinishMessage("FINISH");
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionJudgeBreakout::~SideActionJudgeBreakout()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionJudgeBreakout::OnPostAction( TbFloat32 deltatime )
{
    Super::OnPostAction(deltatime);
    switch( m_Phase )
    {
    case PHASE_IDLE:
        {
            if( TYPE_BREAKOUT == m_Type )
            {
                const CrActionMap& map = GetActionStage()->GetMap();
                if( !map.Exists(CR_ACTION_MAP_CHIP_BREAK_BEGIN, CR_ACTION_MAP_CHIP_BREAK_END) )
                {
                    NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON,nullptr);
                }
            }
        }
        break;
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
            if (maxCount == m_Result[m_JudgeCount].count)
            {
                if (m_JudgeCount == playerCount - 1) {
                    SetPrizePlayerIndex(m_Result[m_JudgeCount].playerIndex);
                } else {
                    SetPrizePlayerIndex(-1);
                }
                setPhase(PHASE_RESULT);
            }
            else
            {
                while ((count < 0 ||
                    count == m_Result[m_JudgeCount].count) &&
                    maxCount != m_Result[m_JudgeCount].count)
                {
                    if (count < 0) {
                        count = m_Result[m_JudgeCount].count;
                    }
                    deleteTerrirory(m_Result[m_JudgeCount].playerIndex);
                    ++m_JudgeCount;
                }
                setPhase(m_Phase);
            }
        }
        break;
    }
    m_PhaseSec += deltatime;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionJudgeBreakout::NotifyEvent( TbUint32 event , void* param )
{
    Super::NotifyEvent(event,param);
    if( SIDE_ACTION_EVENT_SWITCH_ON == event ) {

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

    }
    else if( SIDE_ACTION_EVENT_REGISTER == event )
    {
        RegistInfo* info = reinterpret_cast<RegistInfo*>(param);
        m_TerritoryMap.Set(info->x,info->y,info->playerIndex);
    }
    return 0;
}

/*!
 * 領土取得
 * @author teco
 */
TbUint32 SideActionJudgeBreakout::getTerritory( TbUint32 playerIndex )
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
void SideActionJudgeBreakout::deleteTerrirory( TbUint32 playerIndex )
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
void SideActionJudgeBreakout::onDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    static const TbRectF32 UV_RECT(CMN_TEX_UV_RECT(496,0,16,16));

    CrActionStage* stage = GetActionStage();
    const CrActionMap& map = stage->GetMap();
    TbFloat32 chipSize = static_cast<TbFloat32>(map.GetChipSize());
    const TbRectF32 RECT(0.0f,0.0f,chipSize,chipSize);

    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    TbUint32 fontSize = 16;
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    fontBuffer->SetFontSize(fontSize);
    fontBuffer->SetColor(TbColor::Black());
    fontBuffer->SetPoseMatrix(poseMatrix);

    const TbDimensionS32& size = m_TerritoryMap.GetSize();
    for( TbSint32 y = 0; y < size.height; ++y )
    {
        for( TbSint32 x = 0; x < size.width; ++x )
        {
            TbSint32 value = m_TerritoryMap.Get(x,y);
            if( 0 <= value )
            {
                TbColor color = CmnUtil::GetPlayerColor(value);
                TbVector2 drawPos( x*chipSize , y*chipSize );
                TbMatrix44 pose;
                pose.SetTranslation(drawPos.GetX(),drawPos.GetY(),0.0f);
                material->Draw(RECT,UV_RECT,color,pose);  

                TbSint32 x = drawPos.GetX() + RECT.width/2.0f;
                TbSint32 y = drawPos.GetY() + RECT.height/2.0f;

                if (SaveFile::GetInstance().IsVisiblePlayerNumber()) {
                    fontBuffer->Printf(x, y, "%d", value + 1);
                }
            }
        }
    }

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}

/*!
 * 前景描画
 * @author teco
 * @since 2014.08.05
 */
void SideActionJudgeBreakout::onDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
}
