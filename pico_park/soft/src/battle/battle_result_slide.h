/*!
* バトル結果ページ
* @author teco
*/

#ifndef _INCLUDED_BATTLE_RESULT_SLIDE_H_
#define _INCLUDED_BATTLE_RESULT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"
#include "battle/battle_manager.h"
#include "common/material/cmn_sprite_player_material.h"

class BattleResultPage : public CmnSlidePage
{
public:

    // コンストラクタ
    BattleResultPage();

    // デストラクタ
    virtual ~BattleResultPage() {}

public:

    // 定期処理
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // アクティブ状態が変化した
    void OnChangeActive(TbBool isActive) TB_OVERRIDE;

    // 描画処理
    void OnDraw(const TbVector2& offset) TB_OVERRIDE;

private:

    struct PlayerInfo
    {
        CmnSimpleSprite         playerSprite;
        CmnSimpleSprite         helmetSprite;
        CmnSimpleSprite         winSpirte[BattleManager::WIN_MAX];
    };

private:

    // プレイヤー位置取得
    TbVector2 getPlayerPos( TbUint32 playerIndex );

private:

    PlayerInfo              m_PlayerInfo[CMN_PLAYER_MAX];
    CmnSpritePlayerMaterial m_PlayerMaterial;
    TbFloat32               m_Sec;
    TbBool                  m_IsNewWin;

};

class BattleResultSlide : public CmnSlide
{
    typedef CmnSlide Super;
public:

    // コンストラクタs
    BattleResultSlide();

    // デストラクタ
    virtual ~BattleResultSlide();

public:

    // ステージID取得
    TbUint32 GetStageId() const;

protected:

    // 定期処理
    virtual void OnUpdate();

    // 描画
    virtual void OnDraw();

    // ページ変更
    virtual void OnChangingPage(TbBool isNext) TB_OVERRIDE;

    // 次のページ取得
    virtual CmnSlidePage* GetNextPage() TB_OVERRIDE;

private:
    BattleResultPage    m_BattleResultPage;
};

#endif