/*!
 * キーフレームアニメーション用データ
 * @author Miyake Shunsuke
 * @since 2012.01.01
 */
 
#ifndef _INCLUDED_TB_ANIMATION_KEY_FRAME_DATA_H_
#define _INCLUDED_TB_ANIMATION_KEY_FRAME_DATA_H_

#include <base/algorithm/tb_search.h>
#include <base/animation/tb_animation_types.h>
#include <base/util/tb_adapter_iterator.h>
#include <base/math/tb_math_util.h>

namespace toybox
{

// キーフレーム
template <typename T>
class TbAnimationKeyFrameData
{
public:

    struct KeyFrame
    {
        TbFloat32 keyFrame;
        T         data;
    };
    
    struct ValueResult
    {
        T value;
        TbSint32 lowerBoundKeyFrameIndex;
    };
    
    struct Getter {
        TbFloat32 operator() ( const KeyFrame& data ) {
            return data.keyFrame;
        }
    };

public:
    
    // コンストラクタ
    TbAnimationKeyFrameData();

    // コンストラクタ
    TbAnimationKeyFrameData( const KeyFrame* keyFrames , TbUint32 keyFrameCount , TbAnimationInterpolateType interpolateType , TbBool canCopy );

    // コピーコンストラクタ
    TbAnimationKeyFrameData( const TbAnimationKeyFrameData& data ) {
        m_BitArray = data.m_BitArray;
        m_KeyFrameCount = data.m_KeyFrameCount;
        m_KeyFrame = data.m_KeyFrame;
    }

    // デストラクタ
    ~TbAnimationKeyFrameData();

public:

    // コピー演算子
    TbAnimationKeyFrameData<T>& operator=(const TbAnimationKeyFrameData<T>& data) {
        if( m_BitArray.Test( FLAG_COPY_DATA ) ){
            TB_SAFE_DELETE_ARRAY(m_KeyFrame);
            m_BitArray.ResetBit(FLAG_COPY_DATA);
        }
        m_BitArray = data.m_BitArray;
        m_KeyFrameCount = data.m_KeyFrameCount;
        m_InterpolateType = data.m_InterpolateType;
        if( m_BitArray.Test( FLAG_COPY_DATA ) ){
            KeyFrame* keyFrameDst = TB_NEW KeyFrame[m_KeyFrameCount];
            // miyake_todo:基本型の場合はメモリコピーにするように変更すべき
            for( TbUint32 i = 0; i < m_KeyFrameCount; ++i ){
                keyFrameDst[i] = data.m_KeyFrame[i];
            }
            m_KeyFrame = keyFrameDst;
        }else{
            m_KeyFrame = data.m_KeyFrame;
        }
        return *this;
    }

public:

    // プログラム上で初期化 ( data配列はframeが昇順に並んでいる必要がある )
    TbResult Initialize( const KeyFrame* keyFrames , TbUint32 keyFrameCount , TbAnimationInterpolateType type , TbBool canCopy );
    
    // 中身は空かどうか
    TbBool IsEmpty() const {
        return m_KeyFrameCount == 0;
    }

public:

    // 値を取得する
    TbResult GetValue( ValueResult& result , TbFloat32 frame , TbBool isEnableLoop ) const;

    // 値を取得する ( フレーム範囲を指定 )
    TbResult GetValue( ValueResult& result , TbFloat32 frame , TbBool isEnableLoop , TbFloat32 frameBegin , TbFloat32 frameEnd ) const;

    // 値を取得する ( 検索範囲指定 )
    TbResult GetValueWithSearchHint( ValueResult& result , 
                                     TbFloat32 frame , 
                                     TbBool isEnableLoop ,
                                     TbUint32 searchHintKeyFrameBegin , 
                                     TbUint32 searchHintKeyFrameEnd ) const;

    // 値を取得する ( フレーム範囲を指定 、検索範囲指定 )
    TbResult GetValueWithSearchHint( ValueResult& result , 
                                     TbFloat32 frame , 
                                     TbBool isEnableLoop , 
                                     TbFloat32 frameBegin , 
                                     TbFloat32 frameEnd ,
                                     TbUint32 searchHintKeyFrameBegin , 
                                     TbUint32 searchHintKeyFrameEnd ) const;

    // キーフレーム数取得
    TbUint32 GetKeyFrameCount() const {
        return m_KeyFrameCount;
    }
    
    // キーフレームデータ取得
    const KeyFrame* GetKeyFrame( TbUint32 index ) const;

    // 先頭キーフレームのフレーム取得
    TbFloat32 GetFrameBegin() const {
        if( !IsEmpty() ) {
            return GetKeyFrame(0)->keyFrame;
        }
        return 0.0f;
    }

    // 最終キーフレームのフレーム取得
    const TbFloat32 GetFrameEnd() const {
        if( !IsEmpty() ) {
            return GetKeyFrame(m_KeyFrameCount-1)->keyFrame;
        }
        return 0.0f;
    }    
private:

    enum {
        FLAG_COPY_DATA , 
    };

private:

    TbBitArray32                m_BitArray;         // ビット配列
    TbUint32                    m_KeyFrameCount;    // キーフレーム数
    const KeyFrame*             m_KeyFrame;         // キーフレーム
    TbAnimationInterpolateType  m_InterpolateType;  // 補間タイプ

};

#include "./tb_animation_key_frame_data_inl.h"

typedef TbAnimationKeyFrameData<TbFloat32> TbAnimationKeyFrameDataF32;

}

#endif
