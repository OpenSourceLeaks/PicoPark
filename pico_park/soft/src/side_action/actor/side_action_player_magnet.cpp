/*!
 * プレイヤー用磁石
 * @author teco
 */

#include "fwd.h"
#include "side_action_player_magnet.h"
#include "side_action_player.h"
#include "side_action_big_box.h"
#include "side_action/stage/side_action_stage.h"
#include <crayon/map/cr_action_map.h>
#include "common/cmn_util.h"
#include <base/math/tb_random.h>
#include "root.h"


//////////////////////////////////////////
// SideActionPlayerMagnet
//////////////////////////////////////////

static TbFloat32 GUN_WIDTH = 11.0f;
static TbFloat32 GUN_HEIGHT = 15.0f;
static TbFloat32 GUN_OFFSET_Y = -10.0f;
static TbFloat32 GUN_CENTER_OFFSET_Y = -20.0f;
static const TbRectF32Pod SPRITE_RECT =
{
    -GUN_WIDTH , -GUN_HEIGHT*1.5f , GUN_WIDTH*2.0f , GUN_HEIGHT*2.0f ,
};

static const TbRectF32Pod SPRITE_UV_RECT =
{
    CMN_TEX_UV_RECT(208,496,GUN_WIDTH,GUN_HEIGHT)
};

static TB_NDEBUG_CONST TbRectF32Pod COLLISION_RECT =
{
    20.0f , -40.0f+ GUN_OFFSET_Y , 110.0f , 80.0f
};

static TB_NDEBUG_CONST TbVector2 OFFSET_POS =
{
    TbVector2(20.0f,-10.0f) , 
};

struct CandidateCompare
{
    CandidateCompare( const TbVector2& base )
        : basePos(base)
    {}
    // 比較
    TbBool operator() (CrActionActor* left, CrActionActor* right)
    {
        TbFloat32 l = (left->GetPosXY() - basePos).LengthSq();
        TbFloat32 r = (right->GetPosXY() - basePos).LengthSq();
        return l < r;
    }
    TbVector2 basePos;
};

static TB_NDEBUG_CONST TbFloat32 APPROACH_SPEED_X = 0.03f;
static TB_NDEBUG_CONST TbFloat32 APPROACH_SPEED_X_MIN = 1.f;
static TB_NDEBUG_CONST TbFloat32 APPROACH_SPEED_Y = 0.08f;
static TB_NDEBUG_CONST TbFloat32 APPROACH_SPEED_Y_MIN = 2.f;

/////////////////////////////
// SideActionPlayerMagnet
/////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerMagnet::SideActionPlayerMagnet( SideActionPlayer* player )
    : Super()
    , m_Player(player)
    , m_Target(nullptr)
    , m_OffsetUpCounter(0)
{
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
    sprite.SetMaterial(&m_Material);
    m_Material.SetColor(CmnUtil::GetPlayerColor(m_Player->GetPlayerIndex()));

    // 吸引パーティクル
    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this, &Self::onDrawParticle));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_BACK_1);

    // コリジョン設定
    TbUint32 pushBackTarget = 0;
    CrActionCollision* collision = CreateCollision(COLLISION_RECT, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
    collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));
    m_Player->Retain();
    m_Player->Equip(SideActionPlayer::EQUIO_PART_HAND,this);
//    m_Player->SetFloatingFunction( TbCreateMemFunc(this,&Self::IsFloating) );

    // アクターレイヤー設定
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST2);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerMagnet::~SideActionPlayerMagnet()
{
    if(m_Player){
        m_Player->Equip(SideActionPlayer::EQUIO_PART_HAND, nullptr);
    }
    TB_SAFE_RELEASE(m_Player);
}

/*!
 * ターゲットプレイヤー設定
 * @author teco
 */
