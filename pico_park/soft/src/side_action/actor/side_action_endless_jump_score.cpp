/*!
 * エンドレス用のジャンプスコア
 * @author teco
 */

#include "fwd.h"
#include "root.h"
#include "side_action_endless_jump_score.h"

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

static const TbFloat32 DEFAULT_FONT_SIZE = 32.0f;
static const TbUint32  READY_COUNT = 1;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionEndlessJumpScore::SideActionEndlessJumpScore()
    : Super()
    , m_Score(0)
    , m_IsReady(TB_FALSE)
{
    AddComponent(&m_SpriteHooker);

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionEndlessJumpScore::~SideActionEndlessJumpScore()
{
}

/*! 
 * イベント通知
 * @author teco
 */
TbSint32 SideActionEndlessJumpScore::NotifyEvent( TbUint32 event , void* param )
{
    switch (event)
    {
    // カウントアップ
    case SIDE_ACTION_EVENT_COUNT_UP:
        {
            ++m_Score;
            if ( !m_IsReady && READY_COUNT <= m_Score) {
                m_IsReady = TB_TRUE;
            }
            return 0;
        }
        break;
    // 終了
    case SIDE_ACTION_EVENT_GAMEOVER:
        // スコア更新
        {
            if (m_IsReady)
            {
                TbSint32 id = GetStage()->GetId();
                if (SaveFile::GetInstance().SetHiscore(id, Root::GetInstance().GetPlayerCount(),m_Score))
                {
                    // ハイスコア更新
                    SetNewRecord();
                }
                SetResultScore(m_Score);
                NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON, nullptr);
            }
            else 
            {
                m_Score = 0;
            }
            return 0;
        }
        break;
    }
    return Super::NotifyEvent(event,param);
}

/*!
 * 文字描画
 * @author teco
 */
void SideActionEndlessJumpScore::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
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
        "%d", m_IsReady ? m_Score : 0);

    static const TbSint32 OFFSET_Y = 100;
    if (!m_IsReady)
    {
        fontBuffer->Printf(
            static_cast<TbSint32>(drawPos.GetX()),
            static_cast<TbSint32>(drawPos.GetY())+OFFSET_Y,
            "READY?" );
    }
    else if(m_Score == 1)
    {
        fontBuffer->Printf(
            static_cast<TbSint32>(drawPos.GetX()),
            static_cast<TbSint32>(drawPos.GetY()) + OFFSET_Y,
            "GO!");
    }

    fontBuffer->PopSetting();
    fontBuffer->Flush();

}

