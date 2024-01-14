/*!
 * アクション用コリジョン
 * @author teco
 * @since 2012.05.25
 */

#include <tb_fwd.h>
#include <crayon/collision/cr_action_collision_scene.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>

#include <graphics/render/tb_draw_2d.h>

using namespace toybox;

namespace
{

static const TbFloat32 MOV_ERROR = 0.01f;

// 補正
TbVector2 reviseMov( const TbVector2& mov )
{
    TbFloat32 x = 0.0f;
    TbFloat32 y = 0.0f;
    if( !TB_FLOAT32_EQUAL_ZERO(mov.GetX()) ){
        x = MOV_ERROR * TbSign(mov.GetX());
    }
    if( !TB_FLOAT32_EQUAL_ZERO(mov.GetY()) ){
        y = MOV_ERROR * TbSign(mov.GetY());
    }
    TbVector2 result = mov;
    result.Add(x,y);
    return result;
}

}

namespace crayon
{

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.06
 */
CrActionCollisionScene::CrActionCollisionScene()
    : m_Top(NULL)
    , m_Map(NULL)
{
    TbMemClear(m_LayerTable,sizeof(m_LayerTable));
    m_LayerTable[0][0] = TB_TRUE;
    m_TempPosInfo.SetCapacity(100);
    m_TempPosInfo.SetEnableSetCapacityAuto(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.10.06
 */
CrActionCollisionScene::~CrActionCollisionScene()
{
}

/*!
 * 衝突更新
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollisionScene::Update()
{
    Result res = RESULT_OK;
    TbStaticArray<CrActionCollision*,32> scaleObject;

    // 現在の座標を保存しておく
    if( m_Top ) {
        CrActionCollision* co = m_Top;
        do {
            if (co->updatePre()) {
                // 1フレームでスケールを変更できるオブジェクトを1つだけとする
                TB_ASSERT(!scaleObject.IsFull());
                scaleObject.PushBack(co);
            }
            co = co->m_Next;
        } while( co != m_Top );
    }

    CheckCollided(TB_TRUE);

    // まずマップだけとの押し戻し判定
    updateMap();

    // 押し戻す必要があるものの押し戻し処理
    do {
        do {
            res = updatePushBack(false);
        } while( res == RESULT_AGAIN ); // やり直し結果が出たら繰り返す
        TB_ASSERT(res!=RESULT_FIRST);
        do {
            res = updatePushBack(true);
        } while( res == RESULT_AGAIN ); // やり直し結果が出たら繰り返す
    } while( res == RESULT_FIRST );

//    CheckCollided(TB_FALSE);
#if 1
    if (m_Top) {
        // パス1
        CrActionCollision* co = m_Top;
        do {
            co->m_DebugPos = co->GetPos();
            co = co->m_Next;
        } while (co != m_Top);
    }
#endif

    CheckCollided(TB_FALSE);

    // 押し戻し後処理
    OnPostPushBack();

    // スケール値変更処理
    TbForIterator( it , scaleObject) {
        updateScale(*it);
    }

    CheckCollided(TB_FALSE);

    // 全ての位置が終了したので、最終的な状態更新
    if( m_Top ) {
        // パス1
        CrActionCollision* co = m_Top;
        do {
            co->updateAfter();
            co = co->m_Next;
        } while( co != m_Top );
    }
    CheckCollided(TB_FALSE);
    CheckCollided(TB_TRUE);

    // 押し戻しは必要ないものとの衝突判定
    {
        updateCollide();
    }

    CheckCollided(TB_TRUE);
}

/*!
 * 衝突しているか(外部から強制的に判定
 * @author teco
 * @since 2014.06.25
 */
TbBool CrActionCollisionScene::IsCollidedByPushBackTarget( const CrActionCollision& src , TbUint32 targetFilter ) const
{
    if ( (targetFilter&CrActionCollision::PUSH_BACK_TARGET_MAP) && src.IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_MAP) ) {
        if (m_Map->IsContacted(&src)) {
            return TB_TRUE;
        }
    }
    if((targetFilter&CrActionCollision::PUSH_BACK_TARGET_COLLISION) && src.IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) ){
        if( m_Top ) {
            CrActionCollision* co = m_Top;
            do {
                if( co != &src && co->IsActive() && co->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) && 
                    IsEnableCheck(co->GetLayer(),src.GetLayer()) && 
                    ( CrActionCollision::IsCollidedPre(*co, src, NULL, NULL, TB_TRUE) ||
                      CrActionCollision::IsCollided(*co,src,NULL,NULL,TB_TRUE)) ) 
                {
                    return TB_TRUE;
                }
                co = co->m_Next;
            } while( co != m_Top );
        }
    }
    return TB_FALSE;
}

/*!
 * 衝突オブジェクト検索(外部から強制的に判定)
 * @author teco
 */