void SideActionPlayerMagnet::SetTarget( CrActionActor* actor )
{
    m_Target = actor;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionPlayerMagnet::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_GET_PLAYERINDEX == event )
    {
        return m_Player->GetPlayerIndex();
    }
    return 0;
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionPlayerMagnet::OnUpdateAction(TbFloat32 deltatime)
{
    static TB_NDEBUG_CONST TbFloat32 INTERVAL_SEC = 0.6f;
    TbBool isOn = isInputOn();
    m_BitArray.SetBit(FLAG_ACTIVE, isOn);
    updateParticle();

    if (isOn)
    {
        if(m_TimingSec <= 0.0f )
        {
            TbPlaySound("magnet");
            m_TimingSec = INTERVAL_SEC;
        }
        m_TimingSec -= deltatime;
    }
    else
    {
        m_TimingSec = 0.0f;
    }

    if (m_Target)
    {
        if (!isOn ||
            (TbFind(m_Candidate, m_Target) == m_Candidate.End()))
        {
            TbBool isOn = TB_FALSE;
            // キャッチ解除
            m_BitArray.ResetBit(FLAG_CATCH);
            m_Target->NotifyEvent(SIDE_ACTION_EVENT_CATCHED, &isOn);
            SideActionPlayer* player = getTargetPlayer();
            if (player) {
                player->SetEnablePushBackVec(TB_TRUE);
            }
            m_Target = nullptr;
        }
        else
        {
            TbFloat32 approachRateX = APPROACH_SPEED_X;
            TbFloat32 approachRateY = APPROACH_SPEED_Y;
            TbVector2 velo;
            if ( m_BitArray.Test(FLAG_CATCH) ) {
                velo = m_Player->GetVelo();  
                approachRateX *= 2.0f;
                approachRateY *= 2.0f;
            }
            if (m_Target->GetCollision()->IsContactedMap(CR_CONTACT_LEFT) ||
                m_Target->GetCollision()->IsContactedMap(CR_CONTACT_RIGHT))
            {
                m_OffsetUpCounter = 3;
            }
            else
            {
                if (0 < m_OffsetUpCounter) {
                    --m_OffsetUpCounter;
                }
            }

            // キャッチし続けている限り近づける
            TbVector2 targetDestPos = getTargetDestPos();
            TbVector2 delta = targetDestPos - (m_Target->GetPosXY()+velo);
            TbBool isCatch = TB_TRUE;
            if ( TbAbs(delta.GetX()) <= APPROACH_SPEED_X_MIN) {
                velo.Add(delta.GetX(),0.0f);
            } else {
                velo.Add(TbSign(delta.GetX())*TbMax(APPROACH_SPEED_Y_MIN,TbAbs(delta.GetX())*approachRateX),0.0f);
                isCatch = TB_FALSE;
            }

            if (TbAbs(delta.GetY()) <= APPROACH_SPEED_Y_MIN) {
                velo.Add(0.0f,delta.GetY());
            } else {
                velo.Add(0.0f,TbSign(delta.GetY())*TbMax(APPROACH_SPEED_Y_MIN,TbAbs(delta.GetY())*approachRateY));
                isCatch = TB_FALSE;
            }
            
            // velo += m_Player->GetMoveByContact();

            if (!m_BitArray.Test(FLAG_CATCH)) {
                m_BitArray.SetBit(FLAG_CATCH, isCatch);
            }

            // 接触チェック
            if (0.0f < velo.GetX()) {
                if (m_Target->GetCollision()->IsContacted(CR_CONTACT_RIGHT)) {
                    velo.SetX(0.0f);
                }
            }
            else {
                if (m_Target->GetCollision()->IsContacted(CR_CONTACT_LEFT)) {
                    velo.SetX(0.0f);
                }
            }
#if 0
            if (0.0f < velo.GetY()) {
                if (m_Target->GetCollision()->IsContacted(CR_CONTACT_DOWN)) {
                    velo.SetY(0.0f);
                }
            }
            else
#endif
            {
                if (m_Target->GetCollision()->IsContacted(CR_CONTACT_UP)) {
                    if (velo.GetY() < 0.0f) {
                        velo.SetY(0.0f);
                    }
                }
            }

            if (isCatchedRelative()) {
                m_Target->SetVeloX(0.0f);
                if (m_Player->IsFloating()) {
                    // 重力を与える
#if 1
                    TbVector2 playerVelo = m_Player->GetVelo();
                    TbVector2 gravityVelo = m_Player->CalcGravity(playerVelo);
                    m_Player->SetVelo(playerVelo+gravityVelo);
#endif
                }
            }else {
//                m_Target->SetVelo(TbVector2());
                m_Target->SetVelo(velo);
            }
            m_Target->AddPos(velo);
        }
    }
    else if (!m_Candidate.IsEmpty())
    {
        if (isOn)
        {
            TbInsertionSort(m_Candidate.Begin(), m_Candidate.End(), CandidateCompare(m_Player->GetPosXY()));
            TbForIterator(it, m_Candidate) 
            {
                // 近い順から問い合わせる
                TbBool isOn = TB_TRUE;
                if ((*it)->NotifyEvent(SIDE_ACTION_EVENT_CATCHED,&isOn)) {
                    m_Target = (*it);
                    SideActionPlayer* player = getTargetPlayer();
                    if (player) {
                        player->SetEnablePushBackVec(TB_FALSE);
                    }
                    break;
                }
            }
        }
    }
    m_Candidate.Clear();
    m_Player->SetEnableChangeLookDir(m_Target == nullptr);
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionPlayerMagnet::OnPostAction( TbFloat32 deltatime )
{
    static TB_NDEBUG_CONST TbFloat32 CATCH_RESET_LENGTH = 32.0f;

    GetMainSprite().SetVisible(m_Player->IsVisible());
    TbVector4 pos = m_Player->GetPos();
    CrActionCollision* collision = GetCollision();

    // スプライト反映
    TbSprite& sprite = GetMainSprite();
    if (m_Player->IsLookingRight()) {
        pos.Add(OFFSET_POS.GetX(), OFFSET_POS.GetY(), 0.0f);
        SetMainSpriteOffsetPos(TbVector2());
        sprite.SetScale(1.0f, 1.0f);
    } else {
        pos.Add(-OFFSET_POS.GetX()- COLLISION_RECT.x*2 - COLLISION_RECT.width, OFFSET_POS.GetY(), 0.0f);
        SetMainSpriteOffsetPos(TbVector2(COLLISION_RECT.x*2+COLLISION_RECT.width,0.0f));
        sprite.SetScale(-1.0f, 1.0f);
        //        collision->Initialize(
    }
    SetPos(pos);
    sprite.SetTranslate(GetDrawPos());

    // 一定以上離れるとリセット
    if (m_BitArray.Test(FLAG_CATCH))
    {
        if (m_Target) {
            TbVector2 delta = getTargetDestPos() - (m_Target->GetPosXY());
            if ( CATCH_RESET_LENGTH < delta.Length() ) {
                m_BitArray.ResetBit(FLAG_CATCH);
            }
        }
    }
}

/*!
* 接触による移動
* @author teco
*/
void SideActionPlayerMagnet::OnMoveByContact(const TbVector2& mov, const TbVector2& normal, ActorSet* set, const MoveContactOption& option)
{
    if (m_Target)
    {
        m_Target->MoveByContactActor(mov, normal, set, option);
    }
}

/*! 
 * 衝突時コールバック
 * @author teco
 */
void SideActionPlayerMagnet::onCollided(CrActionCollision* candidate)
{
    static TB_NDEBUG_CONST TbFloat32 LIMIT_ANGLE_DEG = 40.0f;
    CrActionActor* actor = candidate->GetActor();
    TbBool isCandidage = TB_FALSE;
    if(actor)
    {
        if (candidate->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER && actor != m_Player)
        {
            isCandidage = TB_TRUE;
        }
        else if (TbDynamicCast<SideActionBigBox*>(actor))
        {
            isCandidage = TB_TRUE;
        }
    }
    if( isCandidage )
    {
        TbVector2 delta = actor->GetPosXY() - m_Player->GetPosXY();
        delta.Normalize();
        if ( TbAbs(delta.GetY()) < TbSin(TbDegToRad(LIMIT_ANGLE_DEG))) {
            m_Candidate.PushBack(candidate->GetActor());
        }
    }
}

/*! 
 * 対象の目標座標取得
 * @author teco
 */
TbVector2 SideActionPlayerMagnet::getTargetDestPos() const
{
    static TB_NDEBUG_CONST TbFloat32 ERROR_Y = 0.0f;
    static TB_NDEBUG_CONST TbFloat32 OFFSET_Y = -GUN_OFFSET_Y+ERROR_Y;
    static TB_NDEBUG_CONST TbFloat32 OFFSET_X = 10.0f;
    TbVector2 res;
    if (m_Target) 
    {
        const CrActionCollision* targetCollision = m_Target->GetCollision();
        res = GetPosXY() + GetMainSpriteOffsetPos();
        res.Add(0.0f, OFFSET_Y);
        const TbRectF32& rect = targetCollision->GetRect();
        if( m_Player->IsLookingRight() ){
            res.Add(OFFSET_X -rect.x,0.0f);
        }else {
            res.Add(-OFFSET_X -rect.x-rect.width,0.0f);
        }
    } 
    return res;
}

/*! 
 * パーティクル描画
 * @author teco
 */
void SideActionPlayerMagnet::onDrawParticle(const TbMatrix& poseMatrix, TbSpriteMaterial* material)
{
    static const TbFloat32 PIXEL_SIZE = 4.0f;
    if (m_Particle.IsEmpty()) {
        return;
    }
    TbVector2 basePos = m_Player->GetDrawPos();
    TbForIterator(it, m_Particle)
    {
        TbVector2 pos = basePos + it->pos;
        TbRectF32 rect = TbRectF32(pos.GetX(),
            pos.GetY() + GUN_CENTER_OFFSET_Y,
            PIXEL_SIZE,
            PIXEL_SIZE);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512 - 8, 0, 8, 8));
        TbMatrix mtx = TbMatrix::IDENTITY;
        TbColor color = CmnUtil::GetPlayerColor(m_Player->GetPlayerIndex());
        TbUint32 index = CrInput::GetPlayerPadIndex(m_Player->GetPlayerIndex());
        if (2 == index || 3 == index || 4 == index || 7 == index)
        {
            // 黄色だけ補正
            // 本来は*演算子を実装すべきだが時期が時期なので
            color.SetRedF32(color.GetRedF32()*0.7f);
            color.SetGreenF32(color.GetGreenF32()*0.7f);
            color.SetBlueF32(color.GetBlueF32()*0.7f);
            color.SetAlphaF32(it->alpha);
        }
        material->Draw(rect, uvRect, color, poseMatrix);
    }
}

