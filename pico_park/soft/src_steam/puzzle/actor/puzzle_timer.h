/*!
 * パズルタイマー
 * @author teco
 */

#ifndef _INCLUDED_PUZZLE_TIEMR_H_
#define _INCLUDED_PUZZLE_TIEMR_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/actor.h"
#include <graphics/sprite/tb_sprite_scene_hooker.h>

class PuzzleTimer : public Actor
{
public:

    enum Type
    {
        TYPE_UP       , // 時間増える
        TYPE_DOWN     , // 時間下がる
    };

public:
    
    typedef PuzzleTimer Self;
    typedef Actor Super;

public:

    // コンストラクタ
    PuzzleTimer( Type type = TYPE_UP );

    // デストラクタ
    virtual ~PuzzleTimer();

public:

    // タイムアップしたか
    TbBool IsTimeUp() const {
        return m_BitArray.Test(FLAG_TIMEUP);
    }


    // 時間設定
    void SetTime( TbSint32 sec ) {
        TB_RESQUE(sec>0);
        m_Sec = static_cast<TbFloat32>(sec);
        m_BitArray.ResetBit(FLAG_TIMEUP);
    }

public:

    // シーンに入った
    virtual void OnEnter( void* userData ) TB_OVERRIDE;
    
    // シーンから出た
    virtual void OnExit() TB_OVERRIDE;

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

private:

    enum Flag
    {
        FLAG_TIMEUP , 
        FLAG_MAX
    };

private:

    // 前景描画
    void onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // タイムアップ
    void onTimeUp();

private:

    Type                    m_Type;
    TbFloat32               m_Sec;
    TbBitArray32            m_BitArray;
    TbSprite                m_Sprite;
    TbSpriteSceneHooker     m_SpriteHooker;

};

#endif
