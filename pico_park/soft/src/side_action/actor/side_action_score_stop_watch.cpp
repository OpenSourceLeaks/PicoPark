/*!
 * 判定型ストップウォッチ
 * @author teco
 */

#include "fwd.h"
#include "side_action_score_stop_watch.h"

#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

static const TbFloat32 WIDTH = 40.0f + 1.0f;
static const TbFloat32 HEIGHT = 15.0f + 1.0f;

static const TbRectF32Pod SPRITE_RECT =
{
    -WIDTH*2.0f , -HEIGHT*2.0f , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT =
{
    CMN_TEX_UV_RECT(320,432,WIDTH,HEIGHT)
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionScoreStopWatch::SideActionScoreStopWatch()
    : Super()
    , m_TotalSec(0.0f)
    , m_IsFailed(TB_FALSE)
    , m_FinishTimer(0.0f)
{
    // スプライト
#if 0
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_BACK_1);
    sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
#endif

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionScoreStopWatch::~SideActionScoreStopWatch()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionScoreStopWatch::OnUpdateAction(TbFloat32 deltatime)
{
    m_FinishTimer += deltatime;
    if (0.1f <= m_FinishTimer)
    {
        if ( m_IsFailed )
        {
            if (2.0f <= m_FinishTimer) 
            {
                GetActionStage()->NotifyEvent(nullptr, SIDE_ACTION_EVENT_RETURN, nullptr);
                m_IsFailed = TB_FALSE;
            }
        }
        else
        {
            if (!IsActive()) {
                TbBool isOn = TB_TRUE;
                TbSint32 id = GetStage()->GetId();
                TbUint32 msec = TbMax<TbUint32>(static_cast<TbUint32>(m_TotalSec * 100.0f) , 1 );
                if (SaveFile::GetInstance().SetBestMillSec(id, Root::GetInstance().GetPlayerCount(), msec))
                {
                    // ハイスコア更新
                    SetNewRecord();
                }
                SetResultTime(msec);
                NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON,&isOn);
            }
        }
    }
    m_TotalSec = 0.0f;
    Super::OnUpdateAction(deltatime);
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionScoreStopWatch::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_PLAYER_PARAM:
        {
            SideActionEventPlayerParam* playerParam = reinterpret_cast<SideActionEventPlayerParam*>(param);
            if ( 0.0f == playerParam->paramF32 ) {
                m_IsFailed = TB_TRUE;
            } else {
                m_TotalSec += playerParam->paramF32;
            }
        }
        break;
    case SIDE_ACTION_EVENT_ALIVE:
        {
            m_FinishTimer = 0.0f;
        }
        break;
    }
    return Super::NotifyEvent(event,param);
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionScoreStopWatch::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetFont();
    fontBuffer->PushSetting();
    fontBuffer->SetFontSize(32.0f);
    fontBuffer->SetPoseMatrix(poseMatrix);
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    // 数
    TbFloat32 x = static_cast<TbSint32>(drawPos.GetX());
    TbFloat32 y = static_cast<TbSint32>(drawPos.GetY());

    if (m_IsFailed)
    {
        fontBuffer->Printf(x, y,
            "TOTAL --:--" );
    }
    else 
    {
        TbFloat32 totalSec = TbMax(m_TotalSec,0.01f);
        TbSint32 sec = TbClamp<TbSint32>(static_cast<TbSint32>(totalSec), 0, 99);
        TbSint32 msec = static_cast<TbSint32>(totalSec*100.0f) % 100;
        fontBuffer->Printf(x, y ,
            "TOTAL %02d:%02d", sec, msec);
    }
    fontBuffer->Printf(x, y+60,"GO TO THE MINIMUM TIME!");

    fontBuffer->PopSetting();
}
