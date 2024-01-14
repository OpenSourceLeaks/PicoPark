/*!
 * 距離コンストレイント
 * @author teco
 */

#include "fwd.h"
#include "side_action_distance_constraint.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "common/cmn_util.h"

#include <base/math/tb_geometry_util.h>

static TbFloat32 K = 0.2f; // ばね定数的な意味なもの

const TbFloat32 SideActionDistanceConstraint::GRAVITY_TENSION_DEFAULT = 0.5f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionDistanceConstraint::SideActionDistanceConstraint()
    : Super()
    , m_Distance(0.0f)
    , m_GraityTension(GRAVITY_TENSION_DEFAULT)
{
    // 文字
    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawConstraint));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);
    TbFillZero(m_EnableForceMove);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionDistanceConstraint::~SideActionDistanceConstraint()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionDistanceConstraint::OnUpdateAction( TbFloat32 deltatime )
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    if( !stage ){
        return;
    }
    TbUint32 playerCount = stage->GetPlayerCount();
    for (TbUint32 i = 0; i < playerCount; ++i) {
        if (stage->GetPlayer(i)->GetState() == SideActionPlayer::STATE_DEAD ||
            stage->GetPlayer(i)->GetState() == SideActionPlayer::STATE_NULL) 
        {
            // DEAD状態がいれば何もしない
            return;
        }
    }

    CalcInfo info[CMN_PLAYER_MAX];
    for( TbUint32 i = 0; i < playerCount; ++i ){
        info[i].player = stage->GetPlayer(i);
        if( !info[i].player->IsCleared() ){
            info[i].prevTension = calcPrevTension(i,stage,&info[i].prevFloatingCount);
            info[i].nextTension = calcNextTension(i,stage,&info[i].nextFloatingCount);
        }
    }

    for( TbUint32 i = 0; i < playerCount; ++i ){
        CalcInfo* prev = 0 < i ? &info[i-1] : nullptr;
        CalcInfo* next = i < playerCount-1 ? &info[i+1] : nullptr;
        if( !info[i].player->IsCleared() ){
            calcF(&info[i],prev,next);
        }
    }

    for( TbUint32 i = 0; i < playerCount; ++i ){
        if( info[i].player->IsCleared() ){
            continue;
        }
        info[i].player->AddPos(info[i].f); // そのまま位置補正に使ってみる
        info[i].player->AddVelo(info[i].f);
        TbVector2 normal = info[i].f;
        normal.Normalize();

        // 3-1-2の時の引っかかり対策(引っ張られる先に仲間を押す)
        info[i].player->MoveContactActor( info[i].f , normal );
    }

    TbFillZero(m_EnableForceMove);
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionDistanceConstraint::NotifyEvent(TbUint32 event, void* param)
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_ENABLE_MOVE:
        {
            TbUint32* playerIndex = reinterpret_cast<TbUint32*>(param);
            if (playerIndex) {
                m_EnableForceMove[*playerIndex] = TB_TRUE;
            }
        }
        break;
    }
    return 0;
}

/*!
 * 解決
 * @author teco
 */
