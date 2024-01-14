/*!
 * エンドレス用のスクロールスコア
 * @author teco
 */

#include "fwd.h"
#include "root.h"
#include "side_action_endless_scroll_score.h"
#include "side_action_player.h"
#include "side_action/stage/side_action_stage.h"

static const TbFloat32 WIDTH = 48.0f + 1.0f;
static const TbFloat32 HEIGHT = 15.0f + 1.0f;

static const TbRectF32Pod SPRITE_RECT =
{
    -WIDTH*2.0f , -HEIGHT*2.0f , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT =
{
    CMN_TEX_UV_RECT(464,448,WIDTH,HEIGHT)
};

static const TbFloat32 DEFAULT_FONT_SIZE = 32.0f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionEndlessScrollScore::SideActionEndlessScrollScore()
    : Super()
    , m_Score(0)
{
    AddComponent(&m_SpriteHooker);

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    SetEnableScroll(TB_FALSE);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionEndlessScrollScore::~SideActionEndlessScrollScore()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionEndlessScrollScore::OnUpdateAction(TbFloat32 deltatime)
{
    TbFloat32 scroll = GetStage()->GetScrollX();
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    m_Score = static_cast<TbUint32>(scroll/10.0f);

    TbBool isAlive = TB_TRUE;
    TbUint32 playerCount = stage->GetPlayerCount();
    for (TbUint32 i = 0; i < playerCount; ++i)
    {
        SideActionPlayer::State state = stage->GetPlayer(i)->GetState();
        if (!stage->GetPlayer(i)->IsAlive())
        {
            isAlive = TB_FALSE;
            break;
        }
    }
    if (!isAlive) { 
        if (!IsActive())
        {
            TbSint32 id = GetStage()->GetId();
            if (SaveFile::GetInstance().SetHiscore(id, Root::GetInstance().GetPlayerCount(), m_Score))
            {
                // ハイスコア更新
                SetNewRecord();
            }
            SetResultScore(m_Score);
            NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON, nullptr);
        }
    }
    // GetStage()->IsRequestedRetry()
    // プレイヤーは落ちて死ぬので、リトライリクエストをフックする
}

/*! 
 * イベント通知
 * @author teco
 */
TbSint32 SideActionEndlessScrollScore::NotifyEvent( TbUint32 event , void* param )
{
    return Super::NotifyEvent(event,param);
}

/*!
 * 文字描画
 * @author teco
 */
void SideActionEndlessScrollScore::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbVector2 drawPos = GetDrawPos();
    drawPos.Add(SPRITE_RECT.x + SPRITE_RECT.width / 2, SPRITE_RECT.y + SPRITE_RECT.height / 2);
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetFontSize(DEFAULT_FONT_SIZE);
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    // 数
    fontBuffer->Printf(
        static_cast<TbSint32>(drawPos.GetX()),
        static_cast<TbSint32>(drawPos.GetY()),
        "%d", m_Score );

    fontBuffer->PopSetting();
    fontBuffer->Flush();

}

