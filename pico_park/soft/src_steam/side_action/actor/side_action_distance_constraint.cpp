/*!
 * 距離コンストレイント
 * @author teco
 */

#include "fwd.h"
#include "side_action_distance_constraint.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"

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

    CalcInfo info[CMN_PLAYER_MAX];
    TbUint32 playerCount = stage->GetPlayerCount();
    for( TbUint32 i = 0; i < playerCount; ++i ){
        info[i].player = stage->GetPlayer(i);
        if( !info[i].player->IsCleared() ){
            info[i].prevTension = calcPrevTension(i,stage);
            info[i].nextTension = calcNextTension(i,stage);
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
        // 3-1-2の時の引っかかり対策
        info[i].player->MoveContactActor( info[i].f , normal );
    }
}

/*!
 * 解決
 * @author teco
 */
void SideActionDistanceConstraint::calcF( CalcInfo* dst, CalcInfo* prev , CalcInfo* next )
{
    if( dst->player->IsCleared() ){
        return;
    }
    static const TbFloat32 RESULT_Y_MAX = SIDE_ACTION_GRAVITY_MAX+SIDE_ACTION_GRAVITY;
    TbVector2 result;
    if( prev ){
        result += calcF( dst->player , prev->player , dst->nextTension , prev->prevTension );
    }
    if( next ){
        result += calcF( dst->player , next->player , dst->prevTension , next->nextTension );
    }

    if( result.GetY() < -0.1f ){
        // 上に引っ張られた場合は空中移動不能にする
        if( !dst->player->GetCollision()->IsContacted(CMN_CONTACT_DOWN) ){
            dst->player->SetEnableAirWalk(TB_FALSE);
        }
    }

    if( result.GetY() < -TB_FLOAT32_EPSILON ){
        if( dst->player->GetCollision()->IsContacted(CMN_CONTACT_DOWN) ){
            if( 0 < result.GetY()+SIDE_ACTION_GRAVITY ){
                // 重力より弱い
                result.SetY(0.0f);
            }
        }
    }

    TbFloat32 signY = TbSign(result.GetY());
    result.SetY( signY*TbMin(TbAbs(result.GetY()),RESULT_Y_MAX) );
    dst->f = result;
}

/*!
 * 解決
 * @author teco
 */
TbVector2 SideActionDistanceConstraint::calcF( SideActionPlayer* src , SideActionPlayer* target , const TbVector2& srcTension , const TbVector2& targetTension )
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
        if( src->GetCollision()->IsContactedMap(CMN_CONTACT_RIGHT) ){
            result.Add(0.0f,signY*TbAbs(result.GetX())*FRICTION_SCALE);
            result.SetX(0.0f);
        }
    }else if( result.GetX() < -TB_FLOAT32_EPSILON ){
        if( src->GetCollision()->IsContactedMap(CMN_CONTACT_LEFT) ){
            result.Add(0.0f,signY*TbAbs(result.GetX())*FRICTION_SCALE);
            result.SetX(0.0f);
        }
    }
    if( TB_FLOAT32_EPSILON < result.GetY() ){
        if( src->GetCollision()->IsContactedMap(CMN_CONTACT_DOWN) ){
            result.Add(signX*TbAbs(result.GetY())*FRICTION_SCALE,0.0f);
            result.SetY(0.0f);
        }
    }else if( result.GetY() < -TB_FLOAT32_EPSILON ){
        if( src->GetCollision()->IsContactedMap(CMN_CONTACT_UP) ){
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
TbVector2 SideActionDistanceConstraint::calcPrevTension( TbSint32 playerIndex , SideActionStage* stage )
{
    TbVector2 result;
    SideActionPlayer* main = stage->GetPlayer(playerIndex);
    if( main && main->IsFloating() ){
        TbFloat32 contactCount = main->GetCollision()->GetContactCount(CMN_CONTACT_UP,TB_TRUE);
        result.Add( 0.0f , (contactCount+1.0f)*m_GraityTension );
    }
    for( TbSint32 i = playerIndex; 0 < i; --i ){
        SideActionPlayer* src = stage->GetPlayer(i);
        SideActionPlayer* pre = stage->GetPlayer(i-1);
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
TbVector2 SideActionDistanceConstraint::calcNextTension( TbSint32 playerIndex , SideActionStage* stage )
{
    TbVector2 result;
    SideActionPlayer* main = stage->GetPlayer(playerIndex);
    if( main && main->IsFloating() ){
        TbFloat32 contactCount = main->GetCollision()->GetContactCount(CMN_CONTACT_UP,TB_TRUE);
        result.Add( 0.0f , (contactCount+1.0f)*m_GraityTension );
    }
    TbSint32  playerCount = stage->GetPlayerCount();
    for( TbSint32 i = playerIndex; i < playerCount-2; ++i ){
        SideActionPlayer* src = stage->GetPlayer(i);
        SideActionPlayer* next = stage->GetPlayer(i+1);
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
    static const TbFloat32 OFFSET_Y = 10.0f;
    static TB_NDEBUG_CONST TbFloat32 OFFSET_X = 5.0f;
    static TbPointS32 buffer[1000];

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