void SideActionDistanceConstraint::calcF( CalcInfo* dst, CalcInfo* prev , CalcInfo* next )
{
    dst->player->ResetGravityDir();
    if( dst->player->IsCleared() ){
        return;
    }
    static const TbFloat32 RESULT_Y_MAX = SIDE_ACTION_GRAVITY_MAX+SIDE_ACTION_GRAVITY;
    TbVector2 result;
    TbBool wallUp = TB_FALSE;
    if( prev ){
        result += calcF( dst->player , 
                         prev->player , 
                         dst->nextTension , 
                         prev->prevTension ,
                         dst->prevFloatingCount ,
                         &wallUp );
    }
    if( next ){
        result += calcF( dst->player , 
                         next->player , 
                         dst->prevTension , 
                         next->nextTension ,
                         dst->nextFloatingCount ,
                         &wallUp );
    }
    if ( m_EnableForceMove[dst->player->GetPlayerIndex()] )
    {
        // 最終的に絶対移動はできるように
        TbFloat32 vx = dst->player->GetVelo().GetX();
        TbFloat32 MIN_X = 0.1f;
        if( vx * result.GetX() < 0.0f && TbAbs(vx) < (TbAbs(result.GetX())+MIN_X) ) {
            result.SetX(-TbSign(vx)*(TbAbs(vx)-0.1f));
        }
    }
    if( result.GetY() < -0.1f ){
        // 上に引っ張られた場合は空中移動不能にする
        if( !dst->player->GetCollision()->IsContacted(CR_CONTACT_DOWN) ){
            dst->player->SetEnableWalk(TB_FALSE);
        }
        if (wallUp && dst->player->GetVelo().GetY() < result.GetY() ) {
            // 壁滑りで上に上る時に、その力以上に既に速度がのってる時は何もしない
            result.SetY(0.0f);
        }
    }
    if( result.GetY() < -TB_FLOAT32_EPSILON ){
        if( dst->player->GetCollision()->IsContacted(CR_CONTACT_DOWN) ){
            if( 0 < result.GetY()+SIDE_ACTION_GRAVITY ){
                // 重力より弱い
                result.SetY(0.0f);
            } 
        }
    }

    // 制限
    TbFloat32 signY = TbSign(result.GetY());
    result.SetY( signY*TbMin(TbAbs(result.GetY()),RESULT_Y_MAX) );
    dst->f = result;
}

/*!
 * 解決
 * @author teco
 */
TbVector2 SideActionDistanceConstraint::calcF( SideActionPlayer* src , 
                                               SideActionPlayer* target , 
                                               const TbVector2& srcTension , 
                                               const TbVector2& targetTension ,
                                               TbUint32 floatingCount ,
                                               TbBool* wallUp )
{
    TbVector2 result;
    if( !src || !target ){
        return result;
    }    
    if( src->IsCleared() ){
        return result;
    }

    TbVector4 deltaPos = target->GetPos() - src->GetPos();
    
    TbFloat32 length = deltaPos.Length();
    if( TB_FLOAT32_EQUAL_ZERO(length) ) {
        return result;
    }

    if( length <= m_Distance ){
        return result;
    }

    // 重力特殊対応
//    if( src-> )
    TbVector2 dir(deltaPos.GetX(),deltaPos.GetY());
    dir.Normalize();
    TbFloat32 div = 1.0f;
    if( !target->IsCleared() ) {
        TbFloat32 tension1 = 1.0f + TbMax(0.0f,-dir.Dot(srcTension));
        TbFloat32 tension2 = 1.0f + TbMax(0.0f,dir.Dot(targetTension));
        div = 1.0f-(tension1/(tension1+tension2));
    }
    TbFloat32 lengthF = length - m_Distance;
    TbFloat32 f = (lengthF*div)*K;
    result = dir * f;

    // 壁滑り
    static TbFloat32 FRICTION_SCALE = 1.0f;
    TbFloat32 signX = TbSign(result.GetX());
    TbFloat32 signY = TbSign(result.GetY());
    if( TB_FLOAT32_EPSILON < result.GetX() ){
        if( src->GetCollision()->IsContactedMap(CR_CONTACT_RIGHT) ){
            result.Add(0.0f,signY*TbAbs(result.GetX())*FRICTION_SCALE);
            result.SetX(0.0f);
            *wallUp = (signY < 0.0f);
        }
    }else if( result.GetX() < -TB_FLOAT32_EPSILON ){
        if( src->GetCollision()->IsContactedMap(CR_CONTACT_LEFT) ){
            result.Add(0.0f,signY*TbAbs(result.GetX())*FRICTION_SCALE);
            result.SetX(0.0f);
            *wallUp = (signY < 0.0f);
        }
    }
    if( TB_FLOAT32_EPSILON < result.GetY() ){
        if (src->GetCollision()->IsContactedMap(CR_CONTACT_DOWN)) {
            static TbFloat32 FRICTION_MOVE_SCALE_BEGIN = 0.0f;
            static TbFloat32 FRICTION_MOVE_SCALE = 1.2f;
            TbFloat32 frictionScale = FRICTION_SCALE;
#if 1
            if (0 <= floatingCount && m_EnableForceMove[src->GetPlayerIndex()])
            {
                TbFloat32 correctScale = (1.0f / (floatingCount + 1.0f));
                correctScale *= FRICTION_MOVE_SCALE;
                correctScale += FRICTION_MOVE_SCALE_BEGIN;
                correctScale = TbMin(correctScale, 1.0f);
                // 宙に全員ぶら下がって歩けるときは滑りにくい
                frictionScale *= correctScale;
            }
#endif
            result.Add(signX*TbAbs(result.GetY())*frictionScale, 0.0f);
            result.SetY(0.0f);
        }
    }else if( result.GetY() < -TB_FLOAT32_EPSILON ){
        if( src->GetCollision()->IsContactedMap(CR_CONTACT_UP) ){
            result.Add(signX*TbAbs(result.GetY())*FRICTION_SCALE,0.0f);
            result.SetY(0.0f);
        }
    }

    return result;
}

