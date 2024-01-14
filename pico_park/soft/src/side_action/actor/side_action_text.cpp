/*!
 * テキスト
 * @author teco
 */

#include "fwd.h"
#include "side_action_text.h"
#include "root.h"
#include "common/cmn_util.h"
#include <base/system/tb_system_setting.h>

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionText::SideActionText()
    : m_FontSize(-1)
    , m_LifeSec(-1.0f)
    , m_HasBorder(TB_FALSE)
{
    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_4);

    m_FontColor = CmnUtil::GetMainColor();
    m_FontBorderColor = TbColor::White();
};

/*! 
 * デストラクタ
 * @author teco
 */
SideActionText::~SideActionText()
{
}

/*! 
 * セットアップ
 * @author teco
 */
void SideActionText::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        ARG_TEXT ,
        ARG_SIZE ,
        ARG_WHITE ,
        ARG_VELO , 
        ARG_BORDER,
        ARG_BACK ,
    };
    SetText(userData.AsString(ARG_TEXT));
    // プレイヤー数置換
    TbSint32 plIndex = TbStrFind(m_Text.GetBuffer(), "[pl]");
    if ( 0 <= plIndex ) {
        TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
        TbStaticString64 front;
        if (0 < plIndex) {
            front = m_Text.Substr(0,plIndex);
        }
        TbStaticString64 back = m_Text.Substr(plIndex + 4);
        m_Text.SetFormatedString("%s%u%s",front.GetBuffer(), playerCount,back.GetBuffer());
    }
    if (ARG_SIZE < userData.GetCount() && 0 < userData.AsSint32(ARG_SIZE)) {
        SetFontSize(userData.AsSint32(ARG_SIZE));
    }
    if (ARG_WHITE < userData.GetCount()) {
        if (userData.AsSint32(ARG_WHITE)){
            m_FontColor = TbColor::White();
            m_FontBorderColor = CmnUtil::GetMainColor();
        }
    }
    if (ARG_VELO < userData.GetCount()) {
        TbFloat32 velo = userData.AsFloat32(ARG_VELO);
        SetVelo(TbVector2(velo,0.0f));
    }
    if (ARG_BORDER < userData.GetCount() && userData.AsSint32(ARG_BORDER)) {
        m_HasBorder = TB_TRUE;
    }
    if (ARG_BACK < userData.GetCount() && userData.AsSint32(ARG_BACK)) {
        m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_BACK_1);
    }
}

/*! 
 * 定期処理
 * @author teco
 */
void SideActionText::OnUpdateAction(TbFloat32 deltatime)
{
    if (0.0f <= m_LifeSec)
    {
        m_LifeSec -= deltatime;
        if (m_LifeSec <= 0.0f)
        {
            Exit();
        }
    }
}

/*! 
 * 前景描画
 * @author teco
 */
void SideActionText::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    if( 0 <= m_FontSize ){
        fontBuffer->SetFontSize(m_FontSize);
    }

    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    fontBuffer->SetColor(m_FontColor);
    if (m_HasBorder) {
        fontBuffer->SetEnableBorder(m_HasBorder);
        fontBuffer->SetBorderColor(m_FontBorderColor);
    }

    fontBuffer->SetPoseMatrix(poseMatrix);
    fontBuffer->Printf( static_cast<TbSint32>(drawPos.GetX()),
                        static_cast<TbSint32>(drawPos.GetY()),m_Text.GetBuffer());

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
