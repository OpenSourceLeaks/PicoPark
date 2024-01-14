/*!
 * アクターシーンクラス
 * @author Miyake Shunsuke
 * @since 2013.03.15
 */

#ifndef _INCLUDED_TB_ACTOR_SCENE_H_
#define _INCLUDED_TB_ACTOR_SCENE_H_

#include <base/actor/tb_actor.h>

namespace toybox
{

class TbActorScene
{
    friend class TbActor;
    typedef TbActor::Iterator Iterator;
public:

    // コンストラクタ
    TbActorScene( TbUint32 layerCount );

    // デストラクタ
    ~TbActorScene();

public:
    
    // 更新
    void Update( TbFloat32 deltatime );

    // 後処理
    void Post( TbFloat32 deltatime );
    
    // 描画
    void Draw( TbFloat32 deltatime );

public: // 

    // begin
    TbActor::Iterator Begin( TbUint32 layerIndex = 0 ) {
        if( layerIndex < m_LayerCount ){
            return Iterator(m_Layer[layerIndex].top);
        }
        return Iterator(nullptr);
    }

    // end
    TbActor::Iterator End( TbUint32 layerIndex = 0 ) {
        return Iterator(nullptr);
    }

public:

    // 強制的に登録しているアクターを全て削除
    void Clear() {
        removeActorAll();
    }

public:
    
    // 更新可能設定
    void SetEnableUpdate( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_DISABLE_UPDATE,!isEnable);
    }

    // 更新可能か
    TbBool IsEnableUpdate() const {
        return !m_BitArray.Test(FLAG_DISABLE_UPDATE);
    }
    
    // 描画可能設定
    void SetEnableDraw( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_DISABLE_DRAW,!isEnable);
    }

    // 描画可能か
    TbBool IsEnableDraw() const {
        return !m_BitArray.Test(FLAG_DISABLE_DRAW);
    }

private:
    
    enum 
    {
        FLAG_DISABLE_UPDATE , 
        FLAG_DISABLE_DRAW ,
        FLAG_LOCK_LAYER , 
        FLAG_MAX ,
    };

    struct Layer
    {
        Layer() : top(NULL) {}
        TbActor* top;
    };

private:

    // アクター登録
    void addActor( TbActor* actor , TbUint32 layerIndex );

    // アクター登録
    void addActor( TbActor* actor , Layer& layer );

    // アクターを除去
    void removeActor( TbActor* actor );

    // 全アクター削除
    void removeActorAll();

    // レイヤーの状態ロック
    void lockLayer() {
        m_BitArray.SetBit(FLAG_LOCK_LAYER);
    }
    
    // レイヤーの状態ロック
    void unlockLayer();

    // レイヤーの状態がロックされている
    TbBool isLockedLayer() const {
        return m_BitArray.Test(FLAG_LOCK_LAYER);
    }
    
private:

    TbBitArray32        m_BitArray;     // ビット配列
    TbUint32            m_LayerCount;   // レイヤー数
    Layer*              m_Layer;        // レイヤー
    Layer               m_EnterLayer;   // 入る用レイヤー

};

}

#endif
