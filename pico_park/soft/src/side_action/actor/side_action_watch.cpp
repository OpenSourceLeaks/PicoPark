/*!
 * 時計
 * @author teco
 */

#include "fwd.h"
#include "side_action_watch.h"

#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

#include <framework/sub/tb_framework_simple.h>

static const TbFloat32 WIDTH = 22.0f+1.0f;
static const TbFloat32 HEIGHT = 9.0f+1.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    0 , -HEIGHT*2.0f , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(352,80,WIDTH,HEIGHT)
};

static const TbFontCharInfoSjis FONT_SJIS[] =
{
#if 1
    { "0" , CMN_TEX_UV_RECT(376,80,7,10) } , 
    { "1" , CMN_TEX_UV_RECT(383,80,7,10) } , 
    { "2" , CMN_TEX_UV_RECT(390,80,7,10) } , 
    { "3" , CMN_TEX_UV_RECT(397,80,7,10) } , 
    { "4" , CMN_TEX_UV_RECT(404,80,7,10) } , 
    { "5" , CMN_TEX_UV_RECT(411,80,7,10) } , 
    { "6" , CMN_TEX_UV_RECT(418,80,7,10) } , 
    { "7" , CMN_TEX_UV_RECT(425,80,7,10) } , 
    { "8" , CMN_TEX_UV_RECT(432,80,7,10) } , 
    { "9" , CMN_TEX_UV_RECT(439,80,7,10) } , 
    { ":" , CMN_TEX_UV_RECT(446,80,4,10) } , 
    { "?" , CMN_TEX_UV_RECT(450,80,7,10) } , 
#else
    { "0" , CMN_TEX_UV_RECT(320,450,12,18) } , 
    { "1" , CMN_TEX_UV_RECT(334,450,12,18) } , 
    { "2" , CMN_TEX_UV_RECT(348,450,12,18) } , 
    { "3" , CMN_TEX_UV_RECT(362,450,12,18) } , 
    { "4" , CMN_TEX_UV_RECT(376,450,12,18) } , 
    { "5" , CMN_TEX_UV_RECT(390,450,12,18) } , 
    { "6" , CMN_TEX_UV_RECT(320,468,12,18) } , 
    { "7" , CMN_TEX_UV_RECT(334,468,12,18) } , 
    { "8" , CMN_TEX_UV_RECT(348,468,12,18) } , 
    { "9" , CMN_TEX_UV_RECT(362,468,12,18) } , 
    { ":" , CMN_TEX_UV_RECT(376,468,6,18) } , 
    { "?" , CMN_TEX_UV_RECT(450,468,12,18) } , 
#endif
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionWatch::SideActionWatch()
    : Super()
    , m_Font(nullptr)
    , m_FontBuffer(nullptr)
{
    TbFontCreateParamSjis param;
    param.common.charCount = TB_ARRAY_COUNT_OF(FONT_SJIS);
    param.common.defaultFontSize = 10;
    param.common.uvBaseSize = TbDimensionF32(CMN_TEX_POS(10.0f),CMN_TEX_POS(10.0f));
    param.charInfoList = FONT_SJIS;
    m_Font = new TbFontSjis(param);
    m_Font->SetTexture(Root::GetInstance().GetCommonTexture());
    m_FontBuffer = new TbFontBuffer2d(2048,TbFrameworkSimple::RENDER_PASS_2D);
    m_FontBuffer->SetFont(m_Font);
    m_FontBuffer->SetFontSize(40);
    m_FontBuffer->SetColor(TbColor::White());

    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_6);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawTime));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionWatch::~SideActionWatch()
{
    TB_SAFE_DELETE(m_FontBuffer);
    TB_SAFE_DELETE(m_Font);
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionWatch::onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    m_FontBuffer->PushSetting();
    m_FontBuffer->SetPoseMatrix(GetStage()->GetPoseMatrix());
    m_FontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_LEFT);
    m_FontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    TbFloat32 playSec = GetStage()->GetPlaySec();
    TbSint32 minute = static_cast<TbSint32>(playSec)/60;
    TbSint32 sec = static_cast<TbSint32>(playSec)%60;

    // 数
    m_FontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()+SPRITE_RECT.x+SPRITE_RECT.width+8.0f),
                         static_cast<TbSint32>(drawPos.GetY()),
                         "%02d:%02d",minute,sec);

    m_FontBuffer->PopSetting();
    m_FontBuffer->Flush();
}