TbUint32 CrActionCollisionScene::SearchCollidedObjectByPushBackTarget(SearchResult* result, 
    const CrActionCollision& src, 
    CrContactFace type ) const
{
    static const TbFloat32 OFS = 0.5f;
    const TbVector2& ofsVec = CrUtil::GetContactFaceNormal(type)*OFS;

    result->Clear();
    if (src.IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION)) {
        if (m_Top) {
            CrActionCollision* co = m_Top;
            do {
                if (co != &src && co->IsActive() && co->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) &&
                    IsEnableCheck(co->GetLayer(), src.GetLayer()) &&
                    CrActionCollision::IsCollided(*co, src, nullptr , &ofsVec, TB_TRUE))
                {
                    result->PushBack(co);
                }
                co = co->m_Next;
            } while (co != m_Top);
        }
    }
    return result->GetCount();
}

/*!
 * 次のコリジョン取得
 * @author teco
 * @since 2014.07.22
 */
CrActionCollision* CrActionCollisionScene::GetNextCollision( CrActionCollision* col ) 
{
    CrActionCollision* result = NULL;
    if( col && m_Top != col->m_Next ){
        result = col->m_Next;
    }
    return result;
}


/*!
 * マップとのコリジョンチェック
 * @author teco
 * @since 2013.10.13
 */
void CrActionCollisionScene::updateMap()
{
    if( !m_Map ) {
        return;
    }
    if( m_Top ) {
        CrActionCollision* co = m_Top;
        do {
            if( co->IsActive() && co->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_MAP) ) {
                // マップ取得
                CrActionMap* map = GetMap();
                TbVector2 mov = co->GetMov();
                TB_ASSERT(!mov.HasNan());
                co->RevertPos();
                co->MoveMap( map , mov , NULL );
            }
            co = co->m_Next;
        } while( co != m_Top );
    }
}

/*! 
 * 全コリジョンの衝突判定
 * @author teco
 * @since 2013.10.13
 */
void CrActionCollisionScene::updateCollide()
{
    if( !m_Top ){
        return;
    }
    CrActionCollision* base = m_Top;
    CrActionCollision* next = base->m_Next;
    if( next == m_Top ) {
        return;
    }
    do {
        if( base->IsActive() ){
            do {
                if( base != next &&
                    next->IsActive() &&
                    IsEnableCheck(base->GetLayer(),next->GetLayer()) && 
                    CrActionCollision::IsCollided( *base , *next ) )
                {
                    base->onCollided(next);
                    next->onCollided(base);
                }
                next = next->m_Next;
            } while( next != m_Top );
        }
        // 基準を変更
        base = base->m_Next;
        next = base->m_Next;
    } while( next != m_Top );
}

/*!
 * コリジョン同士による押し戻し更新
 * @author teco
 * @param isEnableCheckFixedPos 
 * @since 2014.05.03
 */
CrActionCollisionScene::Result CrActionCollisionScene::updatePushBack( TbBool isEnableCheckFixedPos )
{
    if( !m_Top ){
        return RESULT_OK;
    }
    CrActionCollision* base = m_Top;
    CrActionCollision* next = base->m_Next;
    if( next == m_Top ) {
        return RESULT_OK;
    }

    Result res = RESULT_OK;

    m_TempPosInfo.Clear();
    if( isEnableCheckFixedPos ) {
        // 保存しておく
        do {
            PosInfo info = { base , base->GetPos() };
            m_TempPosInfo.PushBack(info);
            base = base->m_Next;
            next = base->m_Next;
        } while( next != m_Top );
        base = m_Top;
        next = base->m_Next;
    }

    do {
        if( (isEnableCheckFixedPos || !base->IsFixedPos()) && 
            base->IsActive() && 
            base->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) )
        {
            do {
                // 衝突処理
                if( (isEnableCheckFixedPos ? (base->IsFixedPos()||next->IsFixedPos()) :
                                             !next->IsFixedPos()) &&
                    next->IsActive() &&
                    next->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) )
                {
                    Result pushBackResult = UpdatePushBack(base,next);
                    if( RESULT_OK != pushBackResult ){
                        res = pushBackResult;
                        if( RESULT_FIRST == res ) {
                            // 位置固定がどうしても固定できない場合
                            TbForIterator( it , m_TempPosInfo ){
                                it->co->SetPos( it->pos );
                            }
                            // はじめからやり直しは一旦抜ける
                            return res;
                        }
                    }
                }
                next = next->m_Next;
            } while( next != m_Top );
        }

        // 基準を変更
        base = base->m_Next;
        next = base->m_Next;
    } while( next != m_Top );

    return res;
}

/*!
 * コリジョン同士の衝突判定更新
 * @param isFixedTarget2 target2の位置を固定するか
 * @author teco
 * @since 2013.10.06
 */
