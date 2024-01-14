/*!
 * キーフレームアニメーションテンプレート
 * @author Miyake Shunsuke
 * @since 2012.01.01
 */
 
#ifndef _INCLUDED_TB_ANIMATION_KEY_FRAME_H_
#define _INCLUDED_TB_ANIMATION_KEY_FRAME_H_

#include <base/animation/tb_animation_types.h>
#include <base/animation/tb_animation_key_frame_data.h>
#include <base/math/tb_math_util.h>

namespace toybox
{

// キーフレー
template <typename T>
class TbAnimationKeyFrame
{
public:
    
    // コンストラクタ
    TbAnimationKeyFrame();

    // デストラクタ
    ~TbAnimationKeyFrame();

public:

    // 空かどうか
    TbBool IsEmpty() const {
        return !(m_Data);
    }
    
    // データ設定
    TbResult SetData( const TbAnimationKeyFrameData<T>* data );


    // キーフレームデータ取得
    const TbAnimationKeyFrameData<T>* GetData() const {
        return m_Data;
    }

public:
    
    // フレームステップ
    TbResult StepFrame( TbFloat32 frame );
    
    // フレーム強制設定
    TbResult SetFrame( TbFloat32 frame );

    // フレーム取得
    TbFloat32 GetFrame() const {
        return m_Frame;
    }
    
    // 開始フレーム設定(デフォルトは最初のキーフレーム)
    TbResult SetFrameBegin( TbFloat32 begin );
    
    // 開始フレーム取得
    TbFloat32 GetFrameBegin() const {
        return m_FrameBegin;
    }

    // 終了フレームを強制設定(デフォルトは最後のキーフレーム)
    TbResult SetFrameEnd( TbFloat32 end );

    // 終了フレーム取得
    TbFloat32 GetFrameEnd() const {
        return m_FrameEnd;
    }

    // ループ設定
    void SetEnableLoop( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_LOOP , isEnable );
    }
    
    // ループ設定取得
    TbBool IsEnableLoop() const {
        return m_BitArray.Test( FLAG_LOOP );
    }

    // 現在の値取得
    TbBool GetCurrentValue( T& value ) const;
    
    // キーフレーム数取得
    TbUint32 GetKeyFrameCount() const {
        return m_Data ? m_Data->GetKeyFrameCount() : 0;
    }
        
private:
    
    enum {
        FLAG_LOOP , 
    };
    
    static const TbSint32 KEY_FRAME_INDEX_INVALID = -1;
    
private:
    
    // フレーム強制設定
    TbResult setFrame( TbFloat32 frame , TbSint32 candidateKeyFrameBegin , TbSint32 candidateKeyFrameEnd );

private:

    TbBitArray32                        m_BitArray;                 // ビット配列
    const TbAnimationKeyFrameData<T>*   m_Data;                     // 参照データ
    T                                   m_CurrentValue;             // 現在値
    TbSint32                            m_LowerBoundKeyFrameIndex;  // 現在フレーム以上で最初のキーフレームインデックス
    TbFloat32                           m_FrameEnd;                 // 終了フレーム
    TbFloat32                           m_FrameBegin;               // 開始フレーム
    TbFloat32                           m_Frame;                    // フレーム

};

#include "./tb_animation_key_frame_inl.h"

typedef TbAnimationKeyFrame<TbFloat32> TbAnimationKeyFrameF32;

}

#endif
