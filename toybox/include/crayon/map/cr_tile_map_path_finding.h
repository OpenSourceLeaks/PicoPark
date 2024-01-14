/*!
 * タイルマップ用パスファインディング
 * A*アルゴリズムを利用して実装
 * @author teco
 */

#ifndef _INCLUDED_CR_TILE_MAP_PATH_FINDING_H_
#define _INCLUDED_CR_TILE_MAP_PATH_FINDING_H_

#include <base/container/tb_sorted_array.h>
#include <base/math/tb_geometry.h>

namespace crayon
{

class CrTileMap;

class CrTileMapPathFinding
{
public:

    struct Result
    {
        Result() : chipSize(1) {}
        TbSint32            chipSize;
        toybox::TbArray<toybox::TbPointS32> rootList;
    };

public:

    // コンストラクタ
    CrTileMapPathFinding();
    
    // デストラクタ
    ~CrTileMapPathFinding();

public:

    // セットアップ
    void Setup( CrTileMap* map );

    // パス検索
    TbBool Searcth( Result* dst , const toybox::TbPointS32& from , const toybox::TbPointS32& to , TbSint32 searchLimit = -1 );

private:

    struct Node
    {
        enum State
        {
            STATE_NULL ,
            STATE_IGNORE ,
            STATE_OPEN ,
            STATE_CLOSE ,
        };


        Node()
            : state(STATE_OPEN)
            , cost(0)
            , hCost(0)
            , score(0)
            , parent(nullptr)
        {}

        State       state;
        toybox::TbPointS32  pos;
        TbSint32    cost;   // コスト
        TbSint32    hCost; // ヒューリスティックコスト
        TbSint32    score;
        Node*       parent;
    };

    struct OpenListCompareFunc
    {
        // 比較演算子
        TbBool operator()( const Node* left , const Node* right ) const
        {
            if( left->score == right->score ) {
                return left->cost < right->cost;
            }
            return left->score < right->score;
        }
    }; 

private:

    // オープンリスト追加
    TbBool pushOpenList( TbSint32 x , TbSint32 y , const toybox::TbPointS32& to , Node* parent );

    // 位置補正
    toybox::TbPointS32 correctPos( const toybox::TbPointS32& src );

private:

    Node*                                       m_Node;
    toybox::TbSortedArray<Node*,OpenListCompareFunc>    m_OpenList;
    CrTileMap*                                    m_Map;

};

}

#endif