CrActionCollisionScene::Result CrActionCollisionScene::UpdatePushBack( CrActionCollision* target1 , CrActionCollision* target2 , const TbVector2* extPre1 , const TbVector2* extPre2 )
{
    TB_RESQUE_RET(target1 && target1->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) && 
                  target2 && target2->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION),
                  CrActionCollisionScene::RESULT_OK);
    
    if( target1 == target2 ) {
        return CrActionCollisionScene::RESULT_OK;
    }

    if( !IsEnableCheck(target1->GetLayer(),target2->GetLayer()) ){
        // 衝突しないレイヤー同士
        return CrActionCollisionScene::RESULT_OK;
    }

    if( target1->IsFixedPos() && 
        target2->IsFixedPos() )
    {
        // 両方強制移動なら無視
        return CrActionCollisionScene::RESULT_OK;
    }

    if( !CrActionCollision::IsCollided( *target1 , *target2 ) ){
        // 衝突していない
        return CrActionCollisionScene::RESULT_OK;
    }

    if( CrActionCollision::IsCollidedPre( *target1 , *target2 ) ){
        // もし衝突していたら無視
        return CrActionCollisionScene::RESULT_OK;
    }

    // 円形同士のみ専用処理
    if( CrActionCollision::SHAPE_TYPE_CIRCLE == target1->GetShapeType() && 
        CrActionCollision::SHAPE_TYPE_CIRCLE == target2->GetShapeType() )
    {
        TB_ASSERT_MSG(!target1->IsFixedPos(),"sorry unsupported");
        TB_ASSERT_MSG(!target2->IsFixedPos(),"sorry unsupported");
        return updatePushBackCircle( target1 , target2 , extPre1 , extPre2 );
    }
    return updatePushBackRect( target1 , target2 , extPre1 , extPre2 );
}
 /* コリジョン同士の押し戻し更新
 * @author teco
 * @since 2014.07.27
 */
