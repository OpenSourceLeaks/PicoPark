/*!
 * 踏むと倒れるエネミー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_big_box.h"
#include "side_action/side_action_types.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT[] = 
{
    { -40.0f , -703.0f , 80.0f , 703.0f } ,
    { -48.0f , -128.0f , 96.0f , 128.0f } ,
    { -48.0f , -96     , 96.0f , 96 } ,
    { -25.0f , -48.0f  , 48.0f , 48.0f }
};

static const TbRectF32Pod SPRITE_UV_PLACE_RECT[] = 
{
    { CMN_TEX_UV_RECT(512-48,32,16,16) } , // PLACE_LU
    { CMN_TEX_UV_RECT(512-48,48,16,16) }   , // PLACE_LC
    { CMN_TEX_UV_RECT(512-48,64,16,16) }   , // PLACE_LB
    { CMN_TEX_UV_RECT(512-32,32,16,16) }   , // PLACE_UP
    { CMN_TEX_UV_RECT(512-32,48,16,16) } , // PLACE_IN
    { CMN_TEX_UV_RECT(512-32,64,16,16) } , // PLACE_BT
    { CMN_TEX_UV_RECT(512-16,32,16,16) } , // PLACE_RU
    { CMN_TEX_UV_RECT(512-16,48,16,16) } , // PLACE_RC
    { CMN_TEX_UV_RECT(512-16,64,16,16) } , // PLACE_RB
};

TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT)==TbElasticSprite::PLACE_MAX);

static TbUint32 PUSH_COUNT =1;
static TbFloat32 VELO_X = 1.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionBigBox::SideActionBigBox( Type type )
    : m_PushCountMax(PUSH_COUNT)
    , m_PushCount(0)
{
    initialize(SPRITE_RECT[type]);
}

// コンストラクタ
SideActionBigBox::SideActionBigBox(const TbRectF32Pod& rect)
    : m_PushCountMax(PUSH_COUNT)
    , m_PushCount(0)
{
    initialize(rect);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionBigBox::~SideActionBigBox()
{
}

/*!
 * 初期化
 * @author teco
 */
void SideActionBigBox::initialize(const TbRectF32Pod& rect)
{
    m_Rect = rect;

    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);
    // スプライト
    AddComponent(&m_Sprite);
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(24.0f, 24.0f);
    spriteParam.rect = m_Rect;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT); ++i) {
        spriteParam.rectUV[i] = SPRITE_UV_PLACE_RECT[i];
    }
    m_Sprite.GetSprite().Initialize(spriteParam);
    m_Sprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_4);

    // 文字
    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this, &Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);

    // コリジョン
    TbRectF32Pod  collisionRect = m_Rect;
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
        ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision(m_Rect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_ENEMY);
}

/*!
 * 最大押し数
 * @author teco
 * @since 2014.06.28
 */
void SideActionBigBox::SetPushCountMax( TbUint32 parcent )
{
    TbFloat32 maxScale = (static_cast<TbFloat32>(parcent)/100.0f);
    TbFloat32 max = static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount())*maxScale;
    m_PushCountMax = static_cast<TbUint32>(TbCeil(max));
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionBigBox::OnUpdateAction( TbFloat32 deltatime )
{
    // 接しているオブジェクトが押してきていたら
    const ActionCollision* collision = GetCollision();
    m_PushCount = 0;

    ActionStage* stage = GetActionStage();
    ActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    // 左右への移動
    TbFloat32 veloX = 0.0f;
    SetVeloX(0.0f);
    if( isPushed(CMN_CONTACT_LEFT) ) {
        SetVeloX(VELO_X);
//        MoveContactActor(TbVector2(VELO_X,0.0f),CMN_CONTACT_LEFT);

        // 上に乗っかっているやつも動かす
//        MoveContactActor(TbVector2(VELO_X,0.0f),CMN_CONTACT_UP);
    }else if( isPushed(CMN_CONTACT_RIGHT) ) {
        SetVeloX(-VELO_X);
//        MoveContactActor(TbVector2(-VELO_X,0.0f),CMN_CONTACT_RIGHT);

        // 上に乗っかっているやつも動かす
//        MoveContactActor(TbVector2(-VELO_X,0.0f),CMN_CONTACT_UP);
    }

    // 自由落下
    if( !collision->IsContacted(CMN_CONTACT_DOWN) ) {
        AddVelo(0.0f,SIDE_ACTION_GRAVITY);
    }else{
        SetVeloY(0.0f);
    }
}


/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionBigBox::NotifyEvent(TbUint32 event, void* param)
{
    if (SIDE_ACTION_EVENT_WARP_INIT_POS == event) {
        SetPos( GetInitPos() );
    }
    return 0;
}


/*!
 * 任意の方向から押されているか
 * @author teco
 * @since 2013.10.14
 */
TbBool SideActionBigBox::isPushed( CmnContactFace type )
{
    // 接しているオブジェクトが押してきていたら
    TbUint32 pushCount = getPushCount(this,type);
    if( pushCount > m_PushCount ){
        m_PushCount = pushCount;
    }
    if( pushCount >= m_PushCountMax ){
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 押している数
 * @author teco
 * @since 2013.10.14
 */
TbUint32 SideActionBigBox::getPushCount( ActionActor* actor , CmnContactFace type )
{
    const ActionCollision* collision = actor->GetCollision();
    if( !collision ) {
        return 0;
    }
    TbUint32 pushCount = 0;
    ActionCollision* contactCollision[ActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision( contactCollision , type );
    for( TbUint32 i = 0; i < contactCount; ++i ) {
        ActionActor* next = static_cast<ActionActor*>(contactCollision[i]->GetActor());
        switch( type ) {
        case CMN_CONTACT_UP:
            if( next->GetVelo().GetY() > TB_FLOAT32_EPSILON ){
                ++pushCount;
            }
            break;
        case CMN_CONTACT_DOWN:
            if( next->GetVelo().GetY() < TB_FLOAT32_EPSILON ){
                ++pushCount;
            }
            break;
        case CMN_CONTACT_LEFT:
            if( next->GetVelo().GetX() > TB_FLOAT32_EPSILON ){
                ++pushCount;
            }
            break;
        case CMN_CONTACT_RIGHT:
            if( next->GetVelo().GetX() < TB_FLOAT32_EPSILON ){
                ++pushCount;
            }
            break;
        }
        pushCount += getPushCount(next,type);
    }
    return pushCount;
}


/*!
 * 前景描画
 * @author teco
 * @since 2014.09.12
 */
void SideActionBigBox::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{

    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
//    fontBuffer->SetFontSize(64);
    // 数
    TbSint32 count = TbMax<TbSint32>(0,m_PushCountMax-m_PushCount);
    fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
                       static_cast<TbSint32>(drawPos.GetY()+ m_Rect.y+ m_Rect.height/2),"%d",count);

    // ヒント
    if( IsHint() ){
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
        fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX())-64.0f,
                           static_cast<TbSint32>(drawPos.GetY()+ m_Rect.y+ m_Rect.height/2),"2. PUSH→",count);
    }

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
