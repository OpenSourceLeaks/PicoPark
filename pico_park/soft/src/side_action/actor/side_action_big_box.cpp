/*!
 * 踏むと倒れるエネミー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_big_box.h"
#include "side_action/side_action_types.h"
#include <crayon/map/cr_action_map.h>
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

static TbFloat32 VELO_X = 1.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionBigBox::SideActionBigBox( Type type )
    : m_PushCountBorderParcent(100)
    , m_PushCountBorderOffset(0)
    , m_PushCount(0)
{
    initialize(SPRITE_RECT[type]);
    SetEnableNotifyJumpRecursive();
}

// コンストラクタ
SideActionBigBox::SideActionBigBox(const TbRectF32Pod& rect)
    : m_PushCountBorderParcent(100)
    , m_PushCountBorderOffset(0)
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
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
        CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    collisionRect.x += 1.0f;
    collisionRect.y += 1.0f;
    collisionRect.width  -= 2.0f;
    collisionRect.height -= 2.0f;
    CrActionCollision* collision = CreateCollision(collisionRect, pushBackTarget, TB_TRUE);
    collision->SetContactAddedCallback(TbCreateMemFunc(this, &Self::onContactAdded));
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
}

/*!
 * セットアップ
 * @author teco
 * @since 2014.06.28
 */
void SideActionBigBox::Setup(const CrActorFactory::UserData& userData)
{
    if (0 < userData.AsSint32(0)) {
        SetPushCountMaxParcent(userData.AsSint32(0));
    } else {
        m_PushCountBorderOffset = userData.AsSint32(0);
    }
}

/*!
 * 最大押し数
 * @author teco
 * @since 2014.06.28
 */
void SideActionBigBox::SetPushCountMaxParcent( TbUint32 parcent )
{
    m_PushCountBorderParcent = parcent;
}

/*!
 * シーンに入った
 * @author teco
 * @since 2013.09.19
 */
void SideActionBigBox::OnEnter(void* userData)
{
    Super::OnEnter(userData);
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    if (stage->IsDarkness()) {
        m_Sprite.GetSprite().SetVisible(TB_FALSE);
        m_SpriteHooker.GetSprite().SetVisible(TB_FALSE);
    }
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionBigBox::OnUpdateAction( TbFloat32 deltatime )
{
    // 接しているオブジェクトが押してきていたら
    const CrActionCollision* collision = GetCollision();
    m_PushCount = 0;

    if (m_BitArray.Test(FLAG_CATCHED)){
        // 掴まれている
        SetVelo(TbVector2());
        return;
    }

    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    CrActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    // 左右への移動
    TbFloat32 veloX = 0.0f;
    SetVeloX(0.0f);
    MoveContactOption moveContactOption;
    moveContactOption.targetCollisionLayerMask = TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER);
    if( isPushed(CR_CONTACT_LEFT) ) {
        SetVeloX(VELO_X);
        if(isEnablePush(this, CR_CONTACT_RIGHT)){
            MoveContactActor(TbVector2(VELO_X,0.0f), CR_CONTACT_RIGHT , moveContactOption);
        }
    }else if( isPushed(CR_CONTACT_RIGHT) ) {
        SetVeloX(-VELO_X);
        if (isEnablePush(this, CR_CONTACT_LEFT)) {
            MoveContactActor(TbVector2(-VELO_X, 0.0f), CR_CONTACT_LEFT, moveContactOption);
        }
    }

    if (collision->IsContacted(CR_CONTACT_UP)) {
//        m_ForceJumpVelo.Clear();
    }else if( !TB_FLOAT32_EQUAL_ZERO(m_ForceJumpVelo.GetX()) ) {
        // 強制ジャンプ中はXを動かす
        AddVelo(m_ForceJumpVelo.GetX(),0.0f);
    }

    // 自由落下
    if( !collision->IsContacted(CR_CONTACT_DOWN) ) {
        AddVelo(0.0f,SIDE_ACTION_GRAVITY);
    }else{
        SetVeloY(0.0f);
        if (!m_BitArray.Test(FLAG_FORCE_JUMP)) {
            m_ForceJumpVelo.Clear();
        }
    }

    if (m_BitArray.Test(FLAG_FORCE_JUMP)) {
        // 強制ジャンプ
        AddVelo(0.0f, m_ForceJumpVelo.GetY());
        m_BitArray.ResetBit(FLAG_FORCE_JUMP);
    }
}