CrActionCollisionScene::Result CrActionCollisionScene::updatePushBackRect( CrActionCollision* target1 , CrActionCollision* target2 , const TbVector2* extPre1 , const TbVector2* extPre2 )
{
    CrContactFace contactTypeX = CR_CONTACT_INVALID; // X軸接面フラグ
    TbFloat32 contactTimeX1 = 0.0f;
    TbFloat32 contactTimeX2 = 0.0f;
    CrContactFace contactTypeY = CR_CONTACT_INVALID; // Y軸接面フラグ
    TbFloat32 contactTimeY1 = 0.0f;
    TbFloat32 contactTimeY2 = 0.0f;

    TbRectF32 rect1 = target1->GetGlobalRect();
    TbRectF32 rect2 = target2->GetGlobalRect();
    TbVector2 pre1 = extPre1 ? *extPre1 : target1->GetPrePos();
    TbVector2 pre2 = extPre2 ? *extPre2 : target2->GetPrePos();
    TbVector2 mov1 = target1->GetPos() - pre1; // 対象の移動量
    TbVector2 mov2 = target2->GetPos() - pre2; // 自身の移動量

    // X軸の接触チェック
    if( mov1.GetX() != 0.0f || mov2.GetX() != 0.0f )
    {
        TbFloat32 x1l = (rect1.x-mov1.GetX());
        TbFloat32 x1r = rect1.x+rect1.width-mov1.GetX();
        TbFloat32 x2l = (rect2.x-mov2.GetX());
        TbFloat32 x2r = (rect2.x+rect2.width-mov2.GetX());
        // X軸方向に接面があるか
        if( x1r <= x2l ) {
            // target1の右側で接触
            if( mov1.GetX() * mov2.GetX() > TB_FLOAT32_EPSILON ){
                // 同じ方向の場合は先頭の位置は動かさない
                if( TbAbs(mov1.GetX()) > TbAbs(mov2.GetX()) ){
                    contactTimeX1 = (rect1.x+rect1.width-rect2.x)/(-mov1.GetX());
                }else{
                    contactTimeX2 = (rect1.x+rect1.width-rect2.x)/(mov2.GetX());
                }
            }else{
                contactTimeX1 = (rect1.x+rect1.width-rect2.x)/(mov2.GetX()-mov1.GetX());
                contactTimeX2 = contactTimeX1;
            }
            contactTypeX = CR_CONTACT_RIGHT;
        }else if( x2r <= x1l ){
            // target2の右側で接触
            if( mov1.GetX() * mov2.GetX() > TB_FLOAT32_EPSILON ){
                // 同じ方向の場合は先頭の位置は動かさない
                if( TbAbs(mov1.GetX()) > TbAbs(mov2.GetX()) ){
                    contactTimeX1 = (rect2.x+rect2.width-rect1.x)/(mov1.GetX());
                }else{
                    contactTimeX2 = (rect2.x+rect2.width-rect1.x)/(-mov2.GetX());
                }
            }else{
                contactTimeX1 = (rect2.x+rect2.width-rect1.x)/(mov1.GetX()-mov2.GetX());
                contactTimeX2 = contactTimeX1;
            }
            contactTypeX = CR_CONTACT_LEFT;
        }
        contactTimeX1 = TbClamp<TbFloat32>(contactTimeX1,-1.0f,0.0f);
        contactTimeX2 = TbClamp<TbFloat32>(contactTimeX2,-1.0f,0.0f);
        if( !TB_FLOAT32_EQUAL_ZERO(contactTimeX1) && !TB_FLOAT32_EQUAL_ZERO(contactTimeX2) )
        {
            TbFloat32 y1 = rect1.y + (contactTimeX1)*mov1.GetY(); // 自分
            TbFloat32 y2 = rect2.y + (contactTimeX2)*mov2.GetY(); // 相手
            if( (y2+rect2.height) < y1 || (y1+rect1.height) < y2 ) {
                // Y軸で接してない場合はリセット
                contactTypeX = CR_CONTACT_INVALID;
                contactTimeX1 = 0.0f;
                contactTimeX2 = 0.0f;
            }
        }
    }

    // Y軸の接触チェック
    if( mov1.GetY() != 0.0f || mov2.GetY() != 0.0f )
    {
        TbFloat32 y1u = (rect1.y-mov1.GetY());
        TbFloat32 y1d = (rect1.y+rect1.height-mov1.GetY());
        TbFloat32 y2u = (rect2.y-mov2.GetY());
        TbFloat32 y2d = (rect2.y+rect2.height-mov2.GetY());
        // Y軸方向に接面があるか
        if( y1d <= y2u ) {
            if( mov1.GetY() * mov2.GetY() > TB_FLOAT32_EPSILON ){
                // 同じ方向の場合は先頭の位置は動かさない
                if( TbAbs(mov1.GetY()) > TbAbs(mov2.GetY()) ){
                    contactTimeY1 = (rect1.y+rect1.height-rect2.y)/(-mov1.GetY());
                }else{
                    contactTimeY2 = (rect1.y+rect1.height-rect2.y)/(mov2.GetY());
                }
            }else{
                contactTimeY1 = (rect1.y+rect1.height-rect2.y)/(mov2.GetY()-mov1.GetY());
                contactTimeY2 = contactTimeY1;
            }
            contactTypeY = CR_CONTACT_DOWN;
        }else if( y2d <= y1u ){
            if( mov1.GetY() * mov2.GetY() > TB_FLOAT32_EPSILON ){
                // 同じ方向の場合は先頭の位置は動かさない
                if( TbAbs(mov1.GetY()) > TbAbs(mov2.GetY()) ){
                    contactTimeY1 = (rect2.y+rect2.height-rect1.y)/(mov1.GetY());
                }else{
                    contactTimeY2 = (rect2.y+rect2.height-rect1.y)/(-mov2.GetY());
                }
            }else{
                contactTimeY1 = (rect2.y+rect2.height-rect1.y)/(mov1.GetY()-mov2.GetY());
                contactTimeY2 = contactTimeY1;
            }
            contactTypeY = CR_CONTACT_UP;
        }
        contactTimeY1 = TbClamp<TbFloat32>(contactTimeY1,-1.0f,0.0f);
        contactTimeY2 = TbClamp<TbFloat32>(contactTimeY2,-1.0f,0.0f);
        if( !TB_FLOAT32_EQUAL_ZERO(contactTimeY1) && !TB_FLOAT32_EQUAL_ZERO(contactTimeY2) )
        {
            // X軸で接しているはず
            TbFloat32 x1 = rect1.x + (contactTimeY1)*mov1.GetX();
            TbFloat32 x2 = rect2.x + (contactTimeY2)*mov2.GetX();
            if( (x2+rect2.width) < x1 || (x1+rect1.width) < x2 ) {
                // X軸で接してない場合はリセット
                contactTypeY = CR_CONTACT_INVALID;
                contactTimeY1 = 0.0f;
                contactTimeY2 = 0.0f;
            }        
        }
    }

    if( contactTypeX != CR_CONTACT_INVALID || contactTypeY != CR_CONTACT_INVALID )
    {
        TbVector2 newPos1( target1->GetPos().GetX() + mov1.GetX()*(contactTimeX1) , 
                           target1->GetPos().GetY() + mov1.GetY()*(contactTimeY1) );
        {
            TbVector2 delta = (newPos1 - target1->GetPos());
            if( !TB_FLOAT32_EQUAL(contactTimeX1,0.0f) && 
                !TB_FLOAT32_EQUAL(mov1.GetX(),0.0f) && 
                TB_FLOAT32_EQUAL(delta.GetX(),0.0f) )
            {
                newPos1.SetX(pre1.GetX());
            }
            if( !TB_FLOAT32_EQUAL(contactTimeY1,0.0f) && 
                !TB_FLOAT32_EQUAL(mov1.GetY(),0.0f) && 
                TB_FLOAT32_EQUAL(delta.GetY(),0.0f) )
            {
                newPos1.SetY(pre1.GetY());
            }
        }
        TbVector2 newPos2( target2->GetPos().GetX() + mov2.GetX()*(contactTimeX2) , 
                           target2->GetPos().GetY() + mov2.GetY()*(contactTimeY2) );
        {
            TbVector2 delta = (newPos2 - target2->GetPos());
            if( !TB_FLOAT32_EQUAL(contactTimeX2,0.0f) && 
                !TB_FLOAT32_EQUAL(mov2.GetX(),0.0f) && 
                TB_FLOAT32_EQUAL(delta.GetX(),0.0f) )
            {
                newPos2.SetX(pre2.GetX());
            }
            if( !TB_FLOAT32_EQUAL(contactTimeY2,0.0f) && 
                !TB_FLOAT32_EQUAL(mov2.GetY(),0.0f) && 
                TB_FLOAT32_EQUAL(delta.GetY(),0.0f) )
            {
                newPos2.SetY(pre2.GetY());
            }
        }
        CrContactFace contactTypeX2 = CR_CONTACT_INVALID;
        CrContactFace contactTypeY2 = CR_CONTACT_INVALID;
        switch( contactTypeX )
        {
        case CR_CONTACT_LEFT:
            {
                if( contactTimeX1 <= contactTimeX2 && !TB_FLOAT32_EQUAL(mov1.GetX(),0.0f) ){
                    newPos1.Add(MOV_ERROR,0.0f);
                    newPos1.SetX( TbMin(newPos1.GetX(),pre1.GetX()) );
                }else{
                    newPos2.Add(-MOV_ERROR,0.0f);
                    newPos2.SetX( TbMax(newPos2.GetX(),pre2.GetX()) );
                }
                contactTypeX2 = CR_CONTACT_RIGHT;
            }
            break;
        case CR_CONTACT_RIGHT:
            {
                if( contactTimeX1 <= contactTimeX2 && !TB_FLOAT32_EQUAL(mov1.GetX(),0.0f) ){
                    newPos1.Add(-MOV_ERROR,0.0f);
                    newPos1.SetX( TbMax(newPos1.GetX(),pre1.GetX()) );
                }else{
                    newPos2.Add(MOV_ERROR,0.0f);
                    newPos2.SetX( TbMin(newPos2.GetX(),pre2.GetX()) );
                }
                contactTypeX2 = CR_CONTACT_LEFT;
            }
            break;
        case CR_CONTACT_INVALID:
            break;
        default:
            TB_RESQUE_RET(0,RESULT_OK);
            break;
        }
        switch( contactTypeY )
        {
        case CR_CONTACT_UP:
            {
                if( contactTimeY1 <= contactTimeY2 && !TB_FLOAT32_EQUAL(mov1.GetY(),0.0f) ){
                    newPos1.Add(0.0f,MOV_ERROR);
                    newPos1.SetY( TbMin(newPos1.GetY(),pre1.GetY()) );
                }else{
                    newPos2.Add(0.0f,-MOV_ERROR);
                    newPos2.SetY( TbMax(newPos2.GetY(),pre2.GetY()) );
                }
                contactTypeY2 = CR_CONTACT_DOWN;
            }
            break;
        case CR_CONTACT_DOWN:
            {
                if( contactTimeY1 <= contactTimeY2 && !TB_FLOAT32_EQUAL(mov1.GetY(),0.0f) ){
                    newPos1.Add(0.0f,-MOV_ERROR);
                    newPos1.SetY( TbMax(newPos1.GetY(),pre1.GetY()) );
                }else{
                    newPos2.Add(0.0f,MOV_ERROR);
                    newPos2.SetY( TbMin(newPos2.GetY(),pre2.GetY()) );
                }
                contactTypeY2 = CR_CONTACT_UP;
            }
            break;
        case CR_CONTACT_INVALID:
            break;
        default:
            TB_RESQUE_RET(0,RESULT_OK);
            break;
        }

        // 強制移動量を考慮
        CrActionMap* map = GetMap();
        if( target1->IsFixedPos() ) {
            // 強制移動
            newPos2 -= reviseMov(newPos1 - target1->GetPos());
            if( !moveCollisionForce( target2 , newPos2 ) ) {
                target1->pauseFixedPos();
                return RESULT_FIRST;
            }
        }else if( target2->IsFixedPos() ){
            // 強制移動
            newPos1 -= reviseMov(newPos2 - target2->GetPos());
            if( !moveCollisionForce( target1 , newPos1 ) ) {
                target2->pauseFixedPos();
                return RESULT_FIRST;
            }
        }else{
            target1->SetPos(newPos1);
            target2->SetPos(newPos2);
        }

        if( contactTypeX != CR_CONTACT_INVALID ) {
            target1->AddContactCollision(target2,contactTypeX);
            target2->AddContactCollision(target1,contactTypeX2);
        }
        if( contactTypeY != CR_CONTACT_INVALID ) {
            target1->AddContactCollision(target2,contactTypeY);
            target2->AddContactCollision(target1,contactTypeY2);
        }
        return RESULT_AGAIN;
    }else{
        // この状態になったら強制的に前フレームに戻す
//        TB_ASSERT(!"unexpected state");
        if( target1->IsFixedPos() ){
            TbVector2 newPos2 = pre2;
            newPos2 -= reviseMov(pre1 - target1->GetPos());
            if( !moveCollisionForce( target2 , newPos2 ) ) {
                target1->pauseFixedPos();
                return RESULT_FIRST;
            }
        }else if( target2->IsFixedPos() ){
            TbVector2 newPos1 = pre1;
            newPos1 -= reviseMov(pre2 - target2->GetPos());
            if( !moveCollisionForce( target1 , newPos1 ) ) {
                target2->pauseFixedPos();
                return RESULT_FIRST;
            }
        }else{
            target1->SetPos(pre1);
            target2->SetPos(pre2);
        }
        return RESULT_AGAIN;
    }
    return RESULT_OK;
}

