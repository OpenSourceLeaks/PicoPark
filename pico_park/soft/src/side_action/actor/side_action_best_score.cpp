/*!
 * スコアベース
 * @author teco
 */

#include "fwd.h"
#include "side_action_best_score.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"
#include "common/cmn_util.h"

#include "side_action/side_action_types.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBestScore::SideActionBestScore()
    : Super()
    , m_IsWhite(TB_FALSE)
{
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDraw));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );

    SetEnableScroll(TB_FALSE);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionBestScore::~SideActionBestScore()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionBestScore::Setup(const CrActorFactory::UserData& userData)
{
    if (0 < userData.GetCount()) {
        m_IsWhite = 0 < userData.AsSint32(0);
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBestScore::OnPostAction( TbFloat32 deltatime )
{
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionBestScore::NotifyEvent( TbUint32 event , void* param )
{
    return 0;
}

/*!
 * 前景描画
 * @author teco
 * @since 2014.08.05
 */
void SideActionBestScore::onDraw( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbVector2 pos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    fontBuffer->SetPoseMatrix(TbMatrix::IDENTITY);
    if (m_IsWhite) {
        fontBuffer->SetColor(TbColor::White());
    }else {
        fontBuffer->SetEnableBorder(TB_TRUE);
        fontBuffer->SetColor(CmnUtil::GetMainColor());
    }

    TbStaticString512 text;
    TbUint32 hiscore = SaveFile::GetInstance().GetHiscore(GetStage()->GetId(), Root::GetInstance().GetPlayerCount());
    text.SetFormatedString("BEST:%u", hiscore);
    fontBuffer->Printf(pos.GetX(),pos.GetY(), text.GetBuffer());

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
