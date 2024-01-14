/*!
 * パズルのブロック
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTOR_PUZZLE_BLOCK_TETRIS_H_
#define _INCLUDED_ACTOR_PUZZLE_BLOCK_TETRIS_H_

#include "puzzle/actor/puzzle_block_base.h"

class PuzzleBlockTetris : public PuzzleBlockBase
{
public:

    typedef PuzzleBlockBase Super;
    typedef PuzzleBlockTetris Self;

    enum Type
    {
        TYPE_DEFAULT , 
        TYPE_3 ,
    };

public:

    // コンストラクタ
    PuzzleBlockTetris( Type type );

    // デストラクタ
    virtual ~PuzzleBlockTetris();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:
    
    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

    // 床と衝突
    virtual void OnHitFloor();

    // マップ上のブロックの何かが削除された場合
    virtual void OnEraseMapBlock();

    // 終了更新
    virtual void OnUpdateEnd();

private:

    // ブロックタイプを乱数決定
    TbUint32 randBlockType() const;

    // 描画
    void draw(const TbMatrix& poseMatrix, TbSpriteMaterial* material );

    // ブロック更新
    void updateNewBlock();

private:
    
    TbUint32            m_CurrentBlockType;
    TbUint32            m_NextBlockType;        // 次のブロックタイプ
    TbSpriteSceneHooker m_SpriteHooker;         // スプライト処理のフックオブジェクト

    const BlockData*    m_BlockTypeTable;
    TbUint32            m_BlockTypeMax;

};

#endif