/*!
 * 強制移動
 * @author teco
 * @since 2014.09.10
 */
TbBool CrActionCollisionScene::moveCollisionForce( CrActionCollision* src , const TbVector2& pos )
{
    if (!src->IsEnablePushedFromFixedPosCollision() ){
        // 強制移動禁止の場合は失敗
        return false;
    }

    CrActionMap* map = GetMap();
    TbVector2 delta = (pos - src->GetPos());
    TbVector2 movResult;
    src->MoveMap( map , delta , &movResult );
    if( delta != movResult ) {
        return false;
    }
    if( !delta.IsZero() ){
        // 衝突しているもの全てにオフセット
        if( !moveCollidedCollision( src , delta ) ) {
            return false;
        }
    }
    return true;
}

/*!
 * 衝突しているコリジョンを強制移動
 * @author teco
 * @since 2014.09.07
 */
TbBool CrActionCollisionScene::moveCollidedCollision( CrActionCollision* src , const TbVector2& mov )
{
    CrActionMap* map = GetMap();
    TbVector2 movVal = mov;
    TbFloat32 offsetX = 0.0f;
    TbFloat32 offsetY = 0.0f;
#if 1
    if( !TB_FLOAT32_EQUAL_ZERO(mov.GetX()) ){
        offsetX = 0.001f * TbSign(movVal.GetX());
    }
    if( !TB_FLOAT32_EQUAL_ZERO(mov.GetY()) ){
        offsetY = 0.001f * TbSign(movVal.GetY());
    }
#endif
    movVal.Add( offsetX , offsetY );

    if( !src ) {
        return true;
    }
    if (!src->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) ){
        return true;
    }
    if( m_Top ) {
        CrActionCollision* co = m_Top;
        do {
            if( co == src ) {
                continue;
            }
            if( !co->IsActive() || 
                !co->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) )
            {
                continue;
            }
            if( !IsEnableCheck(src->GetLayer(),co->GetLayer()) ){
                continue;
            }
            if( !CrActionCollision::IsCollided( *src , *co ) ){
                continue;
            }
            // エンバグにならなければいいな。。。 
            if (CrActionCollision::IsCollidedPre(*src, *co)) {
                continue;
            }
            // 
            if (!src->IsEnablePushedFromFixedPosCollision() || 
                !co->IsEnablePushedFromFixedPosCollision() )
            {
                // 強制移動禁止の場合は失敗
                return TB_FALSE;
            }

            TbBool isPre = CrActionCollision::IsCollidedPre( *src , *co );



            TbVector2 movResult;
            co->MoveMap( map , movVal , &movResult );
            if( movVal != movResult ) {
                return TB_FALSE;
            }
            // TB_ASSERT(!CrActionCollision::IsCollided( *src , *co ));
            if( !moveCollidedCollision(co,movVal) ) {
                return TB_FALSE;
            }
            // TB_ASSERT(!CrActionCollision::IsCollided( *src , *co ));
        } while( (co = co->m_Next) != m_Top );
    }
    return true;
}

