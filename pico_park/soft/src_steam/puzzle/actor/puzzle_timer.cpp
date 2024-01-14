/*!
 * スイッチ
 * @author teco
 */

#include "fwd.h"
#include "puzzle_timer.h"
#include "common/stage/stage.h"

#include "system/sys_input.h"
#include "root.h"

static const TbFloat32 WIDTH = 44.0f+1.0f;
static const TbFloat32 HEIGHT = 20.0f+1.0f;

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
PuzzleTimer::PuzzleTimer( Type type )
    : Super()
    , m_Type(type)
    , m_Sec(0.0f)
{
    // スプライト
    m_Sprite.SetZ(CMN_SPRITE_Z_BACK_1);
    m_Sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    m_SpriteHooker.SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawTime));
    m_SpriteHooker.SetZ(CMN_SPRITE_Z_FRONT_6);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
PuzzleTimer::~PuzzleTimer()
{
}

/*!
 * シーンに入った
 * @author teco
 */
void PuzzleTimer::OnEnter( void* userData )
{
    Stage* stage = GetStage();
    m_Sprite.SetTranslate(GetPos().GetX(),GetPos().GetY());
    m_Sprite.Enter(&stage->GetSpriteScene());
    m_SpriteHooker.Enter(&stage->GetSpriteScene());
}

/*!
 * シーンから出た
 * @author teco
 */
void PuzzleTimer::OnExit()
{
    m_Sprite.Exit();
    m_SpriteHooker.Exit();
}

/*!
 * ロジック更新
 * @author teco
 */
void PuzzleTimer::OnUpdate( TbFloat32 deltatime )
{
    switch( m_Type )
    {
    case TYPE_UP:
        m_Sec += deltatime;
        break;
    case TYPE_DOWN:
        m_Sec -= deltatime;
        m_Sec = TbMax(0.0f,m_Sec);
        if( TB_FLOAT32_EQUAL_ZERO(m_Sec) ){
            if( !m_BitArray.Test(FLAG_TIMEUP) ){
                onTimeUp();
            }
        }
        break;
    }
}

/*!
 * タイムアップ
 * @author teco
 */
void PuzzleTimer::onTimeUp()
{
    m_BitArray.SetBit(FLAG_TIMEUP);
}

/*!
 * 前景描画
 * @author teco
 */
void PuzzleTimer::onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector4& drawPos = GetPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    TbSint32 minute = static_cast<TbSint32>(m_Sec)/60;
    TbSint32 sec = static_cast<TbSint32>(TbCeil(m_Sec))%60;

    // 数
    fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()+SPRITE_RECT.x+SPRITE_RECT.width/2),
                       static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y+SPRITE_RECT.height/2),
                       "%02d:%02d",minute,sec);

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
