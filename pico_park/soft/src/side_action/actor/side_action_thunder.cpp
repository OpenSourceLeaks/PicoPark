/*!
* プレイヤー用レーザー
* @author teco
*/

#include "fwd.h"
#include "side_action_thunder.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player_guard.h"
#include <crayon/map/cr_action_map.h>
#include <crayon/collision/cr_action_collision_util.h>
#include "common/cmn_util.h"
#include <base/math/tb_random.h>
#include <sound/tb_sound_manager.h>
#include "root.h"

static const TbRectF32Pod SPRITE_RECT =
{
    -16 , -8 , 32 , 8
};

static const TbRectF32Pod SPRITE_UV_RECT =
{
    CMN_TEX_UV_RECT(192,432,16,4),
};

static const TbRectF32Pod SPRITE_UV_LASER_RECT[] =
{
    CMN_TEX_UV_RECT(160,400,32,32),
    CMN_TEX_UV_RECT(192,400,32,32)
};

const TbFloat32 SideActionThunder::LASER_LEN = 2400.0f;

/*!
* コンストラクタ
* @author teco
*/
SideActionThunder::SideActionThunder()
    : Super()
    , m_Dir(CMN_DIR_UP)
    , m_LaserLen(0.0f)
    , m_LaserLenOld(0.0f)
    , m_LaserLenMap(0.0f)
    , m_AnimTimer(0.0f)
    , m_UvIndex(0)
    , m_CollidedPlayerBit(0)
    , m_CollidedPlayerBitOld(0)
{
}

/*!
* デストラクタ
* @author teco
*/
SideActionThunder::~SideActionThunder()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionThunder::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_DIR ,
        PARAM_THROUGH ,
        PARAM_SHOOT_BLOCK ,
    };
    static const TbFloat32 LASER_WIDTH = 4.0f;
    m_Dir = static_cast<CmnDir>(userData.AsSint32(PARAM_DIR));
    if (PARAM_THROUGH < userData.GetCount() ) {
        m_BitArray.SetBit(FLAG_THROUGH, userData.AsSint32(PARAM_THROUGH));
    }
    if (PARAM_SHOOT_BLOCK < userData.GetCount()) {
        m_BitArray.SetBit(FLAG_SHOOT_BLOCK, userData.AsSint32(PARAM_SHOOT_BLOCK));
    }    
    TbSprite& sprite = GetMainSprite();
    if (!m_BitArray.Test(FLAG_THROUGH)) {
        sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
        sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
    }

    // 吸引パーティクル
    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this, &Self::onDrawLaser));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_BACK_3);


    TbRectF32 collisionRect;
    switch (m_Dir)
    {
    case CMN_DIR_UP:
        collisionRect = TbRectF32(-LASER_WIDTH/2.0f,-LASER_LEN, LASER_WIDTH, LASER_LEN);
        break;
    case CMN_DIR_DOWN:
        collisionRect = TbRectF32(-LASER_WIDTH / 2.0f, 0 , LASER_WIDTH, LASER_LEN);
        sprite.SetRotation(TbDegToAng32(180.0f));
        break;
    case CMN_DIR_RIGHT:
        collisionRect = TbRectF32(0.0f, -LASER_WIDTH / 2.0f, LASER_LEN, LASER_WIDTH);
        sprite.SetRotation(TbDegToAng32(90.0f));
        break;
    case CMN_DIR_LEFT:
        collisionRect = TbRectF32(-LASER_LEN, -LASER_WIDTH / 2.0f, LASER_LEN, LASER_WIDTH);
        sprite.SetRotation(TbDegToAng32(270.0f));
        break;
    }

    // コリジョン設定
    CrActionCollision* collision = CreateCollision(collisionRect, 0, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));

    collisionRect = TbRectF32(-16.0f, -16.0f,32.0f,32.0f);
    TbUint32 pushBackBit = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    if (!m_BitArray.Test(FLAG_THROUGH)) {
        pushBackBit |= CrActionCollision::PUSH_BACK_TARGET_MAP;
    }
    collision = CreateCollision(collisionRect, 
                                pushBackBit, 
                                TB_TRUE);
}

/*!
* ロジック更新
* @author teco
*/
void SideActionThunder::OnUpdateAction(TbFloat32 deltatime)
{
    if( !m_BitArray.Test(FLAG_SETUP) )
    {
        TbVector2 mov;
        switch (m_Dir)
        {
        case CMN_DIR_UP:
            mov.Set(0.0f,-LASER_LEN);
            break;
        case CMN_DIR_DOWN:
            mov.Set(0.0f, LASER_LEN);
            break;
        case CMN_DIR_RIGHT:
            mov.Set(LASER_LEN,0.0f);
            break;
        case CMN_DIR_LEFT:
            mov.Set(-LASER_LEN,0.0f);
            break;
        }
        CrActionCollision* collision = GetCollision(1);
        collision->SetInitPos(GetPosXY());
        TbVector2 correctMov;
        GetActionStage()->GetMap().Move(collision, mov,&correctMov);
        m_LaserLenMap = correctMov.Length() + collision->GetRect().width;
        m_LaserLen = m_LaserLenMap;
        m_LaserLenOld = m_LaserLen;
        if (!m_BitArray.Test(FLAG_SHOOT_BLOCK)) {
            collision->SetActive(TB_FALSE);
        }
        m_BitArray.SetBit(FLAG_SETUP);
    }
    m_AnimTimer += deltatime;
    if ( (6.0f/60.0f) < m_AnimTimer )
    {
        m_UvIndex = !m_UvIndex;
        m_AnimTimer = 0.0f;
    }
    m_CollidedPlayerBitOld = m_CollidedPlayerBit;
    m_CollidedPlayerBit = 0;
    m_LaserLenOld = m_LaserLen;
    m_LaserLen = m_LaserLenMap;
    m_Target.Clear();
}