/*!
 * 円形コリジョン同士の押し戻し更新
 * @author teco
 * @since 2014.07.27
 */
CrActionCollisionScene::Result CrActionCollisionScene::updatePushBackCircle( CrActionCollision* target1 , CrActionCollision* target2 , const TbVector2* extPre1 , const TbVector2* extPre2 )
{
    TbCircleF32 circle1 = target1->GetGlobalCircle();
    TbCircleF32 circle2 = target2->GetGlobalCircle();
    TbVector2 pre1 = extPre1 ? *extPre1 : target1->GetPrePos();
    TbVector2 pre2 = extPre2 ? *extPre2 : target2->GetPrePos();
    TbVector2 mov1 = target1->GetPos() - pre1; // 対象の移動量
    TbVector2 movDir1 = mov1.GetNormal();
    TbVector2 mov2 = target2->GetPos() - pre2; // 自身の移動量
    TbVector2 movDir2 = mov2.GetNormal();

    TbFloat32 len = circle1.radius + circle2.radius;
    
    TbVector2 d( circle1.x - circle2.x , circle1.y - circle2.y );
    if( d.Dot(mov2) * d.Dot(mov1) > TB_FLOAT32_EPSILON ) {
        // 自身と対象の向きが近い場合は、前にいるやつは動かさない
        if( d.Dot(mov2) > 0.0f ) {
            mov1.Clear();
        }else{
            mov2.Clear();
        }
    }
    TbVector2 dv( mov2 - mov1 );
    
    TbFloat32 a = dv.LengthSq();
    TbFloat32 b = 2*d.GetX()*dv.GetX() + 2*d.GetY()*dv.GetY();
    TbFloat32 c = d.LengthSq() - TbSquare(len);

    TbFloat32 t = (-b+TbSqrt(b*b-4*a*c)) / (2*a);
    t = TbMin( 1.0f , t ); 

    TbVector2 back1;
    if( !mov1.IsZero() ){
        back1 = mov1 * t + movDir1 * 0.001f;
        if( back1.LengthSq() >= mov1.LengthSq() ){
            back1 = mov1;
        }
    }
    TbVector2 back2;
    if( !mov2.IsZero() ){
        back2 = mov2 * t + movDir2 * 0.001f;
        if( back2.LengthSq() >= mov2.LengthSq() ){
            back2 = mov2;
        }
    }

    TbVector2 newPos1 = target1->GetPos() - back1;
    TbVector2 newPos2 = target2->GetPos() - back2;
    TbVector2 normal = newPos2 - newPos1;
    normal.Normalize();

    target1->SetPos(newPos1);
    target1->AddContactCollision(target2,normal);
    target2->SetPos(newPos2);
    target2->AddContactCollision(target1,-normal);
    return RESULT_AGAIN;
}

