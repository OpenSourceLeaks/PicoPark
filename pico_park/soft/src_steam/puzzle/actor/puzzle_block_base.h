/*!
 * パズルのブロック既定
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTOR_PUZZLE_BLOCK_BASE_H_
#define _INCLUDED_ACTOR_PUZZLE_BLOCK_BASE_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/actor.h"
#include <graphics/sprite/tb_sprite_scene_hooker.h>

class PuzzleMap;

class PuzzleBlockBase : public Actor
{
public:

    typedef Actor Super;
    typedef PuzzleBlockBase Self;

    static const TbUint32 BLOCK_COUNT_MAX = 4;
    static const TbUint32 BLOCK_COLOR_MAX = CMN_PLAYER_MAX;

    enum RotateType
    {
        ROTATE_NONE , 
        ROTATE_L ,
        ROTATE_R
    };

    struct BlockData
    {
        TbBool     isEnableRot;
        TbBool     isEnableHigh;
        TbUint32   count;
        TbPointS32 point[BLOCK_COUNT_MAX];
    };

public:

    // コンストラクタ
    PuzzleBlockBase( TbSint32 playerIndex );

    // デストラクタ
    virtual ~PuzzleBlockBase();

public:

    // プレイヤー番号
    void SetPlayerIndex( TbSint32 playerIndex ) {
        m_PlayerIndex = playerIndex;
    }

    // プレイヤー番号
    TbSint32 GetPlayerIndex() const {
        return m_PlayerIndex;
    }

    // パッド番号取得
    TbUint32 GetPadIndex() const {
        return m_PadIndex;
    }

    // マップ取得
    PuzzleMap* GetMap() const {
        return m_Map;
    }

    // 位置取得
    const TbPointS32& GetPoint() const {
        return m_Point;
    }

    // マップ設定
    void SetMap( PuzzleMap* map ) {
        m_Map = map;
    }

    // ポイント設定
    void SetPoint( TbSint32 x , TbSint32 y , TbBool isEnableMapChip = TB_TRUE );

    // ポイント設定
    void SetPoint( const TbPointS32& pos ) {
        SetPoint( pos.x , pos.y );
    }

    // スタートポイントに戻る
    bool ReturnStartPoint();

    // スタートポイント設定
    void SetStartPoint( TbSint32 x , TbSint32 y ) {
        m_StartPoint.x = x;
        m_StartPoint.y = y;
        SetPoint( x , y );
    }

    // 落下速度設定
    void SetFallTimeScale( TbFloat32 scale ){
        m_FallTimeScale = scale;
    }

    // 終了
    TbBool IsEnd() const {
        return m_BitArray.Test(FLAG_END);
    }

    // アクティブ
    TbBool IsActive() const {
        return m_BitArray.Test(FLAG_ACTIVE);
    }

    // コントロール可能か
    void SetEnableControl(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_CONTROL,isEnable);
    }

    // コントロール可能か
    TbBool IsEnableControl() const {
        return m_BitArray.Test(FLAG_ENABLE_CONTROL);
    }

    // 移動可能か
    void SetEnableMove( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_MOVE,isEnable);
    }

    // 移動可能か
    TbBool IsEnableMove() const {
        return m_BitArray.Test(FLAG_ENABLE_MOVE);
    }

    // 現在の基準マップチップ
    TbUint32 GetBaseMapChip() const {
        return m_BaseMapChip;
    }

public:

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime );

    // 終了更新
    virtual void OnUpdateEnd() {}
    
    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

    // マップ上のブロックの何かが削除された場合
    virtual void OnEraseMapBlock() {}

    // 床と衝突した
    virtual void OnHitFloor() = 0;

protected:

    // 終了フラグ強制
    void SetEnd( TbBool isEnd ) {
        m_BitArray.SetBit(FLAG_END,isEnd);
    }

    // アクティブ設定
    void SetActive( TbBool isActive ){
        m_BitArray.SetBit(FLAG_ACTIVE,isActive);
    }

    // ブロック設定
    void SetBlock(const BlockData& block) {
        SetBlock(block.point, block.count, block.isEnableRot);
    }

    // ブロック設定
    void SetBlock(const TbPointS32* blockPoint, TbUint32 blockCount, TbBool isEnableRot);

    // ブロック設定
    void SetBlock( const PuzzleBlockBase& src );

    // ブロック数取得
    TbUint32 GetBlockCount() const{
        return m_BlockCount;
    }

    // ブロック削除
    void EraseBlock( TbUint32 blockIndex );

    // ブロック位置取得
    TbPointS32 GetBlockPos( TbUint32 blockIndex ) const {
        if( blockIndex < m_BlockCount ){
            return m_Block[blockIndex].pos;
        }
        return TbPointS32();
    }

    // ブロックカラー設定
    void SetBlockColor(TbUint32 blockIndex, TbUint32 colorIndex);

    // ブロックカラー設定
    void SetBlockColor(TbUint32 colorIndex);

    // ブロックカラー取得
    TbUint32 GetBlockColor( TbUint32 blockIndex ) {
        if( blockIndex < m_BlockCount ){
            return m_Block[blockIndex].color;
        }
        return 0;
    }

    // マップチップ更新
    void SetMapChip( TbUint32 chip );

    // 移動可能か
    TbBool IsMovable( const TbPointS32& mov ) const {
        TbPointS32 pos = m_Point + mov;
        return  isMovable(pos,m_Block,m_BlockCount);
    }

private:

    enum Flag
    {
        FLAG_ENABLE_MOVE , 
        FLAG_ENABLE_ROT , 
        FLAG_ENABLE_CONTROL ,
        FLAG_UPDATE_MAP_CHIP ,
        FLAG_END , // 死亡
        FLAG_ROT , // 回転した
        FLAG_ACTIVE , // アクティブ
    };

    struct BlockInfo
    {
        BlockInfo() : pos(), color(0){}
        TbPointS32 pos;
        TbUint32   color;
    };

private:

    // 移動
    void move( TbSint32 x , TbSint32 y , RotateType rot );

    // 移動可能か
    TbBool isMovable( const TbPointS32& point , const BlockInfo* block , TbUint32 blockCount , TbBool* isHitMap = NULL ) const;

private:

    TbSint32            m_PlayerIndex;              // プレイヤー番号
    TbUint32            m_PadIndex;                 // パッド番号
    TbBitArray32        m_BitArray;                 // ビット配列
    TbFloat32           m_FallTimer;                // 落下タイマー
    TbFloat32           m_FallTimeScale;
    PuzzleMap*          m_Map;                      // マップ
    TbUint32            m_BaseMapChip;

private:

    TbPointS32          m_OldPoint;                 // 1フレーム前の位置
    TbPointS32          m_Point;                    // 位置
    TbPointS32          m_StartPoint;               // スタート位置

    TbUint32            m_BlockCount;              // 
    BlockInfo           m_Block[BLOCK_COUNT_MAX];  // 

private:

};

#endif
