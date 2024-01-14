/*!
 * シンプルな矩形
 * @author teco
 */

#include "fwd.h"
#include "side_action_rect.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"
#include <sound/tb_sound.h>

static const TbRectF32Pod SPRITE_UV_PLACE_RECT[] =
{
#if 0
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,0     ,512,512) } , // PLACE_LU
#else
    { CMN_TEX_UV_RECT(0       ,528     ,16,16) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,528 + 16,16,16) } , // PLACE_LC
    { CMN_TEX_UV_RECT(0       ,528 + 32,16,16) } , // PLACE_LB
    { CMN_TEX_UV_RECT(16      ,528     ,16,16) } , // PLACE_UP
    { CMN_TEX_UV_RECT(16      ,528 + 16,16,16) } , // PLACE_IN
    { CMN_TEX_UV_RECT(16      ,528 + 32,16,16) } , // PLACE_BT
    { CMN_TEX_UV_RECT(32      ,528     ,16,16) } , // PLACE_RU
    { CMN_TEX_UV_RECT(32      ,528 + 16,16,16) } , // PLACE_RC
    { CMN_TEX_UV_RECT(32      ,528 + 32,16,16) } , // PLACE_RB
#endif
};

static const TbRectF32Pod SPRITE_UV_PLACE_RECT_DAMAGE[] =
{
    { CMN_TEX_UV_RECT(160       ,528     ,16,16) } , // PLACE_LU
    { CMN_TEX_UV_RECT(160       ,528 + 16,16,16) } , // PLACE_LC
    { CMN_TEX_UV_RECT(160       ,528 + 32,16,16) } , // PLACE_LB
    { CMN_TEX_UV_RECT(160 + 16  ,528     ,16,16) } , // PLACE_UP
    { CMN_TEX_UV_RECT(160 + 16  ,528 + 16,16,16) } , // PLACE_IN
    { CMN_TEX_UV_RECT(160 + 16  ,528 + 32,16,16) } , // PLACE_BT
    { CMN_TEX_UV_RECT(160 + 32  ,528     ,16,16) } , // PLACE_RU
    { CMN_TEX_UV_RECT(160 + 32  ,528 + 16,16,16) } , // PLACE_RC
    { CMN_TEX_UV_RECT(160 + 32  ,528 + 32,16,16) } , // PLACE_RB
};

TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT) == TbElasticSprite::PLACE_MAX);


/*! 
 * コンストラクタ
 * @author teco
 */
SideActionRect::SideActionRect(const TbRectF32& rect )
    : m_Type(TYPE_NORMAL)
    , m_RefCounter(0)
{
    m_Rect = rect;

    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);
    m_Sprite.GetSprite().SetName("Rect");
    // スプライト
    AddComponent(&m_Sprite);
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(32.0f, 32.0f);
    spriteParam.rect = m_Rect;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT); ++i) {
        spriteParam.rectUV[i] = SPRITE_UV_PLACE_RECT[i];
    }
    m_Sprite.GetSprite().Initialize(spriteParam);
    m_Sprite.GetSprite().SetZ(CMN_SPRITE_Z_BACK_2);

    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision(rect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT_2);
    collision->SetFixedPos(TB_TRUE);
    collision->SetActive(TB_FALSE);
};

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionRect::~SideActionRect()
{
}

/*! 
 * シーンに入った
 * @author teco
 */
void SideActionRect::OnEnter(void* userData)
{
    Super::OnEnter(userData);
    m_RefCounter = 0;
    if (TYPE_SWITCH == m_Type) {
        SetVisible(TB_FALSE);
        GetCollision()->SetActive(TB_FALSE);
    }
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionRect::OnUpdateAction(TbFloat32 deltatime)
{
    if( m_Sprite.GetSprite().IsVisible() )
    {
        CrActionCollision* collision = GetCollision();
        if (collision && !collision->IsActive()) {
            if (!collision->GetScene()->IsCollidedByPushBackTarget(*collision)) {
                collision->SetActive(TB_TRUE);
            }
        }
    }
    if( TYPE_DAMAGE == m_Type )
    {
        CrActionCollision* collision = GetCollision();
        TbUint32 count = collision->GetContactCount();
        for (TbUint32 i = 0; i < count; ++i)
        {
            CrActionCollision* target = collision->GetContactCollision(i);
            if (target->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER ) {
                GetStage()->RequestScreenQuake();
                TbPlaySound("hit");
                target->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_DEAD,nullptr);
            }
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionRect::NotifyEvent(TbUint32 event, void* param)
{
    if( TYPE_SWITCH == m_Type )
    {
        if (SIDE_ACTION_EVENT_SWITCH_ON == event) {
            ++m_RefCounter;
            if( 1 == m_RefCounter ){
                SetVisible(TB_TRUE);
                // GetCollision()->SetActive(TB_TRUE);
            }
        }
        else if (SIDE_ACTION_EVENT_SWITCH_OFF == event) {
            if( TB_VERIFY(0<m_RefCounter) )
            {
                --m_RefCounter;
                if( 0 == m_RefCounter ){
                    SetVisible(TB_FALSE);
                    GetCollision()->SetActive(TB_FALSE);
                }
            }
        }
    }
    if (SIDE_ACTION_EVENT_ALIVE_CONFIRM == event)
    {
        // 画面左端からいなくなってたら、死んでいる状態として返す
        // 殺すかどうかは呼び出し元の判断
        TbFloat32 x = GetDrawPos().GetX() + m_Rect.x + m_Rect.width;
        if (x < 0.0f) {
            return TB_FALSE;
        }
        return TB_TRUE;
    }
    else if (SIDE_ACTION_EVENT_SUSPEND == event)
    {
        Suspend();
    }
    else if (SIDE_ACTION_EVENT_SET_VISIBLE == event)
    {
        TbBool* visible = reinterpret_cast<TbBool*>(param);
        m_Sprite.GetSprite().SetVisible(*visible);
    }
    return 0;
}

/*!
 * タイプ設定
 * @author teco
 */
void SideActionRect::SetType(Type type)
{
    m_Type = type;
    if (m_Type == TYPE_DAMAGE) {
        TbElasticSprite::UvParam uvParam;
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT_DAMAGE); ++i) {
            uvParam.rectUV[i] = SPRITE_UV_PLACE_RECT_DAMAGE[i];
        }
        m_Sprite.GetSprite().SetUV(uvParam);
    }
}
