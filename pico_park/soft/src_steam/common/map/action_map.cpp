/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "common/map/action_map.h"
#include "common/actor/actor.h"
#include "common/collision/action_collision.h"
#include <graphics/render/tb_draw_2d.h>

static const TbFloat32 ERROR_MOV = 0.0002f;

const TbUint32 MAP_CHIP_ATTR[] = 
{
    // 土管
    TILE_MAP_CHIP_ATTR_COLLISION | TILE_MAP_CHIP_ATTR_FRONT ,
    TILE_MAP_CHIP_ATTR_COLLISION | TILE_MAP_CHIP_ATTR_FRONT , 
    TILE_MAP_CHIP_ATTR_COLLISION | TILE_MAP_CHIP_ATTR_FRONT ,
    TILE_MAP_CHIP_ATTR_COLLISION | TILE_MAP_CHIP_ATTR_FRONT ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION ,
    TILE_MAP_CHIP_ATTR_COLLISION
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(MAP_CHIP_ATTR)==ACTION_MAP_CHIP_MAX);

// アクションパート用マップチップのUV矩形座標
static TB_NDEBUG_CONST TbRectF32 MAP_CHIP_UV_RECT[] = 
{
    TbRectF32() , // TILE_MAP_CHIP_INVALID , 
    TbRectF32() , // TILE_MAP_CHIP_NONE , 
    TbRectF32(CMN_TEX_UV_RECT(0,320,16,16)) , // TILE_MAP_CHIP_BLOCK ,
    TbRectF32(CMN_TEX_UV_RECT(16,528,16,16)) , // 床
    TbRectF32(CMN_TEX_UV_RECT(0,528,16,16)) , // 床+左壁
    TbRectF32(CMN_TEX_UV_RECT(32,528,16,16)) , // 床+右壁 
    TbRectF32(CMN_TEX_UV_RECT(16,560,16,16)) , // 天井
    TbRectF32(CMN_TEX_UV_RECT(0,560,16,16)) , // 天井+左壁
    TbRectF32(CMN_TEX_UV_RECT(32,560,16,16)) , // 天井+右壁
    TbRectF32(CMN_TEX_UV_RECT(0,544,16,16)) , // 左壁
    TbRectF32(CMN_TEX_UV_RECT(32,544,16,16)) , // 右壁
    TbRectF32(CMN_TEX_UV_RECT(16,544,16,16)) , // 内側
    TbRectF32(CMN_TEX_UV_RECT(0,464,16,16)) , // 内側(右壁と床に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(16,464,16,16)) , // 内側(左壁と床に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(0,480,16,16)) , // 内側(右壁と天井に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(16,480,16,16)) , // 内側(左壁と天井に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(32,464,16,16)) , // 内側(上)
    TbRectF32(CMN_TEX_UV_RECT(32,480,16,16)) , // 内側(下)
    TbRectF32(CMN_TEX_UV_RECT(48,464,16,16)) , // 内側(左)
    TbRectF32(CMN_TEX_UV_RECT(48,480,16,16)) , // 内側(右)
    TbRectF32(CMN_TEX_UV_RECT(64,528,16,16)) , // 長ブロック上
    TbRectF32(CMN_TEX_UV_RECT(64,544,16,16)) ,  // 長ブロック中
    TbRectF32(CMN_TEX_UV_RECT(64,560,16,16)) , // 長ブロック下
    TbRectF32(CMN_TEX_UV_RECT(0,592,16,16)) , // 長ブロック左
    TbRectF32(CMN_TEX_UV_RECT(16,592,16,16)) ,  // 長ブロック中
    TbRectF32(CMN_TEX_UV_RECT(32,592,16,16)) , // 長ブロック右
    TbRectF32(CMN_TEX_UV_RECT(48,368,16,16)) ,  // 土管左上
    TbRectF32(CMN_TEX_UV_RECT(48,384,16,16)) ,  // 土管左下
    TbRectF32(CMN_TEX_UV_RECT(64,368,16,16)) , // 土管右上
    TbRectF32(CMN_TEX_UV_RECT(64,384,16,16)) , // 土管右下
    TbRectF32(CMN_TEX_UV_RECT(0,352,12,12)) , // ブロックB
    TbRectF32(CMN_TEX_UV_RECT(16,352,12,12)) , // ブロックC
    TbRectF32(CMN_TEX_UV_RECT(32,352,12,12)) , // ブロックD
    TbRectF32(CMN_TEX_UV_RECT(48,352,12,12)) , // ブロックE
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.11
 */
ActionMap::ActionMap()
    : Super()
{
    SetUserMapChipAttrTable(MAP_CHIP_ATTR,TB_ARRAY_COUNT_OF(MAP_CHIP_ATTR));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.11
 */
ActionMap::~ActionMap()
{
}

/*!
 * アクターを移動する
 * @author teco
 * @since 2013.09.11
 */
TbVector2 ActionMap::Move( ActionCollision* collision , const TbVector2& mov , TbVector2* resultCorrectMov )
{
    if( mov.Length() < TB_FLOAT32_EPSILON || !collision ) {
        return TbVector2();
    }

    TB_ASSERT(!mov.HasNan());

    CmnContactFace result = CMN_CONTACT_INVALID;
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
        CmnContactFace contact = unit.x > 0 ? CMN_CONTACT_RIGHT : CMN_CONTACT_LEFT;
        if( IsContacted(collision, CmnUtil::GetContactFaceNormal(contact) ) ){
            correctMov.SetX(0.0f);
            unit.x = 0;
            result = contact;
        }
    }
    if( unit.y != 0 ){
        CmnContactFace contact = unit.y > 0 ? CMN_CONTACT_DOWN : CMN_CONTACT_UP;
        if( IsContacted(collision,CmnUtil::GetContactFaceNormal(contact) ) ){
            correctMov.SetY(0.0f);
            unit.y = 0;
            result = contact;
        }
    }

    TbVector2 beginPos( pos.GetX()+rect.x , pos.GetY()+rect.y );
    TbRectS32 beginPoint( static_cast<TbSint32>(beginPos.GetX()) ,
                          static_cast<TbSint32>(beginPos.GetY()) ,
                          static_cast<TbSint32>(beginPos.GetX()+rect.width) ,
                          static_cast<TbSint32>(beginPos.GetY()+rect.height) );
    TbRectS32 endPoint( static_cast<TbSint32>(beginPos.GetX()+correctMov.GetX()) ,
                        static_cast<TbSint32>(beginPos.GetY()+correctMov.GetY()) ,
                        static_cast<TbSint32>(beginPos.GetX()+correctMov.GetX()+rect.width) ,
                        static_cast<TbSint32>(beginPos.GetY()+correctMov.GetY()+rect.height) );

    beginPoint /= chipSize;
    beginPoint.width = beginPoint.width-beginPoint.x+1;
    beginPoint.height = beginPoint.height-beginPoint.y+1;
    endPoint /= chipSize;
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
                }
                collision->SetPos( pos + newMov );
                result = unit.x > 0 ? CMN_CONTACT_RIGHT : CMN_CONTACT_LEFT;
                if( resultCorrectMov ) {
                    *resultCorrectMov = newMov;
                }
                return CmnUtil::GetContactFaceNormal(result);
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
                }
                collision->SetPos( pos + newMov );
                result = unit.y > 0 ? CMN_CONTACT_DOWN : CMN_CONTACT_UP;
                if( resultCorrectMov ) {
                    *resultCorrectMov = newMov;
                }
                return CmnUtil::GetContactFaceNormal(result);
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
    if( CMN_CONTACT_INVALID != result ) {
        resVec = CmnUtil::GetContactFaceNormal(result);
    }
    TB_ASSERT( !IsContacted(collision,CMN_CONTACT_LEFT ) || !IsContacted(collision,CMN_CONTACT_RIGHT ));
    TB_ASSERT( !IsContacted(collision,CMN_CONTACT_DOWN ) || !IsContacted(collision,CMN_CONTACT_UP ));
    return resVec;
}

