/*!
 * パズル用マップ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_TETRIS_MAP_H_
#define _INCLUDED_PUZZLE_TETRIS_MAP_H_

#include "puzzle_map.h"

class PuzzleTetrisMap : public PuzzleMap
{
public:

    typedef TbStaticFunction32<void (TbUint32)> LineErasedCallback;
    typedef PuzzleTetrisMap Self;
    typedef PuzzleMap Super;

    struct FallTimeParam
    {
        TbUint32    border;
        float       time;
    };

    static const TbSizeT FALL_TIME_TABLE_MAX = 10;

public:

    // コンストラクタ
    PuzzleTetrisMap();

    // デストラクタ
    virtual ~PuzzleTetrisMap();

public: // テトリス用

    // ノルマ取得
    TbUint32 GetNormaLineCount() const {
        return m_NormaLineCount;
    }

    // ノルマ設定
    void SetNormaLineCount( TbUint32 norma ){
        m_NormaLineCount = norma;
    }

    // ライン数取得
    TbUint32 GetLineCount() const {
        return m_LineCount;
    }

    // カウントアップしたか
    TbBool IsLineCountedUp() const {
        return m_CountUpLineCount > 0;
    }

    // カウントアップしたライン数取得
    TbUint32 GetCountUpLineCount() const {
        return m_CountUpLineCount;
    }

    // ライン消滅時コールバック
    void SetLineErasedCallback( const LineErasedCallback& cb ) {
        m_LineErasedCallback = cb;
    }

public:

    // 初期化
    virtual void OnInitialize( TbScriptLuaNode* node ) TB_OVERRIDE;

    // マップチップ更新
    virtual void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

    // 前景描画
    virtual void OnDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material ) TB_OVERRIDE;

    // デバッグメニュー構築
    virtual void OnCreateDebugMenu( TbDebugMenu* menu ) TB_OVERRIDE;

private:// テトリス用

    // ライン設定
    void updateTetrisLine();

    // 特定の高さでラインが消せれるか
    TbBool isEnableEraseTetrisLine( TbSint32 y );

    // デバッグ用ブロック設定
    void setDebugBlock();

private: // テトリス用

    TbUint32            m_NormaLineCount;       // ノルマのライン数
    TbUint32            m_LineCount;            // 消したライン数
    TbUint32            m_CountUpLineCount;     // 増えたライン数
    LineErasedCallback  m_LineErasedCallback;   // ライン消滅持コールバック

    TbUint32            m_FallTimeCount;
    FallTimeParam       m_FallTimeTable[FALL_TIME_TABLE_MAX];

};

#endif