/*! 
 * パーティクル更新
 * @author teco
 */
void SideActionPlayerMagnet::updateParticle()
{
    static TB_NDEBUG_CONST TbFloat32 FADE_OUT_SPEED = 0.02f;
    static TB_NDEBUG_CONST TbFloat32 FADE_OUT_DIST = 30.0f;
    static TB_NDEBUG_CONST TbFloat32 OFFSET_X = 18.0f;
    static TB_NDEBUG_CONST TbFloat32 SPEED_Y_SCALE = 0.5f;
    static TB_NDEBUG_CONST TbFloat32 SPEED_Y_MIN = 0.0f;
    static TB_NDEBUG_CONST TbFloat32 CREATE_RATE = 0.8f;
    TbBool isOn = m_BitArray.Test(FLAG_ACTIVE);

    // 移動
    auto it = m_Particle.Begin();
    while( it != m_Particle.End() )
    {
        TbFloat32 approachRateX = APPROACH_SPEED_X;
        TbFloat32 approachRateY = APPROACH_SPEED_Y * SPEED_Y_SCALE;
        TbVector2 velo;

        // キャッチし続けている限り近づける
        TbVector2 delta = -it->pos;

        if(it->isActive)
        {
            if (m_Player->IsLookingRight())
            {
                delta.Add(OFFSET_X,0.0f);
                it->isActive = delta.GetX() < 0.0f;
            }
            else
            {
                delta.Add(-OFFSET_X, 0.0f);
                it->isActive = 0.0f < delta.GetX();
            }
        }

        if (it->isActive)
        {
            if (TbAbs(delta.GetX()) <= SPEED_Y_MIN) {
                velo.Add(delta.GetX(), 0.0f);
            } else {
                velo.Add(TbSign(delta.GetX())*TbMax(SPEED_Y_MIN, TbAbs(delta.GetX())*approachRateX), 0.0f);
            }

            if (TbAbs(delta.GetY()) <= SPEED_Y_MIN) {
                velo.Add(0.0f, delta.GetY());
            } else {
                velo.Add(0.0f, TbSign(delta.GetY())*TbMax(SPEED_Y_MIN, TbAbs(delta.GetY())*approachRateY));
            }
            it->pos += velo;
            if (delta.Length() < FADE_OUT_DIST || !isOn )
            {
                it->alpha -= FADE_OUT_SPEED;
            }
        }
        else
        {
            it->alpha -= FADE_OUT_SPEED;
        }
        if (it->alpha <= 0.0f)
        {
            it = m_Particle.Erase(it);
        }
        else
        {
            ++it;
        }
    }

    if(isOn)
    {
        // 生成
        if (m_Particle.IsEmpty()) {
            createParticle();
        }
        else
        {
            // ランダム
            TbFloat32 rand = TbRandomFloat();
            if (CREATE_RATE < rand) {
                createParticle();
            }
        }
    }
}