/*!
 * ロジック後処理
 * @author teco
 */
void SideActionThunder::OnPostAction(TbFloat32 deltatime)
{
    TbRectF32 rect = getCollisionRect(m_LaserLen);
    for( auto it =m_Target.BeginConst(); it != m_Target.End(); ++it )
    {
        const CrActionCollision* collision = (*it)->GetCollision();
        if (TB_VERIFY(CrActionCollision::SHAPE_TYPE_RECT == collision->GetShapeType()))
        {
            if (CrActionCollisionUtil::IsCollidedRect(rect, collision->GetGlobalRect()))
            {
                // ダメージ判定
                TbUint32 bit = TB_BIT((*it)->GetPlayerIndex());
                m_CollidedPlayerBit |= bit;
                if (m_CollidedPlayerBitOld & bit)
                {
                    Root::GetInstance().Flash();
                    TbPlaySound("hit");
                    (*it)->NotifyEvent(SIDE_ACTION_EVENT_DEAD, 0);
                }
            }
        }

    }
}

/*!
* 衝突時コールバック
* @author teco
*/
void SideActionThunder::onCollided(CrActionCollision* collision)
{
//    SideActionPlayerGuard* guard = TbDynamicCast<SideActionPlayerGuard*>(collision->GetActor());
    SideActionPlayer* player = TbDynamicCast<SideActionPlayer*>(collision->GetActor());
    SideActionThunder* thunder = TbDynamicCast<SideActionThunder*>(collision->GetActor());
    if (thunder) {
        return;
    }
    if (!player)
    {
        if (m_BitArray.Test(FLAG_THROUGH)) {
            return;
        }
        TbRectF32 rect = getCollisionRect(m_LaserLen);
        TbRectF32 targetRect = collision->GetGlobalRect();
        if (CrActionCollisionUtil::IsCollidedRect(rect, targetRect))
        {
            switch (m_Dir)
            {
            case CMN_DIR_UP:
                m_LaserLen = TbAbs((targetRect.y+targetRect.height)-GetPos().GetY());
                break;
            case CMN_DIR_DOWN:
                m_LaserLen = TbAbs((targetRect.y) - GetPos().GetY());
                break;
            case CMN_DIR_RIGHT:
                m_LaserLen = TbAbs((targetRect.x) - GetPos().GetX());
                break;
            case CMN_DIR_LEFT:
                m_LaserLen = TbAbs((targetRect.x + targetRect.width) - GetPos().GetX());
                break;
            }
        }
    }
    else if(player)
    {
        m_Target.PushBack(player);
    }
}

/*!
* パーティクル描画
* @author teco
*/
void SideActionThunder::onDrawLaser(const TbMatrix& poseMatrix, TbSpriteMaterial* material)
{
    static TB_NDEBUG_CONST TbFloat32 OFFSET_Y = -5.0f;
    {
        TbMatrix mtx = TbMatrix::IDENTITY;
        TbVector2 pos = GetDrawPos();
        mtx.Translate(pos.GetX(), pos.GetY(), 0.0f);
        switch (m_Dir)
        {
        case CMN_DIR_DOWN:
            mtx.RotateZ(TbDegToAng32(180.0f));
            break;
        case CMN_DIR_RIGHT:
            mtx.RotateZ(TbDegToAng32(90.0f));
            break;
        case CMN_DIR_LEFT:
            mtx.RotateZ(TbDegToAng32(270.0f));
            break;
        }
        mtx = poseMatrix * mtx;
        TbUint32 count = (m_LaserLen / SPRITE_RECT.width);
        TbFloat32 scale = TbMod(m_LaserLen/SPRITE_RECT.width,1.0f);
        if (0.0f < scale) {
            ++count;
        }
        for (TbUint32 i = 0; i < count; ++i)
        {
            TbFloat32 height = SPRITE_RECT.width;
            TbRectF32 uvRect = SPRITE_UV_LASER_RECT[m_UvIndex];
            if (i == count - 1) {
                height *= scale;
                uvRect.height *= scale;
            }
            TbRectF32 rect( SPRITE_RECT.x, 
                            -(SPRITE_RECT.width*i+ height) + OFFSET_Y,
                            SPRITE_RECT.width , 
                            height + 1.0f);
            material->Draw(rect, uvRect, TbColor::White(), mtx);
        }
    }
}

/*!
 * コリジョン矩形取得
 * @author teco
 */
TbRectF32 SideActionThunder::getCollisionRect(TbFloat32 len) const
{
    TbRectF32 rect = GetCollision()->GetGlobalRect();
    switch (m_Dir)
    {
    case CMN_DIR_UP:
        rect.y += (LASER_LEN - len);
        rect.height = len;
        break;
    case CMN_DIR_DOWN:
        rect.height = len;
        break;
    case CMN_DIR_RIGHT:
        rect.width = len;
        break;
    case CMN_DIR_LEFT:
        rect.x += (LASER_LEN - len);
        rect.width = len;
        break;
    }
    return rect;
}