// どこかが接触している
TbBool ActionMap::IsContacted(const ActionCollision* collision) const
{
    TbUint32 chipSize = GetChipSize();
    const TbRectF32& rect = collision->GetRect();
    TbVector2 pos = collision->GetPos();
    TbPointS32 from(static_cast<TbSint32>(pos.GetX() + rect.x),
                    static_cast<TbSint32>(pos.GetY() + rect.y));
    TbPointS32 to(from.x + static_cast<TbSint32>(rect.width), from.y + static_cast<TbSint32>(rect.height));
    from /= chipSize;
    to /= chipSize;
    return !IsMovable(from, to);
}

/*!
 * 移動不可能チップと接しているか
 * @author teco
 * @since 2013.09.16
 */
TbBool ActionMap::IsContacted( const ActionCollision* collision , const TbVector2& dir , TbPointS32* contactPoint ) const
{
    // 差分
    TbVector2 ofs = dir;
    ofs.Normalize();
    ofs *= 0.5f;
    TbUint32 chipSize =GetChipSize();
    const TbRectF32& rect = collision->GetRect();
    TbVector2 pos = collision->GetPos();
    pos += ofs;
    TbPointS32 from( static_cast<TbSint32>(pos.GetX()+rect.x) , 
                     static_cast<TbSint32>(pos.GetY()+rect.y) );
    TbPointS32 to( from.x + static_cast<TbSint32>(rect.width) , from.y + static_cast<TbSint32>(rect.height) );
#if 1 // 全部は見ずに端っこだけを見る。(全部見ると見る込むことがある、アクター同士の衝突が原因で）
    if( ofs.GetY() < 0.0f ){
        to.y = from.y;
    }else if ( ofs.GetY() > 0.0f ) {
        from.y = to.y;
    }
    if( ofs.GetX() < 0.0f ){
        to.x = from.x;
    }else if ( ofs.GetX() > 0.0f ) {
        from.x = to.x;
    }
#endif
    from /= chipSize;
    to /= chipSize;
    return !IsMovable(from,to,contactPoint);
}

