/*!
 * アクション用コリジョンシーン
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_COLLISION_SCENE_H_
#define _INCLUDED_ACTION_COLLISION_SCENE_H_

#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_matrix.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_array.h>

class ActionCollision;
class ActionMap;

class ActionCollisionScene
{
    friend class ActionCollision;
public:
    
    // コンストラクタ
    ActionCollisionScene();

    // デストラクタ
    virtual ~ActionCollisionScene();

public:

    // マップ設定
    void SetMap( ActionMap* map ) {
        m_Map = map;
    }

    // マップ取得
    ActionMap* GetMap() {
        return m_Map;
    }

    // マップ取得
    const ActionMap* GetMap() const {
        return m_Map;
    }

    // 判定可能か
    TbBool IsEnableCheck( TbUint32 layer1 , TbUint32 layer2 ) const {
        if( layer1 < LAYER_MAX && layer2 < LAYER_MAX ){
            return m_LayerTable[layer1][layer2];
        }
        return TB_FALSE;
    }

    // 判定可能設定
    void SetEnableCheck( TbUint32 layer1 , TbUint32 layer2 , TbBool isEnable ) {
        if( layer1 < LAYER_MAX && layer2 < LAYER_MAX ){
            m_LayerTable[layer1][layer2] = isEnable;
            m_LayerTable[layer2][layer1] = isEnable;
        }
    }

    // 衝突しているか(外部から強制的に判定)
    TbBool IsCollidedByPushBackTarget( const ActionCollision& src , TbUint32 targetFilter = TB_UINT32_MAX ) const;

public:

    // 衝突更
    void Update();

public: // デバッグ

    // デバッグ描画
    void DrawDebug(const TbMatrix& poseMatrix);

protected:

    enum Result
    {
        RESULT_AGAIN , // 衝突判定によってコリジョンの位置が変化したのでやり直す必要がある
        RESULT_FIRST , 
        RESULT_OK , 
    };

    static const TbUint32 SEARCH_COLLISION_MAX = 512;
    typedef TbStaticArray<ActionCollision*,SEARCH_COLLISION_MAX> SearchCollisionArray;

protected:

    // 押し戻し後処理
    virtual void OnPostPushBack() {}

    // 先頭コリジョン取得
    ActionCollision* GetTopCollision() {
        return m_Top;
    }

    // 次のコリジョン取得
    ActionCollision* GetNextCollision( ActionCollision* col );

    // コリジョン同士の押し戻し更新
    Result UpdatePushBack( ActionCollision* target1 , ActionCollision* target2 , const TbVector2* extPre1 = NULL , const TbVector2* extPre2 = NULL );

private:

    static const TbUint32 LAYER_MAX = 32;

    // 座標一時保存用
    struct PosInfo
    {
        ActionCollision* co;
        TbVector2        pos;
    };

private:

    // マップとのコリジョンチェック
    void updateMap();

    // 押し戻しコリジョンの衝突判定を一回更新
    Result updatePushBack( TbBool isEnableCheckFixedPos );

    // 全コリジョンの衝突判定
    void updateCollide();

    // コリジョン同士の押し戻し更新
    Result updatePushBackRect( ActionCollision* target1 , ActionCollision* target2 , const TbVector2* extPre1 = NULL , const TbVector2* extPre2 = NULL );

    // 円形コリジョン同士の押し戻し更新
    Result updatePushBackCircle( ActionCollision* target1 , ActionCollision* target2 , const TbVector2* extPre1 = NULL , const TbVector2* extPre2 = NULL );

    // ポリゴンの押し戻し更新
    Result updatePushBackPolygon( ActionCollision* target1 , ActionCollision* target2 , const TbVector2* extPre1 = NULL , const TbVector2* extPre2 = NULL );

private:

    // コリジョン追加
    void addCollision( ActionCollision* collision );

    // コリジョン除去
    void removeCollision( ActionCollision* collision );

    // 強制移動
    TbBool moveCollisionForce( ActionCollision* src , const TbVector2& pos );

    // 衝突しているコリジョンを強制移動
    TbBool moveCollidedCollision( ActionCollision* src , const TbVector2& mov );

private: // デバッグ

    // 前段階で
    void checkCollided( TbBool isPre );

private:
    
    ActionCollision*            m_Top;
    ActionMap*                  m_Map;
    TbArray<PosInfo>            m_TempPosInfo;                      // 一時位置保存用情報
    TbBool                      m_LayerTable[LAYER_MAX][LAYER_MAX]; // レイヤー同士の関連性

};

#endif
