/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "tb_fwd.h"
#include "crayon/map/cr_action_map.h"
#include "crayon/actor/cr_actor.h"
#include "crayon/collision/cr_action_collision.h"
#include <graphics/render/tb_draw_2d.h>

using namespace toybox;

static const TbFloat32 ERROR_MOV = 0.01f;

namespace crayon
{

const TbUint32 MAP_CHIP_ATTR[] = 
{
    // 土管
    CR_TILE_MAP_CHIP_ATTR_COLLISION | CR_TILE_MAP_CHIP_ATTR_FRONT ,
    CR_TILE_MAP_CHIP_ATTR_COLLISION | CR_TILE_MAP_CHIP_ATTR_FRONT , 
    CR_TILE_MAP_CHIP_ATTR_COLLISION | CR_TILE_MAP_CHIP_ATTR_FRONT ,
    CR_TILE_MAP_CHIP_ATTR_COLLISION | CR_TILE_MAP_CHIP_ATTR_FRONT ,
    CR_TILE_MAP_CHIP_ATTR_COLLISION ,
    CR_TILE_MAP_CHIP_ATTR_COLLISION ,
    CR_TILE_MAP_CHIP_ATTR_COLLISION ,
    CR_TILE_MAP_CHIP_ATTR_COLLISION ,
    CR_TILE_MAP_CHIP_ATTR_COLLISION
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(MAP_CHIP_ATTR)==CR_ACTION_MAP_CHIP_MAX);

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.11
 */
CrActionMap::CrActionMap(const toybox::TbRectF32* mapUvRecSTable)
    : Super()
    , m_UvRect(mapUvRecSTable)
{
    SetUserMapChipAttrTable(MAP_CHIP_ATTR,TB_ARRAY_COUNT_OF(MAP_CHIP_ATTR));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.11
 */
CrActionMap::~CrActionMap()
{
}

/*!
 * アクターを移動する
 * @author teco
 * @since 2013.09.11
 */
TbVector2 CrActionMap::Move( CrActionCollision* collision , const TbVector2& mov , TbVector2* resultCorrectMov )
{
    if( mov.Length() < TB_FLOAT32_EPSILON || !collision ) {
        return TbVector2();
    }

    TB_ASSERT(!mov.HasNan());
//    TB_ASSERT(!IsContacted(collision, CR_CONTACT_LEFT) || !IsContacted(collision, CR_CONTACT_RIGHT));
//    TB_ASSERT(!IsContacted(collision, CR_CONTACT_DOWN) || !IsContacted(collision, CR_CONTACT_UP));

    CrContactFace result = CR_CONTACT_INVALID;
    const TbRectF32& rect = collision->GetRect();
    const TbVector2& pos = collision->GetPos();
    TbVector2 correctMov = mov;
    TbPointS32 unit( 0 , 0 );
    TbUint32 chipSize =GetChipSize();
    if( mov.GetX() > TB_EPSILON ) {
        unit.x = 1;
    }else if( mov.GetX() < -TB_EPSILON ){
        unit.x = -1;
    }
    if( mov.GetY() > TB_EPSILON ) {
        unit.y = 1;
    }else if( mov.GetY() < -TB_EPSILON ){
        unit.y = -1;
    }

    // 既に移動不可チップと接していたらその方向の移動量は無効化する
    if( unit.x != 0 ){
        CrContactFace contact = unit.x > 0 ? CR_CONTACT_RIGHT : CR_CONTACT_LEFT;
        if( IsContacted(collision, CrUtil::GetContactFaceNormal(contact) ) ){
            correctMov.SetX(0.0f);
            unit.x = 0;
            result = contact;
        }
    }
    if( unit.y != 0 ){
        CrContactFace contact = unit.y > 0 ? CR_CONTACT_DOWN : CR_CONTACT_UP;
        if( IsContacted(collision,CrUtil::GetContactFaceNormal(contact) ) ){
            correctMov.SetY(0.0f);
            unit.y = 0;
            result = contact;
        }
    }

    TbVector2 beginPos( pos.GetX()+rect.x , pos.GetY()+rect.y );
    TbFloat32 chipSizeF = static_cast<TbFloat32>(chipSize);
    TbRectS32 beginPoint( static_cast<TbSint32>(beginPos.GetX() / chipSizeF) ,
                          static_cast<TbSint32>(beginPos.GetY() / chipSizeF) ,
                          static_cast<TbSint32>((beginPos.GetX()+rect.width) / chipSizeF) ,
                          static_cast<TbSint32>((beginPos.GetY()+rect.height) / chipSizeF) );
    TbRectS32 endPoint( static_cast<TbSint32>((beginPos.GetX()+correctMov.GetX()) / chipSizeF) ,
                        static_cast<TbSint32>((beginPos.GetY()+correctMov.GetY()) / chipSizeF) ,
                        static_cast<TbSint32>((beginPos.GetX()+correctMov.GetX()+rect.width) / chipSizeF) ,
                        static_cast<TbSint32>((beginPos.GetY()+correctMov.GetY()+rect.height) / chipSizeF) );
    beginPoint.width = beginPoint.width-beginPoint.x+1;
    beginPoint.height = beginPoint.height-beginPoint.y+1;
    endPoint.width = endPoint.width-endPoint.x+1;
    endPoint.height = endPoint.height-endPoint.y+1;

    TbPointS32 curPoint( beginPoint.x , beginPoint.y );
    TbPointS32 movPoint;
    TbPointS32 distance( endPoint.x - beginPoint.x , endPoint.y - beginPoint.y );
    if( unit.x > 0 ){
        curPoint.x += beginPoint.width-1;
        distance.x += endPoint.width - beginPoint.width;
    }
    if( unit.y > 0 ){
        curPoint.y += beginPoint.height-1;
        distance.y += endPoint.height - beginPoint.height;
    }
    TbBool isEndX = distance.x == 0;
    TbBool isEndY = distance.y == 0;
    TbFloat32 slopeX = correctMov.GetY()/correctMov.GetX();
    TbFloat32 slopeY = correctMov.GetX()/correctMov.GetY();

    while( !isEndX || !isEndY )
    {
        TbFloat32 stepTimeX = TB_FLOAT32_MAX;
        TbFloat32 stepTimeY = TB_FLOAT32_MAX;
        TbFloat32 nextX = 0.0f;
        TbFloat32 nextY = 0.0f;
        TbBool isHitting = TB_FALSE;
        if( !isEndX ) {
            nextX = static_cast<TbFloat32>((curPoint.x+unit.x)*chipSize);
            if( unit.x > 0 ){
                nextX -= (rect.x+rect.width);
            }else{
                nextX += chipSize - rect.x;
            }
            stepTimeX = (nextX - pos.GetX()) / correctMov.GetX();
        }
        if( !isEndY ) {
            nextY = static_cast<TbFloat32>((curPoint.y+unit.y)*chipSize);
            if( unit.y > 0 ){
                nextY -= (rect.y+rect.height);
            }else{
                nextY += chipSize - rect.y;
            }
            stepTimeY = (nextY - pos.GetY()) / correctMov.GetY();
        }
        if( stepTimeX < stepTimeY ) { // X軸方向に動かす
            TbPointS32 checkX( curPoint.x+unit.x , beginPoint.y + movPoint.y );
            isHitting = !IsMovableX( checkX , beginPoint.height );
            if( isHitting ){
                // 終了
                TbVector2 newMov( nextX-pos.GetX() , 0.0f );
                newMov.SetY( newMov.GetX() * slopeX );
                if( unit.x > 0 ){
                    newMov.Add(-ERROR_MOV,0.0f);
                }else {
                    newMov.Add(ERROR_MOV, 0.0f);
                }
                collision->SetPos( pos + newMov );
                result = unit.x > 0 ? CR_CONTACT_RIGHT : CR_CONTACT_LEFT;
                if( resultCorrectMov ) {
                    *resultCorrectMov = newMov;
                }
                TbBool is = IsContacted(collision, CR_CONTACT_UP);
                return CrUtil::GetContactFaceNormal(result);
            }else{
                curPoint.x += unit.x;
                movPoint.x += unit.x;
                isEndX = movPoint.x == distance.x;
            }
        }else{ // Y軸方向に動かす
            TbPointS32 checkY( beginPoint.x + movPoint.x , curPoint.y+unit.y );
            isHitting = !IsMovableY( checkY , beginPoint.width );
            if( isHitting ){
                // 終了
                TbVector2 newMov( 0.0f , nextY-pos.GetY() );
                newMov.SetX( newMov.GetY() * slopeY );
                if( unit.y > 0 ){
                    newMov.Add(0.0f,-ERROR_MOV);
                }else {
                    newMov.Add(0.0f, ERROR_MOV);
                }
                collision->SetPos( pos + newMov );
                result = unit.y > 0 ? CR_CONTACT_DOWN : CR_CONTACT_UP;
                if( resultCorrectMov ) {
                    *resultCorrectMov = newMov;
                }
                return CrUtil::GetContactFaceNormal(result);
            }else{
                curPoint.y += unit.y;
                movPoint.y += unit.y;
                isEndY = movPoint.y == distance.y;
            }
        }
    }
    // 衝突しなかったのでそのまま加算
    collision->SetPos( pos + correctMov );
    if( resultCorrectMov ) {
        *resultCorrectMov = correctMov;
    }

    TbVector2 resVec;
    if( CR_CONTACT_INVALID != result ) {
        resVec = CrUtil::GetContactFaceNormal(result);
    }
#if 0
    TB_ASSERT( !IsContacted(collision,CR_CONTACT_LEFT ) || !IsContacted(collision,CR_CONTACT_RIGHT ));
    TB_ASSERT( !IsContacted(collision,CR_CONTACT_DOWN ) || !IsContacted(collision,CR_CONTACT_UP ));
#endif
    return resVec;
}

/*!
 * どこかと接しているか
 * @author teco
 */
TbBool CrActionMap::IsContacted(const CrActionCollision* collision) const
{
    TbUint32 chipSize = GetChipSize();
    const TbRectF32& rect = collision->GetRect();
    TbVector2 pos = collision->GetPos();
    TbVector2 from(pos.GetX() + rect.x, pos.GetY() + rect.y);
    TbVector2 to(from.GetX() + rect.width, from.GetY() + rect.height);
    from /= chipSize;
    to /= chipSize;
    TbPointS32 fromP(static_cast<TbSint32>(from.GetX()), static_cast<TbSint32>(from.GetY()));
    TbPointS32 toP(static_cast<TbSint32>(to.GetX()), static_cast<TbSint32>(to.GetY()));
    return !IsMovable(fromP, toP);
}

/*!
 * X軸方向の移動不可能チップと接しているか
 * @author teco
 * @since 2013.09.16
 */
TbBool CrActionMap::IsContacted( const CrActionCollision* collision , const TbVector2& dir , TbPointS32* contactPoint ) const
{
    // 差分
    TbVector2 ofs = dir;
    ofs.Normalize();
    ofs *= 0.5f;
    TbFloat32 chipSize =GetChipSize();
    const TbRectF32& rect = collision->GetRect();
    TbVector2 pos = collision->GetPos();
    pos += ofs;
    TbVector2 from( pos.GetX()+rect.x , 
                    pos.GetY()+rect.y );
    TbVector2 to( from.GetX() + rect.width , from.GetY() + rect.height );
#if 1 // 全部は見ずに端っこだけを見る。(全部見ると見る込むことがある、アクター同士の衝突が原因で）
    if( ofs.GetY() < 0.0f ){
        to.SetY( from.GetY() );
    }else if ( ofs.GetY() > 0.0f ) {
        from.SetY( to.GetY() );
    }
    if( ofs.GetX() < 0.0f ){
        to.SetX( from.GetX() );
    }else if ( ofs.GetX() > 0.0f ) {
        from.SetX( to.GetX() );
    }
#endif
    from /= chipSize;
    to /= chipSize;
    TbPointS32 fromP( static_cast<TbSint32>(from.GetX()) , static_cast<TbSint32>(from.GetY()) );
    TbPointS32 toP(static_cast<TbSint32>(to.GetX()), static_cast<TbSint32>(to.GetY()));
    return !IsMovable(fromP,toP,contactPoint);
}

/*!
 * 背景描画
 * @author teco
 * @since 2013.10.24
 */
void CrActionMap::OnDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    if( !m_UvRect ){
        return;
    }
    if (!IsVisible()) {
        return;
    }
    TbUint32 height = GetTileHeight();
    TbUint32 width = GetTileWidth();
    TbPointS32 offset = GetOffset();
    TbFloat32 chipSize = static_cast<TbFloat32>(GetChipSize());
    for( TbUint32 y = 0; y < height; ++y ) {
        for( TbUint32 x = 0; x < width; ++x ) {
            TbUint32 chip = GetMapChip(x,y);
            if( chip >= CR_TILE_MAP_CHIP_BLOCK && !(GetMapChipAttr(x,y) & CR_TILE_MAP_CHIP_ATTR_FRONT) && !IsCulling(x, y))
            {
                TbRectF32 rect( static_cast<TbFloat32>((x)*chipSize) - GetScroll(), 
                                static_cast<TbFloat32>(y*chipSize) , 
                                static_cast<TbFloat32>(chipSize) , 
                                static_cast<TbFloat32>(chipSize) );
                material->Draw(rect,m_UvRect[chip],TbColor::White(),poseMatrix);
            }
        }
    }
}

/*!
 * 前景描画
 * @author teco
 * @since 2013.10.28
 */
void CrActionMap::OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    if( !m_UvRect ){
        return;
    }
    if (!IsVisible()) {
        return;
    }
    TbUint32 height = GetTileHeight();
    TbUint32 width = GetTileWidth();
    TbFloat32 chipSize = static_cast<TbFloat32>(GetChipSize());
    for( TbUint32 y = 0; y < height; ++y ) {
        for( TbUint32 x = 0; x < width; ++x ) {
            TbUint32 chip = GetMapChip(x,y);
            if( GetMapChipAttr(x,y) & CR_TILE_MAP_CHIP_ATTR_FRONT && !IsCulling(x, y))
            {
                TbRectF32 rect( static_cast<TbFloat32>(x*chipSize) - GetScroll(), 
                                static_cast<TbFloat32>(y*chipSize) , 
                                static_cast<TbFloat32>(chipSize) , 
                                static_cast<TbFloat32>(chipSize) );
                material->Draw(rect,m_UvRect[chip],TbColor::White(),poseMatrix);
            }
        }
    }    
}

/*!
* 指定マップチップは存在するか
* @author teco
*/
TbBool CrActionMap::Exists(TbUint32 beginChip, TbUint32 endChip) const
{
    TbUint32 h = GetTileHeight();
    TbUint32 w = GetTileWidth();
    for (TbUint32 x = 0; x < w; ++x) {
        for (TbUint32 y = 0; y < h; ++y) {
            TbUint32 chip = GetMapChip(x, y);
            if (CR_ACTION_MAP_CHIP_BREAK_BEGIN <= chip && chip < CR_ACTION_MAP_CHIP_BREAK_END) {
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}


}