/*!
 * 背景描画
 * @author teco
 * @since 2013.10.24
 */
void ActionMap::OnDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbUint32 height = GetTileHeight();
    TbUint32 width = GetTileWidth();
    TbFloat32 chipSize = static_cast<TbFloat32>(GetChipSize());
    for( TbUint32 y = 0; y < height; ++y ) {
        for( TbUint32 x = 0; x < width; ++x ) {
            TbUint32 chip = GetMapChip(x,y);
            if( chip >= TILE_MAP_CHIP_BLOCK && !(GetMapChipAttr(x,y) & TILE_MAP_CHIP_ATTR_FRONT) )
            {
                TbRectF32 rect( static_cast<TbFloat32>(x*chipSize) - GetScroll(), 
                                static_cast<TbFloat32>(y*chipSize) , 
                                static_cast<TbFloat32>(chipSize) , 
                                static_cast<TbFloat32>(chipSize) );
                material->Draw(rect,MAP_CHIP_UV_RECT[chip],TbColor::White(),poseMatrix);
            }
        }
    }
}

/*!
 * 前景描画
 * @author teco
 */
void ActionMap::OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbUint32 height = GetTileHeight();
    TbUint32 width = GetTileWidth();
    TbFloat32 chipSize = static_cast<TbFloat32>(GetChipSize());
    for( TbUint32 y = 0; y < height; ++y ) {
        for( TbUint32 x = 0; x < width; ++x ) {
            TbUint32 chip = GetMapChip(x,y);
            if( GetMapChipAttr(x,y) & TILE_MAP_CHIP_ATTR_FRONT )
            {
                TbRectF32 rect( static_cast<TbFloat32>(x*chipSize) - GetScroll(), 
                                static_cast<TbFloat32>(y*chipSize) , 
                                static_cast<TbFloat32>(chipSize) , 
                                static_cast<TbFloat32>(chipSize) );
                material->Draw(rect,MAP_CHIP_UV_RECT[chip],TbColor::White(),poseMatrix);
            }
        }
    }    
}

/*!
 * 指定マップチップは存在するか
 * @author teco
 */
TbBool ActionMap::Exists( TbUint32 beginChip , TbUint32 endChip ) const
{
    TbUint32 h = GetTileHeight();
    TbUint32 w = GetTileWidth();
    for( TbUint32 x = 0; x < w; ++x ){
        for( TbUint32 y = 0; y < h ; ++y ) {
            TbUint32 chip = GetMapChip(x,y);
            if ( ACTION_MAP_CHIP_BREAK_BEGIN <= chip && chip < ACTION_MAP_CHIP_BREAK_END ){
                return TB_TRUE;
            }
        }
    }
    return TB_FALSE;
}