/*!
* スケール更新
* coが動いている時、coの上にいるものも一緒に動くようにする
* @author teco
*/
void CrActionCollisionScene::updateScale(CrActionCollision* target)
{
    TB_ASSERT(CrActionCollision::SHAPE_TYPE_RECT==target->GetShapeType());
    if( target->m_ExpectedScale.GetX() < target->m_Scale.GetX() &&
        target->m_ExpectedScale.GetY() < target->m_Scale.GetY() ) {
        // 小さくなるのは許容
        target->m_Scale = target->m_ExpectedScale;
        return;
    }
    TbFloat32 error = 0.001f;
    if (TB_FLOAT32_EQUAL_EPS(target->m_Scale.GetX(), target->m_ExpectedScale.GetX(), error) &&
        TB_FLOAT32_EQUAL_EPS(target->m_Scale.GetY(), target->m_ExpectedScale.GetY(), error)) 
    {
        // 小さな変化は許容しない
        return;
    }
    // 矩形しか許容しない
    TB_RESQUE(CrActionCollision::SHAPE_TYPE_RECT == target->GetShapeType());

    TbVector2 pos = target->GetPos();
    TbRectF32 oldRect = target->GetGlobalRect();
    target->m_Scale = target->m_ExpectedScale;
    if (!target->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION)) {
        return;
    }
    TbRectF32 expectedRect = target->GetGlobalRect();
    target->m_Scale = target->m_PreScale; // 一旦戻す
    TbVector2 sign = target->m_ExpectedScale - target->m_Scale;

    enum
    {
        MOV_LT , 
        MOV_LB,
        MOV_RT ,
        MOV_RB ,
        MOV_MAX
    };
    TbVector2 movSrc[MOV_MAX];
    TbVector2 mov[MOV_MAX];
    {
        movSrc[MOV_LT] = expectedRect.GetLeftTop() - oldRect.GetLeftTop();
        movSrc[MOV_LB] = expectedRect.GetLeftBottom() - oldRect.GetLeftBottom();
        movSrc[MOV_RT] = expectedRect.GetRightTop() - oldRect.GetRightTop();
        movSrc[MOV_RB] = expectedRect.GetRightBottom() - oldRect.GetRightBottom();
    }

    // 背景の判定
    TbFloat32 movRatio = 1.0f;
    {
        CrActionMap* map = GetMap();
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(mov); ++i)
        {
            mov[i] = movSrc[i];
            target->MoveMap(map, mov[i], &mov[i] , TB_FALSE );
            // あくまでも移動量なので、位置を元に戻す
            target->SetPos(pos);
            if(!TB_FLOAT32_EQUAL_EPS(movSrc[i].GetX(), 0.0f, error) ) {
                movRatio = TbMax(0.0f, TbMin(movRatio,mov[i].GetX()/movSrc[i].GetX()));
            }
            if(!TB_FLOAT32_EQUAL_EPS(movSrc[i].GetY(),0.0f, error)) {
                movRatio = TbMax(0.0f,TbMin(movRatio,mov[i].GetY()/movSrc[i].GetY()));
            }
        }
    }

    // コリジョン同士の判定
    if (m_Top)
    {
        CrActionCollision* co = m_Top;
        do {
            if (co == target) {
                continue;
            }
            if (!co->IsActive() ||
                !co->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION))
            {
                continue;
            }

            // 矩形しか許容しない
            TB_RESQUE(CrActionCollision::SHAPE_TYPE_RECT == co->GetShapeType());
            if (!IsEnableCheck(target->GetLayer(), co->GetLayer())) {
                continue;
            }
                
            if ( CrActionCollision::IsCollided(*target, *co )) {
                // 既に衝突してるのは想定外
                TB_ASSERT(0);
                continue;
            }

            if (!TB_VERIFY(!CrActionCollision::IsCollided(*target, *co))) {
                continue;
            }

            for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(mov); ++i)
            {
                TbVector2 movResult;
                target->Move(co, mov[i],&movResult);
                // あくまでも移動量なので、位置を元に戻す
                target->SetPos(pos);
                if (!TB_FLOAT32_EQUAL_EPS(movSrc[i].GetX(),0.0f, error)) {
                    movRatio = TbMax(0.0f, TbMin(movRatio, movResult.GetX() / movSrc[i].GetX()));
                }
                if (!TB_FLOAT32_EQUAL_EPS(movSrc[i].GetY(), 0.0f, error)) {
                    movRatio = TbMax(0.0f, TbMin(movRatio, movResult.GetY() / movSrc[i].GetY()));
                }
            }
        } while ((co = co->m_Next) != m_Top);
    }
    
    // 精度補正
    target->m_Scale = TbLerp(target->m_Scale,target->m_ExpectedScale, movRatio);
    if (!TB_FLOAT32_EQUAL(movRatio, 1.0f)) {
        if( 0.0f < sign.GetX() )
        {
            target->m_Scale.SetX(TbMax(target->m_PreScale.GetX(),target->m_Scale.GetX()-0.02f));
        }
        else
        {
            target->m_Scale.SetX(TbMin(target->m_PreScale.GetX(), target->m_Scale.GetX()+ 0.02f));
        }
        if (0.0f < sign.GetY())
        {
            target->m_Scale.SetY(TbMax(target->m_PreScale.GetY(), target->m_Scale.GetY() - 0.02f));
        }
        else
        {
            target->m_Scale.SetY(TbMin(target->m_PreScale.GetY(), target->m_Scale.GetY() + 0.02f));
        }
    }
}

