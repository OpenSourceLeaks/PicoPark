/*!
 * タイルマップ
 * @author teco
 */

#include "tb_fwd.h"
#include "crayon/map/cr_tile_map_path_finding.h"
#include "crayon/map/cr_tile_map.h"

using namespace toybox;

namespace crayon
{

/*!
 * コンストラクタ
 * @author teco
 */
CrTileMapPathFinding::CrTileMapPathFinding()
    : m_Node(nullptr)
    , m_OpenList(0)
    , m_Map(nullptr)
{
}

/*!
 * デストラクタ
 * @author teco
 */
CrTileMapPathFinding::~CrTileMapPathFinding()
{
    TB_SAFE_DELETE_ARRAY(m_Node);
}

/*!
 * セットアップ
 * @author teco
 */
void CrTileMapPathFinding::Setup( CrTileMap* map )
{
    m_Map = map;
    TbSint32 size = m_Map->GetTileWidth()*m_Map->GetTileHeight();
    m_Node = new Node[size];
    m_OpenList.SetCapacity(size);
}

/*!
 * パス検索
 * @author teco
 */
TbBool CrTileMapPathFinding::Searcth( Result* dst , const TbPointS32& fromSrc , const TbPointS32& toSrc , TbSint32 searchLimit )
{
    if( !m_Map ) {
        return TB_FALSE;
    }

    const TbPointS32 from   = correctPos(fromSrc);
    const TbPointS32 to     = correctPos(toSrc);

    // 範囲外チェック
    TbSint32 width = m_Map->GetTileWidth();
    TbSint32 height = m_Map->GetTileHeight();
    if( from.x < 0 || width <= from.x ) {
        return TB_FALSE;
    }
    if( from.y < 0 || height <= from.y ) {
        return TB_FALSE;
    }
    if( to.x < 0 || width <= to.x ) {
        return TB_FALSE;
    }
    if( to.y < 0 || height <= to.y ) {
        return TB_FALSE;
    }

        // ノードリセット
    m_OpenList.Clear();
    TbUint32 size = width*height;
    for( size_t i = 0; i < size; ++i ) {
        m_Node[i].state = Node::STATE_NULL;
    }

    Node* goalNode = nullptr;

    pushOpenList(from.x,from.y,to,nullptr);
    TbSint32 openCount = 0;
    while( !m_OpenList.IsEmpty() ) {
        Node* node = m_OpenList.GetFront();
        m_OpenList.Erase(m_OpenList.Begin());
        node->state = Node::STATE_CLOSE;

        if( node->pos == to ) {
            // 目的場所到達
            goalNode = node;
            break;
        }

        if( searchLimit == openCount ) {
            // 打ち切り
            goalNode = node;
            break;
        }
        
        TbPointS32 pos = node->pos;
        pushOpenList(pos.x+1,pos.y,to,node);
        pushOpenList(pos.x-1,pos.y,to,node);
        pushOpenList(pos.x,pos.y+1,to,node);
        pushOpenList(pos.x,pos.y-1,to,node);
        ++openCount;
    }

    if( !goalNode ){
        return TB_FALSE;
    }

    dst->chipSize = m_Map->GetChipSize();
    dst->rootList.Clear();
    dst->rootList.SetCapacity(goalNode->cost+1);
    Node* pathNode = goalNode;
    while( pathNode ) {
        dst->rootList.PushBack(pathNode->pos);
        pathNode = pathNode->parent;
    }
    return TB_TRUE;
}

/*!
 * オープンリスト追加
 * @author teco
 */
TbBool CrTileMapPathFinding::pushOpenList( TbSint32 x , TbSint32 y , const TbPointS32& to , Node* parent )
{
    TbSint32 width = m_Map->GetTileWidth();
    TbSint32 height = m_Map->GetTileHeight();
    if( x < 0 || width <= x ) {
        return TB_FALSE;
    }
    if( y < 0 || height <= y ) {
        return TB_FALSE;
    }

    Node* node = &m_Node[(y*width)+x];
    if( node->state != Node::STATE_NULL ){
        // 既に何かしら処理済み
        return TB_FALSE;
    }

    if( !m_Map->IsMovable(x,y) ){
        // 登録不可
        node->state = Node::STATE_IGNORE;
        return TB_FALSE;
    }
    if( !m_Map->IsMovable(x+1,y) ){
        // 登録不可
        node->state = Node::STATE_IGNORE;
        return TB_FALSE;
    }
    if( !m_Map->IsMovable(x,y+1) ){
        // 登録不可
        node->state = Node::STATE_IGNORE;
        return TB_FALSE;
    }
    if( !m_Map->IsMovable(x+1,y+1) ){
        // 登録不可
        node->state = Node::STATE_IGNORE;
        return TB_FALSE;
    }
    node->state = Node::STATE_OPEN;
    node->cost = parent ? parent->cost+1 : 0;
    node->pos.x = x;
    node->pos.y = y;
    TbSint32 dx = to.x - x;
    TbSint32 dy = to.y - y;
    node->hCost = TbMax(TbAbs(dx),TbAbs(dy));
    node->score = node->cost + node->hCost;
    node->parent = parent;
    m_OpenList.Insert(node);

    return TB_TRUE;
}

/*!
 * 位置補正
 * @author teco
 */
TbPointS32 CrTileMapPathFinding::correctPos( const TbPointS32& src )
{
    TbPointS32 result = src;
    if( !m_Map->IsMovable(src.x+1,src.y+1) ){
        if( m_Map->IsMovable(src.x-1,src.y) ){
            result.x -= 1;
        }
        if( m_Map->IsMovable(src.x,src.y-1) ){
            result.y -= 1;
        }
    }else{
        if( !m_Map->IsMovable(src.x+1,src.y) ){
            result.x -= 1;
        }
        if( !m_Map->IsMovable(src.x,src.y+1) ){
            result.y -= 1;
        }
    }
    return result;
}

}