/*! 
 * パーティクル生成
 * @author teco
 */
void SideActionPlayerMagnet::createParticle()
{
    static TB_NDEBUG_CONST TbRangeF32 RANGE_X(80.0f,110.0f);
    static TB_NDEBUG_CONST TbRangeF32 RANGE_Y(30.0f,40.0f);
    if (m_Particle.IsFull()) {
        return;
    }

    Particle particle;
    TbFloat32 ratio = TbRandomFloat();
    particle.pos.SetX(TbLerp(RANGE_X.begin, RANGE_X.end, ratio) );
    TbFloat32 yMax = TbLerp(RANGE_Y.begin, RANGE_Y.end, ratio);
    ratio = TbRandomFloat();
    particle.pos.SetY( TbLerp(0.0f, yMax, ratio) );
    ratio = TbRandomFloat();
    particle.pos.Multiply(m_Player->IsLookingRight() ? 1.0f : -1.0f, 
                          0.5f < ratio ? 1.0f : -1.0f);
    m_Particle.PushBack(particle);
}

/*! 
 * 入力ONかどうか
 * @author teco
 */
TbBool SideActionPlayerMagnet::isInputOn() const
{
    TbBool isOn = m_Player->IsOnInput(CrInput::BUTTON_SPECIAL);
    if (GetActionStage()->TestDebugFlag(0))
    {
        if (m_BitArray.Test(FLAG_ACTIVE)) {
            isOn = !m_Player->IsPressInput(CrInput::BUTTON_SPECIAL);
        } else {
            isOn = m_Player->IsPressInput(CrInput::BUTTON_SPECIAL);
        }
    }
    return isOn;
}

/*! 
 * お互い掴みあっている
 * @author teco
 */
TbBool SideActionPlayerMagnet::isCatchedRelative() const
{
    if (!m_Target) {
        return TB_FALSE;
    }
    const SideActionPlayer* player = GetTargetPlayer();
    if (!player) {
        return TB_FALSE;
    }
    const CrActionActor* equip = player->GetEquip(SideActionPlayer::EQUIO_PART_HAND);
    if (!equip) {
        return TB_FALSE;
    }
    const SideActionPlayerMagnet* magnet = TbDynamicCast<const SideActionPlayerMagnet*>(equip);
    if (!magnet) {
        return TB_FALSE;
    }
    if (m_Player != magnet->GetTarget()) {
        return TB_FALSE;
    }
    return TB_TRUE;
}
