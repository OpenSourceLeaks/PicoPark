/*!
 * パズル用マップ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_DR_MAP_H_
#define _INCLUDED_PUZZLE_DR_MAP_H_

#include "puzzle_map.h"

class PuzzleDrMap : public PuzzleMap
{
    typedef TbStaticFunction32<void ()>         BlockErasedCallback;
    typedef PuzzleDrMap Self;
    typedef PuzzleMap Super;

public:

    // コンストラクタ
    PuzzleDrMap();

    // デストラクタ
    virtual ~PuzzleDrMap();

public: // ドクターマリオ用

    // ターゲット数取得
    TbUint32 GetTargetCount() const {
        return m_TargetCountDr;
    }

    // ブロック消滅時コールバック
    void SetBlockErasedCallback( const BlockErasedCallback& cb ) {
        m_BlockErasedCallbackDr = cb;
    }

    // 自由ブロック追加
    void AddFreeBlock( PuzzleBlockBase* block );

public:

    // 初期化
    virtual void OnInitialize( TbScriptLuaNode* node ) TB_OVERRIDE;

    // マップチップ更新
    virtual void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

    // 前景描画
    virtual void OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material ) TB_OVERRIDE;

private:

    typedef TbArray<PuzzleBlockBase*> SubBlockList;

private: // ドクターマリオ用

    // ターゲットセットアップ
    void setupTargetDr();

    // ターゲットを設置可能か
    TbBool isEnableSetTargetDr( TbSint32 x , TbSint32 y , TbUint32 mapchip );

    // 更新
    void updateDr();

private: // ドクターマリオ用

    TbUint32            m_TargetCountDr;
    BlockErasedCallback m_BlockErasedCallbackDr;   // ライン消滅持コールバック
    TbUint32            m_FreeBlockTotal;
    SubBlockList        m_FreeBlockList;

};

#endif
