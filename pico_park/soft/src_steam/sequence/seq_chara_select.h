/*!
 * キャラ選択シーケンス
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_CHARA_SELECT_H_
#define _INCLUDED_SEQ_CHARA_SELECT_H_

#include "seq_base.h"
#include <graphics/sprite/tb_sprite_all.h>
#include "side_action/stage/side_action_stage.h"

class SideActionStage;

class SeqCharaSelect : public SeqBase
{
public:
    typedef SeqCharaSelect Self;
    typedef SeqBase Super;

public:
    
    // コンストラクタ
    SeqCharaSelect();
    
    // デストラクタ
    virtual ~SeqCharaSelect();
    
public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate();

    // 描画関数
    virtual void OnDraw();
    
    // 終了処理(終了完了したらRESULT_FINISHを返す)
    virtual Result OnFinish() { return RESULT_FINISH; };

private:

    enum SpriteType
    {
        SPRITE_TYPE_ADVERTISE , // アドバタイズ
        SPRITE_TYPE_TITLE   , // タイトルロゴ
        SPRITE_TYPE_MAX 
    };

private:

    enum Phase
    {
        PHASE_BEGIN ,
        PHASE_ADVERTISE = PHASE_BEGIN ,
        PHASE_TITLE = 10 ,
    };

    struct Sprite
    {
        TbSprite           sprite;
        TbSpriteNodeTree   node;
    };

private:
    
    SideActionStage*    m_Stage;
    TbUint32            m_PlayerCount;
    TbSint32            m_PlayerPadIndex[CMN_PLAYER_MAX];
};

#endif
