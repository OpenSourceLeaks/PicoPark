/*!
 * アクション用コリジョン
 * @author teco
 * @since 2012.05.25
 */

#include <tb_fwd.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/collision/cr_action_collision_scene.h>
#include <crayon//map/cr_action_map.h>
#include <base/algorithm/tb_search.h>

using namespace toybox;

namespace
{

struct AngleRect
{
    AngleRect() : angle(0)
    {}

    // 角度設定
    void SetAngle( TbAngle32 ang )
    {
        v[0].Set( src.x             , src.y );
        v[1].Set( src.x             , src.y + src.height );
        v[2].Set( src.x + src.width , src.y + src.height );
        v[3].Set( src.x + src.width , src.y );
        v[4].Set( src.x , src.y );
        if( 0 == angle ) {
            return;
        }
        TbRadian rad = TbAng32ToRad(ang);
        TbFloat32 c = TbCos(rad);
        TbFloat32 s = TbSin(rad);
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(v); ++i ) {
            TbFloat32 x = c*v[i].GetX()-s*v[i].GetY();
            TbFloat32 y = s*v[i].GetX()+c*v[i].GetY();
            v[i].Set(x,y);
        }
        angle = ang;
    };

    void AddPos( const TbVector2& pos ) {
        src.x += pos.GetX();
        src.y += pos.GetY();
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(v); ++i ) {
            v[i].Add(pos.GetX(),pos.GetY());
        }
    }

    TbRectF32 src;          // 元矩形
    TbVector2 v[5];         // 座標
    TbVector2 normal[4];    // 法線
    TbAngle32 angle;        // 角度
};

/*!
 * 衝突しているか
 * @author teco
 * @since 2013.10.06
 */