/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionBigBox::NotifyEvent(TbUint32 event, void* param)
{
    if (SIDE_ACTION_EVENT_WARP_INIT_POS == event) {
        SetPosForce( GetInitPos() );
    }else if (SIDE_ACTION_EVENT_WARP == event) {
        // 瞬間移動
        if (param) {
            RequestWarp(*reinterpret_cast<TbVector2*>(param));
        }
    }else if (SIDE_ACTION_EVENT_SET_VISIBLE == event) {
        TbBool* visible = reinterpret_cast<TbBool*>(param);
        m_Sprite.GetSprite().SetVisible(*visible);
        m_SpriteHooker.GetSprite().SetVisible(*visible);
        // 瞬間移動
    }else if(SIDE_ACTION_EVENT_JUMP == event ||
             SIDE_ACTION_EVENT_JUMP_OBJECT == event ) 
    {
        if (param) {
            m_ForceJumpVelo = (*reinterpret_cast<TbVector2*>(param));
            if( GetVelo().GetX() < 0.0f ){
                m_ForceJumpVelo.SetX(TbAbs(m_ForceJumpVelo.GetX())*-1.0f);
            }else{
                m_ForceJumpVelo.SetX(TbAbs(m_ForceJumpVelo.GetX()));
            }
            m_BitArray.SetBit(FLAG_FORCE_JUMP);
        }
    }else if (SIDE_ACTION_EVENT_CATCHED == event) {
        if (param)
        {
            TbSint32 isOn = *reinterpret_cast<TbSint32*>(param);
            m_BitArray.SetBit(FLAG_CATCHED, isOn);
//            GetCollision()->SetUserDataU32(SIDE_ACTION_COLLISION_U32_DISABLE_INERTIA,isOn);
            SetVelo(TbVector2());
            return 1;
        }
    }
    return 0;
}


/*!
 * 任意の方向から押されているか
 * @author teco
 * @since 2013.10.14
 */
TbBool SideActionBigBox::isPushed( CrContactFace type )
{
    // 接しているオブジェクトが押してきていたら
    TbUint32 pushCount = getPushCount(this,type);
    if( pushCount > m_PushCount ){
        m_PushCount = pushCount;
    }
    TbUint32 border = getPushCountBorder();
    if( pushCount >= border){
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 任意の方向に押すことができるか
 * @author teco
 */
TbBool SideActionBigBox::isEnablePush(CrActionActor* actor, CrContactFace type)
{
    const CrActionCollision* collision = actor->GetCollision();
    if (collision->IsContactedMap(type)) {
        return TB_FALSE;
    }
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision(contactCollision, type);
    for (TbUint32 i = 0; i < contactCount; ++i) {
        CrActionActor* next = static_cast<CrActionActor*>(contactCollision[i]->GetActor());
        SideActionBigBox* box = TbDynamicCast<SideActionBigBox*>(next);
        if (box)
        {
            if(!box->isPushed(type)) {
                return TB_FALSE;
            }
        }
        if(!isEnablePush(next,type)) {
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}

/*!
 * 押している数
 * @author teco
 * @since 2013.10.14
 */
TbUint32 SideActionBigBox::getPushCount( CrActionActor* actor , CrContactFace type )
{
    const CrActionCollision* collision = actor->GetCollision();
    if( !collision ) {
        return 0;
    }
    TbUint32 pushCount = 0;
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision( contactCollision , type );
    for( TbUint32 i = 0; i < contactCount; ++i ) {
        CrActionActor* next = static_cast<CrActionActor*>(contactCollision[i]->GetActor());
        switch( type ) {
        case CR_CONTACT_UP:
            if( next->GetVelo().GetY() > TB_FLOAT32_EPSILON ){
                ++pushCount;
            }
            break;
        case CR_CONTACT_DOWN:
            if( next->GetVelo().GetY() < -TB_FLOAT32_EPSILON ){
                ++pushCount;
            }
            break;
        case CR_CONTACT_LEFT:
            if( next->GetVelo().GetX() > TB_FLOAT32_EPSILON ){
                ++pushCount;
            }
            break;
        case CR_CONTACT_RIGHT:
            if( next->GetVelo().GetX() < -TB_FLOAT32_EPSILON ){
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
    fontBuffer->SetFontSize(32);
    fontBuffer->SetPoseMatrix(poseMatrix);
    TbUint32 border = getPushCountBorder();
    // 数
    TbSint32 count = TbMax<TbSint32>(0, border -m_PushCount);
    fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
                       static_cast<TbSint32>(drawPos.GetY()+ m_Rect.y+ m_Rect.height/2),"%d",count);

#if 0
    // ヒント
    if( IsHint() ){
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
        fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX())-64.0f,
                           static_cast<TbSint32>(drawPos.GetY()+ m_Rect.y+ m_Rect.height/2),"2. PUSH→",count);
    }
#endif

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}

/*!
 * 押すのに必要な数
 * @author teco
 * @since 2014.09.12
 */
TbUint32 SideActionBigBox::getPushCountBorder() const
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbFloat32 maxScale = (static_cast<TbFloat32>(m_PushCountBorderParcent) / 100.0f);
    TbFloat32 max = static_cast<TbFloat32>(stage->GetPlayerCount())*maxScale;
    TbUint32 ret = static_cast<TbUint32>(TbCeil(max)) + m_PushCountBorderOffset;
    return ret;
}

/*!
 * 接触開始コールバック
 * @author teco
 * @since 2014.09.12
 */
void SideActionBigBox::onContactAdded(const TbVector2& normal, CrActionCollision::ContactTarget target, const CrActionCollisionInfo& info)
{
    if (normal.GetY() < 0.0f && GetVelo().GetY() < 0.0f) {
        if (info.collision) {
            TbUint32 recursiveMax = info.collision->GetContactRecursiveMax(CR_CONTACT_UP) + 1;
            TbFloat32 veloY = (GetVelo().GetY());
            veloY /= recursiveMax;
            NotifyJump(info.collision, veloY, veloY, SIDE_ACTION_EVENT_JUMP);
        }
        SetVeloY(0.0f);
    }
}