/*!
 * コリジョン追加
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollisionScene::addCollision( CrActionCollision* collision )
{
    TB_RESQUE(collision && collision->GetScene()==NULL);

    if( m_Top == NULL ){
        m_Top = collision;
        m_Top->m_Next = m_Top;
        m_Top->m_Prev = m_Top;
    }else{
        CrActionCollision* prev = m_Top->m_Prev;
        TbSint32 prio = collision->GetPriority();
        do {
            if( prio <= prev->GetPriority() ){
                break;
            }
            if( prev == m_Top ){
                prev = nullptr;
                break;
            }
            prev = prev->m_Prev;
        } while( 1 );
        CrActionCollision* next = nullptr;
        if( prev ) {
            next = prev->m_Next;
        }else{
            next = m_Top;
        }
        collision->m_Prev = next->m_Prev;
        collision->m_Next = next;
        next->m_Prev->m_Next = collision;
        next->m_Prev = collision;
        if( !prev ){
            m_Top = collision;
        }
    }
}

/*!
 * コリジョン除去
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollisionScene::removeCollision( CrActionCollision* collision )
{
    TB_RESQUE(collision && collision->GetScene()==this);
    if( m_Top == collision ){
        if( m_Top == m_Top->m_Next ) {
            m_Top = NULL;
        }else{
            collision->m_Next->m_Prev = collision->m_Prev;
            collision->m_Prev->m_Next = collision->m_Next;
            m_Top = collision->m_Next;
        }
    }else{
        collision->m_Next->m_Prev = collision->m_Prev;
        collision->m_Prev->m_Next = collision->m_Next;
    }
    collision->m_Prev = NULL;
    collision->m_Next = NULL;
}

/*!
 * 前段階で
 * @author teco
 * @since 2014.09.07
 */
void CrActionCollisionScene::CheckCollided( TbBool isPre )
{
#if TB_IS_DEBUG
    if( !m_Top ){
        return;
    }
    CrActionCollision* base = m_Top;
    CrActionCollision* next = base->m_Next;
    if( next == m_Top ) {
        return;
    }

    Result res = RESULT_OK;

    do {
        if( base->IsActive() && 
            base->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) )
        {
            do {
                // 衝突処理
                if( next->IsActive() &&
                    next->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) &&
                    IsEnableCheck(base->GetLayer(),next->GetLayer()) )
                {
                   if( isPre ? CrActionCollision::IsCollidedPre( *base , *next ) :

                       CrActionCollision::IsCollided( *base , *next ) ){
                       TB_ASSERT(0);
                    }
                }
                next = next->m_Next;
            } while( next != m_Top );
        }

        // 基準を変更
        base = base->m_Next;
        next = base->m_Next;
    } while( next != m_Top );
#endif
}

/*!
 * デバッグ描画
 * @author teco
 */
void CrActionCollisionScene::DrawDebug( const TbMatrix& poseMatrix )
{
#if TB_IS_DEBUG
    TbDraw2dBegin();
    CrActionCollision* co = m_Top;
    if( co )
    {
        do {
            if( co->IsActive() ) {
                TbRectF32 rect = co->GetGlobalRect();
                TbDrawLineRectParam param;
                param.SetRect(rect);
                param.color = TbColor::Red();
                param.matrix = poseMatrix;
                TbDrawLine(param);
            }
            co = co->m_Next;
        } while( co != m_Top );
    }
    TbDraw2dEnd();
#endif
}

}