TbBool isCollidedRect( TbRectF32 colli1 , 
                       TbRectF32 colli2 , 
                       const TbVector2* ofs1 ,
                       const TbVector2* ofs2 )
{
    if( ofs1 ) {
        colli1.x += ofs1->GetX();
        colli1.y += ofs1->GetY();
    }
    if( ofs2 ) {
        colli2.x += ofs2->GetX();
        colli2.y += ofs2->GetY();
    }
    if( colli1.x < (colli2.x+colli2.width) &&
        colli2.x < (colli1.x+colli1.width) &&
        colli1.y < (colli2.y+colli2.height) &&
        colli2.y < (colli1.y+colli1.height) )
    {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 衝突しているか
 * @author teco
 * @since 2013.10.06
 */
TbBool isCollidedCircle(  TbCircleF32 colli1 , 
                          TbCircleF32 colli2 , 
                          const TbVector2* ofs1 ,
                          const TbVector2* ofs2 )
{
    if( ofs1 ) {
        colli1.x += ofs1->GetX();
        colli1.y += ofs1->GetY();
    }
    if( ofs2 ) {
        colli2.x += ofs2->GetX();
        colli2.y += ofs2->GetY();
    }
    TbVector2 delta( colli1.x-colli2.x , colli1.y-colli2.y );
    if( delta.Length() < (colli1.radius+colli2.radius) )
    {
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 衝突しているか
 * @author teco
 * @since 2013.10.06
 */
TbBool isCollidedCircleAndRect(  TbCircleF32 colli1 , 
                                 TbRectF32 colli2 , 
                                 const TbVector2* ofs1 ,
                                 const TbVector2* ofs2 )
{
    if( ofs1 ) {
        colli1.x += ofs1->GetX();
        colli1.y += ofs1->GetY();
    }
    if( ofs2 ) {
        colli2.x += ofs2->GetX();
        colli2.y += ofs2->GetY();
    }
    // 内側か
#if 0
    TbVector2 delta( colli1.x-colli2.x , colli1.y-colli2.y );
    if( delta.Length() < (colli1.radius+colli2.radius) )
    {
        return TB_TRUE;
    }
#endif
    return TB_FALSE;
}

/*!
 * 衝突しているか(線分)
 * @author teco
 */
TbBool isCollidedSegment( const TbVector2& a1 , 
                          const TbVector2& a2 , 
                          const TbVector2& b1 , 
                          const TbVector2& b2 )
{
    TbVector2 delta = a2 - a1;
    TbFloat32 len = delta.Length();
    if( TB_FLOAT32_EQUAL_ZERO(len) ){
        return TB_FALSE;
    }
    delta.Normalize();
    TbFloat32 cross1 = delta.Cross(b1);
    TbFloat32 cross2 = delta.Cross(b2);
    if( cross1 * cross2 >= 0.0f ) { // 符号が同じ場合は絶対交差してない
        return TB_FALSE;
    }
    TbVector2 b1a1 = b1 - a1;
    TbVector2 b2a1 = b2 - a1;
    TbFloat32 d1 = delta.Dot( b1a1 );
    TbFloat32 d2 = delta.Dot( b2a1 );
    if( 0.0f <= d1 && d1 <= len &&
        0.0f <= d2 && d2 <= len )
    {
        
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 衝突しているか(回転付き矩形)
 * @author teco
 */
TbBool isCollidedAngleRect( AngleRect colli1 , 
                            AngleRect colli2 , 
                            const TbVector2* ofs1 ,
                            const TbVector2* ofs2 , 
                            TbVector2* resultNormal )
{
    if( ofs1 ) {
        colli1.AddPos(*ofs1);
    }
    if( ofs2 ) {
        colli2.AddPos(*ofs2);
    }
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(colli1.v)-1; ++i ) {
        for( TbSizeT j = 0; j < TB_ARRAY_COUNT_OF(colli2.v)-1; ++j ) {
            if( isCollidedSegment( colli1.v[i] , colli1.v[i+1] ,
                                   colli2.v[j] , colli2.v[j+1] ) )
            {
                if( resultNormal ) {
                    *resultNormal = colli2.normal[j];
                }
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}

}

namespace crayon
{

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.04
 */
CrActionCollision::CrActionCollision()
    : m_Layer(0)
    , m_Priority(0)
    , m_Pos()
    , m_ShapeType(SHAPE_TYPE_RECT)
    , m_Rect()
    , m_ExpectedScale(1.0f, 1.0f)
    , m_Scale(1.0f,1.0f)
    , m_PreScale(1.0f, 1.0f)
    , m_BitArray()
    , m_Attribute(0)
    , m_PushBackTarget(0)
    , m_Actor(NULL)
    , m_CollidedCallback()
    , m_ContactAddedCallback()
    , m_ContactRemovedCallback()
    , m_Scene(NULL)
    , m_Next(NULL)
    , m_Prev(NULL)
    , m_ContactedMap(NULL)
{
    TbMemClear(m_UserDataU32);
    TbMemClear(m_UserDataF32);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.10.04
 */
CrActionCollision::~CrActionCollision()
{
    Exit();
}

/*!
 * 初期化
 * @author teco
 * @since 2013.10.04
 */
void CrActionCollision::Initialize( const TbRectF32& rect , TbUint32 pushBackTargetBit )
{
    m_ShapeType = SHAPE_TYPE_RECT;
    m_Rect = rect;
    initialize(pushBackTargetBit);
}

// 
/*!
 * 初期化
 * @author teco
 * @since 2014.07.27
 */
void CrActionCollision::Initialize( const TbCircleF32& circle , TbUint32 pushBackTargetBit )
{
    m_ShapeType = SHAPE_TYPE_CIRCLE;
    m_Circle = circle;
    m_Rect.x       = circle.x-circle.radius;
    m_Rect.y       = circle.y-circle.radius;
    m_Rect.width   = circle.radius*2.0f;
    m_Rect.height  = circle.radius*2.0f;
    initialize(pushBackTargetBit);
}

/*!
 * 初期化
 * @author teco
 * @since 2014.07.27
 */
void CrActionCollision::initialize( TbUint32 pushBackTargetBit )
{
    SetPushBackTarget(pushBackTargetBit);
    m_BitArray.SetBit(FLAG_ACTIVE);
    m_ContactCollision.SetCapacity(CONTACT_ACTOR_MAX_DEFAULT);
    m_ContactCollision.SetEnableSetCapacityAuto(TB_TRUE);
}

/*!
 * 接触アクター追加
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollision::AddContactCollision( CrActionCollision* collision , const TbVector2& normal )
{
    ContactArray::Iterator ite = TbFind(m_ContactCollision,collision );
    if( ite == m_ContactCollision.End() ){
        ContactInfo info = { ContactInfo::STATE_NEW , collision , normal };
        m_ContactCollision.PushBack(info);
        
        // 即時には追加コールバックを呼ばない        
    }
}

/*!
 * 接触コリジョン除去
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollision::RemoveContactCollision( CrActionCollision* collision )
{
    CrActionCollisionInfo info;
    info.collision = collision;
    ContactArray::Iterator ite = TbFind(m_ContactCollision,collision );
    if( ite != m_ContactCollision.End() ){
        if( ite->state != ContactInfo::STATE_NEW  ){
            if( m_ContactRemovedCallback.IsValid() ){
                // コールバックを呼ぶ
                m_ContactRemovedCallback(ite->normal,CONTACT_TARGET_COLLISION,info);
            }
        }
        m_ContactCollision.Erase(ite);
    }
}

/*!
 * 接触コリジョンクリア
 * @author teco
 */
void CrActionCollision::ClearContactCollision()
{
    TbForIterator(ite, m_ContactCollision)
    {
        CrActionCollisionInfo info;
        info.collision = ite->collision;
        if (m_ContactRemovedCallback.IsValid()) {
            // コールバックを呼ぶ
            m_ContactRemovedCallback(ite->normal, CONTACT_TARGET_COLLISION, info);
        }
    }
    m_ContactCollision.Clear();
}


/*!
 * マップとの状態更新
 * @author teco
 * @since 2013.10.14
 */
TbBool CrActionCollision::MoveMap( CrActionMap* map , 
                                   const TbVector2& mov , 
                                   TbVector2* movResult , 
                                   TbBool isEnableSlide )
{
    TB_ASSERT(map);
    if( !IsActive() ){
        return TB_FALSE;
    }
    if( !IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_MAP) ) {
        m_Pos += mov;
        return TB_TRUE;
    }

    TbVector2 workMov = mov;
    if (movResult) {
        movResult->Clear();
    }

    // マップ取得
    while( 1 )
    {
        TbVector2 result;
        TbVector2 normal = map->Move( this , workMov , &result );
        if( movResult ){
            *movResult += result;
        }
        TB_ASSERT( normal.Length() < 100.0f );
        if( TB_FLOAT32_EPSILON < normal.LengthSq() ){
            addMapContact(normal,map);
            if(isEnableSlide)
            {
                // 壁滑りが可能な場合
                workMov -= result;
                workMov -= normal*(normal.Dot(workMov));
            }
            else
            {
                break;
            }
        }else{
            break;
        }
    }

    return TB_TRUE;
}

// 指定コリジョンに接触せずに移動
TbBool   CrActionCollision::Move(CrActionCollision* co, const toybox::TbVector2& mov, toybox::TbVector2* movResult )
{
    TB_RESQUE_RET(SHAPE_TYPE_RECT == GetShapeType() && SHAPE_TYPE_RECT == co->GetShapeType(),TB_FALSE);

    static const TbFloat32 MOV_ERROR = 0.01f;
    CrContactFace contactTypeX = CR_CONTACT_INVALID; // X軸接面フラグ
    TbFloat32 contactTimeX1 = 0.0f;
    CrContactFace contactTypeY = CR_CONTACT_INVALID; // Y軸接面フラグ
    TbFloat32 contactTimeY1 = 0.0f;

    TbVector2 targetPos = GetPos() + mov;
    TbVector2 pre1 = GetPos();
    SetPos(targetPos);
    TbRectF32 rect1 = GetGlobalRect();
    TbRectF32 rect2 = co->GetGlobalRect();

    if (!IsCollided(*this, *co)) {
        if (movResult) {
            *movResult = mov;
        }
        return TB_TRUE;
    }

    // X軸の接触チェック
    if (mov.GetX() != 0.0f)
    {
        TbFloat32 x1l = (rect1.x - mov.GetX());
        TbFloat32 x1r = rect1.x + rect1.width - mov.GetX();
        TbFloat32 x2l = (rect2.x);
        TbFloat32 x2r = (rect2.x + rect2.width);

        // X軸方向に接面があるか
        if (x1r <= x2l) {
            contactTimeX1 = (rect1.x + rect1.width - rect2.x) / (-mov.GetX());
            contactTypeX = CR_CONTACT_RIGHT;
        }
        else if (x2r <= x1l) {
            contactTimeX1 = (rect2.x + rect2.width - rect1.x) / mov.GetX();
            contactTypeX = CR_CONTACT_LEFT;
        }
        contactTimeX1 = TbClamp<TbFloat32>(contactTimeX1, -1.0f, 0.0f);
    }
    // Y軸の接触チェック
    if (mov.GetY() != 0.0f)
    {
        TbFloat32 y1u = (rect1.y - mov.GetY());
        TbFloat32 y1d = (rect1.y + rect1.height - mov.GetY());
        TbFloat32 y2u = (rect2.y);
        TbFloat32 y2d = (rect2.y + rect2.height);
        // Y軸方向に接面があるか
        if (y1d <= y2u) {
            contactTimeY1 = (rect1.y + rect1.height - rect2.y) / (-mov.GetY());
            contactTypeY = CR_CONTACT_DOWN;
        }
        else if (y2d <= y1u) {
            contactTimeY1 = (rect2.y + rect2.height - rect1.y) / (mov.GetY());
            contactTypeY = CR_CONTACT_UP;
        }
        contactTimeY1 = TbClamp<TbFloat32>(contactTimeY1, -1.0f, 0.0f);
    }

    if (contactTypeX != CR_CONTACT_INVALID || contactTypeY != CR_CONTACT_INVALID)
    {
        TbVector2 newPos1(targetPos.GetX() + mov.GetX()*(contactTimeX1),
                          targetPos.GetY() + mov.GetY()*(contactTimeY1));
        {
            TbVector2 delta = (newPos1 - targetPos);
            if (!TB_FLOAT32_EQUAL(contactTimeX1, 0.0f) &&
                !TB_FLOAT32_EQUAL(mov.GetX(), 0.0f) &&
                TB_FLOAT32_EQUAL(delta.GetX(), 0.0f))
            {
                newPos1.SetX(pre1.GetX());
            }
            if (!TB_FLOAT32_EQUAL(contactTimeY1, 0.0f) &&
                !TB_FLOAT32_EQUAL(mov.GetY(), 0.0f) &&
                TB_FLOAT32_EQUAL(delta.GetY(), 0.0f))
            {
                newPos1.SetY(pre1.GetY());
            }
        }
        CrContactFace contactTypeX2 = CR_CONTACT_INVALID;
        CrContactFace contactTypeY2 = CR_CONTACT_INVALID;
        switch (contactTypeX)
        {
        case CR_CONTACT_LEFT:
        {
            if (contactTimeX1 <= 0.0f && !TB_FLOAT32_EQUAL(mov.GetX(), 0.0f)) {
                newPos1.Add(MOV_ERROR, 0.0f);
                newPos1.SetX(TbMin(newPos1.GetX(), pre1.GetX()));
            }
            else {
                TB_ASSERT(0);
            }
            contactTypeX2 = CR_CONTACT_RIGHT;
        }
        break;
        case CR_CONTACT_RIGHT:
        {
            if (contactTimeX1 <= 0.0f && !TB_FLOAT32_EQUAL(mov.GetX(), 0.0f)) {
                newPos1.Add(-MOV_ERROR, 0.0f);
                newPos1.SetX(TbMax(newPos1.GetX(), pre1.GetX()));
            }
            else {
                TB_ASSERT(0);
            }
            contactTypeX2 = CR_CONTACT_LEFT;
        }
        break;
        case CR_CONTACT_INVALID:
            break;
        default:
            TB_RESQUE_RET(0, TB_FALSE);
            break;
        }
        switch (contactTypeY)
        {
        case CR_CONTACT_UP:
        {
            if (contactTimeY1 <= 0.0f && !TB_FLOAT32_EQUAL(mov.GetY(), 0.0f)) {
                newPos1.Add(0.0f, MOV_ERROR);
                newPos1.SetY(TbMin(newPos1.GetY(), pre1.GetY()));
            }
            else {
                TB_ASSERT(0);
            }
            contactTypeY2 = CR_CONTACT_DOWN;
        }
        break;
        case CR_CONTACT_DOWN:
        {
            if (contactTimeY1 <= 0.0f && !TB_FLOAT32_EQUAL(mov.GetY(), 0.0f)) {
                newPos1.Add(0.0f, -MOV_ERROR);
                newPos1.SetY(TbMax(newPos1.GetY(), pre1.GetY()));
            }
            else {
                TB_ASSERT(0);
            }
            contactTypeY2 = CR_CONTACT_UP;
        }
        break;
        case CR_CONTACT_INVALID:
            break;
        default:
            TB_RESQUE_RET(0,TB_FALSE);
            break;
        }
        SetPos(newPos1);
    } 
    if(movResult ){
        *movResult = GetPos() - pre1;
    }
    return TB_TRUE;
}

/*!
 * 位置修正が必要な処理が終わった後に呼ばれる関数
 * @author teco
 * @since 2013.10.04
 */
void CrActionCollision::updateAfter()
{
    // 一旦ここで位置は確定しているので保存
    m_PrePos = m_Pos;
    if (m_Scale != m_ExpectedScale) {
        if (m_ChangedScaleCallback.IsValid()) {
            m_ChangedScaleCallback( m_Scale.GetX() , m_Scale.GetY() );
        }
    }
    m_PreScale = m_Scale;
    m_ExpectedScale = m_Scale;
    
    // 押し戻されていたらここで更新
    TbVector2 pushBackVec = m_Pos-m_ExpectedPos;
    TbFloat32 pushBackLen = pushBackVec.LengthSq();
    if( !TB_FLOAT32_EQUAL(pushBackLen,0.0f) ){
        if( m_ChangedPosCallback.IsValid() ){
            m_ChangedPosCallback(m_Pos,pushBackVec);
        }
        updateExpectedPos();
    }

    // 接触しているオブジェクト&マップとの状態更新(押し戻しによって接触状態が変化している可能性があるため)
    updateContactCollision();
    updateContactMap();
}

/*!
 * 接触しているコリジョンの状態更新
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollision::updateContactCollision()
{
    static const TbFloat32 OFS = 0.5f;
    
    if (IsEnableForceSearchUpContact())
    {
        CrActionCollisionScene::SearchResult searchResult;
        TbVector2 scale = GetScale();
        SetScale(TbVector2(scale.GetX()*0.5f,scale.GetY()));
        m_Scene->SearchCollidedObjectByPushBackTarget(&searchResult,*this,CR_CONTACT_UP);
        TbForIterator(it, searchResult)
        {
            AddContactCollision(*it,CR_CONTACT_UP);
        }
        SetScale(scale);
    }

    ContactArray::Iterator ite = m_ContactCollision.Begin();
    for( ; ite != m_ContactCollision.End(); ) {
        CrActionCollisionInfo info;
        TB_ASSERT(!m_ContactCollision.IsEmpty());
        const TbVector2& ofsVec = ite->normal*OFS;

        if( CrActionCollision::IsCollided(*this,*ite->collision,&ofsVec,NULL) ){
            info.collision = ite->collision;
            if( ite->state == ContactInfo::STATE_NEW ) {
                // 追加コールバックを呼んでおく
                ite->state = ContactInfo::STATE_DEFAULT;
                if( m_ContactAddedCallback.IsValid() ){
                    m_ContactAddedCallback(ite->normal,CONTACT_TARGET_COLLISION,info);
                }
            }
            if( m_ContactedCallback.IsValid() ){
                m_ContactedCallback( ite->normal , CONTACT_TARGET_COLLISION,info );
            }
            TB_ASSERT(!m_ContactCollision.IsEmpty());
            ++ite;
        }else{
            if( ite->state == ContactInfo::STATE_DEFAULT ) {
                if( m_ContactRemovedCallback.IsValid() ){
                    // コールバックを呼ぶ
                    info.collision = ite->collision;
                    m_ContactRemovedCallback(ite->normal,CONTACT_TARGET_COLLISION,info);
                }
            }
            TB_ASSERT(!m_ContactCollision.IsEmpty());
            // 衝突解除
            ite = m_ContactCollision.Erase(ite);
//            TB_ASSERT(list.IsEmpty()||ite!=list.Begin());
        }
    }
}

/*!
 * 接触しているマップの状態更新
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollision::updateContactMap()
{
    CrActionCollisionInfo info;
    info.map = m_ContactedMap;
    // 除去
    for( auto ite = m_ContactMapArray.Begin(); ite != m_ContactMapArray.End(); ) {
        TB_ASSERT(ite->normal.Length() < 100.0f);
        if( !IsActive() || (m_ContactedMap && !m_ContactedMap->IsContacted( this , ite->normal , &info.point )) ){
            if( m_ContactRemovedCallback.IsValid() ) {
                m_ContactRemovedCallback( ite->normal , CONTACT_TARGET_MAP , info );
            }
            ite = m_ContactMapArray.Erase(ite);
        }else{
            if( m_ContactedCallback.IsValid() ){
                m_ContactedCallback( ite->normal , CONTACT_TARGET_MAP , info );
            }
            ++ite;
        }
    }
    // 追加
    TbForIterator( ite , m_ContactMapArrayRequest ) {
        // マップとの接触状態
        if( m_ContactedMap && m_ContactedMap->IsContacted( this , ite->normal ,  &info.point ) ){
            if( m_ContactAddedCallback.IsValid() ){
                m_ContactAddedCallback(ite->normal,CrActionCollision::CONTACT_TARGET_MAP,info);
            }
            if( m_ContactedCallback.IsValid() ){
                m_ContactedCallback( ite->normal , CONTACT_TARGET_MAP , info );
            }
            TB_ASSERT(ite->normal.Length() < 100.0f);
            m_ContactMapArray.PushBack(*ite);
        }
    }
    m_ContactMapArrayRequest.Clear();
}

/*!
 * シーンに入る
 * @author teco
 * @since 2013.10.06
 */
TbBool CrActionCollision::Enter( CrActionCollisionScene* scene )
{
    TB_ASSERT(scene);
    if( m_Scene ) {
        return false;
    }
    scene->addCollision(this);
    m_Scene = scene;
    return true;
}

/*!
 * シーンから出る
 * @author teco
 * @since 2013.10.06
 */
TbBool CrActionCollision::Exit()
{
    if( !m_Scene ) {
        return false;
    }
    m_Scene->removeCollision(this);
    m_Scene = NULL;
    resetContact();
    return true;
}

/*!
 * 接触解除
 * @author teco
 * @since 2013.10.06
 */
void CrActionCollision::resetContact()
{
    ContactArray::Iterator ite = m_ContactCollision.Begin();
    ContactArray::Iterator endIte = m_ContactCollision.End();
    while( ite != endIte ) {
        // 接触状態解除
        ite->collision->RemoveContactCollision(this);
        ++ite;
    }
    m_ContactCollision.Clear();
    m_ContactMapArray.Clear();
    m_ContactedMap = NULL;
}

/*!
 * マップとの接触情報追加
 * @author teco
 * @since 2013.10.14
 */
void CrActionCollision::addMapContact( const TbVector2& normal , CrActionMap* map )
{
    if( TbFind( m_ContactMapArray , normal ) == m_ContactMapArray.End() && 
        TbFind( m_ContactMapArrayRequest , normal ) == m_ContactMapArrayRequest.End()
      )
    {
        TB_ASSERT(normal.Length() < 100.0f);

        TB_ASSERT(m_ContactedMap==NULL||m_ContactedMap==map);
        ContactMapInfo info = { normal };
        m_ContactMapArrayRequest.PushBack(info);
        m_ContactedMap = map;
    }
}

/*!
 * 衝突しているか
 * @author teco
 * @since 2013.10.06
 */
TbBool CrActionCollision::IsCollided( const CrActionCollision& target1 , 
                                    const CrActionCollision& target2 , 
                                    const TbVector2* ofs1 ,
                                    const TbVector2* ofs2 ,
                                    TbBool    force )
{
    if( !force && ( !target1.IsActive() || !target2.IsActive() ) ){
        return TB_FALSE;
    }
    
    // 円形同士のみ専用処理
    if( SHAPE_TYPE_CIRCLE == target1.GetShapeType() && 
        SHAPE_TYPE_CIRCLE == target2.GetShapeType() )
    {
        const TbCircleF32& circle1 = target1.GetGlobalCircle();
        const TbCircleF32& circle2 = target2.GetGlobalCircle();
        return isCollidedCircle( circle1 , circle2 , ofs1 , ofs2 );
    }

    TbRectF32 colli1 = target1.GetGlobalRect();
    TbRectF32 colli2 = target2.GetGlobalRect();
    return isCollidedRect(colli1,colli2,ofs1,ofs2);
}

/*!
 * 衝突しているか
 * @author teco
 * @since 2013.10.06
 */
TbBool CrActionCollision::IsCollidedPre(  const CrActionCollision& target1 , 
                                        const CrActionCollision& target2 , 
                                        const TbVector2* ofs1 ,
                                        const TbVector2* ofs2 ,
                                        TbBool    force )
{
    if (!force && (!target1.IsActive() || !target2.IsActive())) {
        return TB_FALSE;
    }

    // 円形同士のみ専用処理
    if( SHAPE_TYPE_CIRCLE == target1.GetShapeType() && 
        SHAPE_TYPE_CIRCLE == target2.GetShapeType() )
    {
        const TbCircleF32& circle1 = target1.GetPreGlobalCircle();
        const TbCircleF32& circle2 = target2.GetPreGlobalCircle();
        return isCollidedCircle( circle1 , circle2 , ofs1 , ofs2 );
    }

    TbRectF32 colli1 = target1.GetPreGlobalRect();
    TbRectF32 colli2 = target2.GetPreGlobalRect();
    return isCollidedRect(colli1,colli2,ofs1,ofs2);
}

/*!
 * 接触コリジョン取得
 * @author teco
 * @since 2014.07.26
 */
TbUint32 CrActionCollision::GetContactCollision( CrActionCollision* (&dst)[CONTACT_GET_MAX] , const TbVector2& contactNormal ) const
{
    TbUint32 count = 0;
    TbForIterator( ite , m_ContactCollision )
    {
        if( ite->normal == contactNormal ){
            TB_ASSERT(count<CONTACT_GET_MAX);
            dst[count++] = ite->collision;
        }
    }
    return count;
}

/*!
 * 接触コリジョン存在するか
 * @author teco
 */
TbBool CrActionCollision::ExistsContactCollision(CrActionCollision* src, const toybox::TbVector2& contactNormal) const
{
    TbForIterator(ite, m_ContactCollision)
    {
        if (ite->collision == src) {
            if (ite->normal == contactNormal) {
                return TB_TRUE;
            }
            return TB_FALSE;
        }
    }
    return TB_FALSE;
}

/*!
 * 接触コリジョン取得
 * @author teco
 * @since 2014.08.09
 */
TbUint32 CrActionCollision::GetContactCount( const TbVector2& contactNormal , 
                                           TbBool isRecursive , 
                                           TbStaticArray<CrActionCollision*,CONTACT_GET_MAX>* checkList ,
                                           TbUint32 collisionLayerMask ) const
{
    TbUint32 count = 0;
    TbStaticArray<CrActionCollision*,CONTACT_GET_MAX> srcCheckList; 
    if( !checkList ) {
        checkList = &srcCheckList;
    }
    TbForIterator( ite , m_ContactCollision )
    {
        if( !(collisionLayerMask & TB_BIT(ite->collision->GetLayer())) ){
            continue;
        }
        // todo 線形探索要修正
        if( ite->normal == contactNormal && TbFind(*checkList,ite->collision) == checkList->End() ){
            ++count;
            checkList->PushBack(ite->collision);
            if( isRecursive ){
                count += ite->collision->GetContactCount(contactNormal,isRecursive,checkList,collisionLayerMask);
            }
        }
    }
    return count;
}

/*!
 * 再帰最大数
 * @author teco
 */
TbUint32 CrActionCollision::GetContactRecursiveMax( const toybox::TbVector2& contactNormal ,
                                                    TbUint32 collisionLayerMask )
{
    TbUint32 count = 0;
    TbUint32 subCount = 0;
    TbForIterator(ite, m_ContactCollision)
    {
        if (!(collisionLayerMask & TB_BIT(ite->collision->GetLayer()))) {
            continue;
        }
        if ( ite->normal == contactNormal ) {
            count = 1;
            subCount = TbMax(subCount,ite->collision->GetContactRecursiveMax(contactNormal,collisionLayerMask));
        }
    }
    count += subCount;
    return count;
}

/*!
 * 指定の法線方向で接触しているか
 * @author teco
 * @since 2014.07.25
 */
TbBool CrActionCollision::IsContacted( const TbVector2& contactNormal , TbBool isEnableMap , TbUint32* attribute ) const
{
    TbBool result = TB_FALSE;
    TbForIterator( ite , m_ContactCollision )
    {
        if( ite->normal == contactNormal ){
            result = TB_TRUE;
            if (attribute){
                *attribute |= ite->collision->GetAttribute();
            }else {
                return TB_TRUE;
            }
        }
    }
    if (result) {
        return result;
    }
    if( isEnableMap ){
        TbForIterator( ite , m_ContactMapArray )
        {
            if( ite->normal == contactNormal ) {
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}

/*!
 * 指定の法線方向でマップと接触しているか
 * @author teco
 * @since 2014.10.21
 */
TbBool CrActionCollision::IsContactedMap( const TbVector2& contactNormal ) const
{
    TbForIterator( ite , m_ContactMapArray )
    {
        if( ite->normal == contactNormal ) {
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

}