/*!
 * 引っ張り計算
 * @author teco
 */
TbVector2 SideActionDistanceConstraint::calcPrevTension( TbSint32 playerIndex , SideActionStage* stage , TbUint32* floatingCount)
{
    TbVector2 result;
    SideActionPlayer* main = stage->GetPlayer(playerIndex);
    if( main && main->IsFloating() ){
        // 中に浮いている時は引っ張る力にはじめから重力を乗せる
        TbFloat32 contactCount = main->GetCollision()->GetContactCount(CR_CONTACT_UP,TB_TRUE);
        result.Add( 0.0f , (contactCount+1.0f)*m_GraityTension);
    }

    *floatingCount = 0;
    TbBool isFloating = TB_TRUE;
    for( TbSint32 i = playerIndex; 0 < i; --i ){
        SideActionPlayer* src = stage->GetPlayer(i);
        SideActionPlayer* pre = stage->GetPlayer(i-1);
        if (!pre->IsFloating()) {
            *floatingCount = 0;
            isFloating = TB_FALSE;
        }
        if(isFloating){
            ++(*floatingCount);
        }
        TbVector2 tension = calcTension(src,pre);
        if( TB_FLOAT32_EQUAL_ZERO(tension.LengthSq()) ){
            continue;
        }
        result += tension;
    }

    return result;
}

/*!
 * 引っ張り計算
 * @author teco
 */
TbVector2 SideActionDistanceConstraint::calcNextTension( TbSint32 playerIndex , SideActionStage* stage, TbUint32* floatingCount)
{
    TbVector2 result;
    SideActionPlayer* main = stage->GetPlayer(playerIndex);
    if (main->IsFloating()) {
        TbFloat32 contactCount = main->GetCollision()->GetContactCount(CR_CONTACT_UP, TB_TRUE);
        result.Add(0.0f, (contactCount + 1.0f)*m_GraityTension);
    }
    *floatingCount = 0;
    TbBool isFloating = TB_TRUE;
    TbSint32  playerCount = stage->GetPlayerCount();
    for( TbSint32 i = playerIndex; i < playerCount-2; ++i ){
        SideActionPlayer* src = stage->GetPlayer(i);
        SideActionPlayer* next = stage->GetPlayer(i+1);
        if (!next->IsFloating()) {
            *floatingCount = 0;
            isFloating = TB_FALSE;
        }
        if (isFloating) {
            ++(*floatingCount);
        }
        TbVector2 tension = calcTension(src,next);
        if( TB_FLOAT32_EQUAL_ZERO(tension.LengthSq()) ){
            continue;
        }
        result += tension;
    }
    return result;
}

/*!
 * 引っ張り計算
 * @author teco
 */
TbVector2 SideActionDistanceConstraint::calcTension( SideActionPlayer* src , SideActionPlayer* target )
{
    if( !src || !target ){
        return TbVector2::ZERO;
    }    

    TbVector4 deltaPos = target->GetPos() - src->GetPos();
    
    TbFloat32 length = deltaPos.Length();
    if( TB_FLOAT32_EQUAL_ZERO(length) ) {
        return TbVector2::ZERO;
    }

    if( length <= m_Distance ){
        return TbVector2::ZERO;
    }

    // 一定距離を超えた
    TbVector2 dir(deltaPos.GetX(),deltaPos.GetY());
    dir.Normalize();

    // 対象の上にのってるやつがいたらそれだけ倍増
    TbFloat32 contactCount = target->GetCollision()->GetContactCount(CR_CONTACT_UP, TB_TRUE);
    dir *= (contactCount+1.0f);

    return dir;
}

#include "root.h"

/*!q
 * 前景描画
 * @author teco
 */
void SideActionDistanceConstraint::onDrawConstraint( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    // 各プレイヤーの両耳をつなぐ
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    if( !stage ){
        return;
    }
    TbUint32 playerCount = stage->GetPlayerCount();
    for( TbUint32 i = 0; i < playerCount-1; ++i ){
        SideActionPlayer* player = stage->GetPlayer(i);
        SideActionPlayer* next = stage->GetPlayer(i+1);
        onDrawConstraintPlayer( player , next , poseMatrix ,  material );
    }

#if 0
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    for( TbUint32 i = 0; i < playerCount; ++i ){
        SideActionPlayer* player = stage->GetPlayer(i);
        fontBuffer->Printf(100,
                           100+i*30,
                           "%dPY=%0.4f",i+1,player->GetPos().GetY());
    }
    fontBuffer->PopSetting();
    fontBuffer->Flush();
#endif
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionDistanceConstraint::onDrawConstraintPlayer( const SideActionPlayer* player, const SideActionPlayer* next , const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    static const TbFloat32 PIXEL_SIZE = 2.0f;
    static const TbFloat32 OFFSET_Y = -14.0f;
    static TB_NDEBUG_CONST TbFloat32 OFFSET_X = 5.0f;
    static TbPointS32 buffer[1000];

    if (player->IsCleared() && next->IsCleared()) {
        return;
    }

    // 線の整数座標配列サイズを取得
    // 線を整数座標配列で取得
    TbPointS32 startPoint( OFFSET_X , 0.0f );
    TbPointS32 endPoint( next->GetDrawPos().GetX() - player->GetDrawPos().GetX() - OFFSET_X , next->GetDrawPos().GetY() - player->GetDrawPos().GetY() );
    TbUint32 count  = TbGetLinePointArray( buffer, 
                                      TB_ARRAY_COUNT_OF(buffer),
                                      startPoint , 
                                      endPoint );
    for( TbUint32 i = 0; i < count; i += PIXEL_SIZE ){
        TbVector2 pos = player->GetDrawPos();
        pos.Add( static_cast<TbFloat32>(buffer[i].x) ,
                 static_cast<TbFloat32>(buffer[i].y) + OFFSET_Y );

        TbRectF32 rect = TbRectF32( pos.GetX() , 
                                    pos.GetY() , 
                                    PIXEL_SIZE , 
                                    PIXEL_SIZE);
        TbRectF32 uvRect( CMN_TEX_UV_RECT(512-8,0,8,8) );
        TbMatrix mtx = TbMatrix::IDENTITY;
        material->Draw( rect , uvRect , CmnUtil::GetMainColor() , poseMatrix);
    }

}
