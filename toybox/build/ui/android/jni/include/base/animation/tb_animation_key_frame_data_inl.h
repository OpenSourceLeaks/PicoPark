/*!
 * キーフレームアニメーションテンプレート
 * @author Miyake Shunsuke
 * @since 2012.01.01
 */

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.01.03
 */
template <typename T>
TbAnimationKeyFrameData<T>::TbAnimationKeyFrameData()
    : m_BitArray()
    , m_KeyFrameCount(0)
    , m_KeyFrame( NULL )
    , m_InterpolateType( TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT )
{
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.01.03
 */
template <typename T>
TbAnimationKeyFrameData<T>::TbAnimationKeyFrameData( const KeyFrame* keyFrames , TbUint32 keyFrameCount , TbAnimationInterpolateType interpolateType , TbBool canCopy )
    : m_BitArray()
    , m_KeyFrameCount(0)
    , m_KeyFrame( NULL )
    , m_InterpolateType( TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT )
{
    Initialize( keyFrames , keyFrameCount , interpolateType , canCopy );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.01.03
 */
template <typename T>
TbAnimationKeyFrameData<T>::~TbAnimationKeyFrameData()
{
    if( m_BitArray.Test( FLAG_COPY_DATA ) ){
        TB_SAFE_DELETE_ARRAY(m_KeyFrame);
    }
}

/*!
 * 初期化 ( data配列はframeが昇順に並んでいる必要がある )
 * @author Miyake Shunsuke
 * @since 2012.01.03
 */
template <typename T>
TbResult TbAnimationKeyFrameData<T>::Initialize( const KeyFrame* keyFrame , TbUint32 keyFrameCount , TbAnimationInterpolateType interpolateType ,  TbBool canCopyData )
{
    if( m_KeyFrameCount > 0 || 0 == keyFrameCount ){
        return TB_E_FAIL;
    }
    m_BitArray.SetBit(FLAG_COPY_DATA,canCopyData);
    m_InterpolateType = interpolateType;
    m_KeyFrameCount = keyFrameCount;
    if( m_BitArray.Test( FLAG_COPY_DATA ) ){
        KeyFrame* keyFrameDst = TB_NEW KeyFrame[keyFrameCount];
        // miyake_todo:基本型の場合はメモリコピーにするように変更すべき
        for( TbUint32 i = 0; i < keyFrameCount; ++i ){
            keyFrameDst[i] = keyFrame[i];
        }
        m_KeyFrame = keyFrameDst;
    }else{
        m_KeyFrame = keyFrame;
    }
    return TB_S_OK;
}

/*!
 * 値を取得する
 * @author Miyake Shunsuke
 * @since 2012.01.16
 */
template <typename T>
TbResult TbAnimationKeyFrameData<T>::GetValue( ValueResult& result , TbFloat32 frame , TbBool isEnableLoop ) const
{
    // キーフレームが設定されていない場合は失敗
    if( 0 == m_KeyFrameCount ){
        return TB_E_FAIL;
    }
    return GetValueWithSearchHint( result , 
                                   frame , 
                                   isEnableLoop , 
                                   m_KeyFrame[0].keyFrame , 
                                   m_KeyFrame[m_KeyFrameCount-1].keyFrame , 
                                   0 , 
                                   m_KeyFrameCount );
}

/*!
 * 値を取得する ( フレーム範囲を指定 )
 * @author Miyake Shunsuke
 * @since 2012.01.16
 */
template <typename T>
TbResult TbAnimationKeyFrameData<T>::GetValue( ValueResult& result , TbFloat32 frame , TbBool isEnableLoop , TbFloat32 frameBegin , TbFloat32 frameEnd ) const
{
    // キーフレームが設定されていない場合は失敗
    if( 0 == m_KeyFrameCount ){
        return TB_E_FAIL;
    }
    return GetValueWithSearchHint( result , 
                                   frame , 
                                   isEnableLoop , 
                                   frameBegin , 
                                   frameEnd , 
                                   0 , 
                                   m_KeyFrameCount );
}

/*!
 * 値を取得する ( 検索範囲指定 )
 * @author Miyake Shunsuke
 * @since 2012.01.16
 */
template <typename T>
TbResult TbAnimationKeyFrameData<T>::GetValueWithSearchHint( ValueResult& result , TbFloat32 frame , TbBool isEnableLoop ,
                                                             TbUint32 searchHintKeyFrameBegin , TbUint32 searchHintKeyFrameEnd ) const
{
    // キーフレームが設定されていない場合は失敗
    if( 0 == m_KeyFrameCount ){
        return TB_E_FAIL;
    }
    return GetValueWithSearchHint( result , 
                                   frame , 
                                   isEnableLoop , 
                                   m_KeyFrame[0].keyFrame , 
                                   m_KeyFrame[m_KeyFrameCount-1].keyFrame , 
                                   searchHintKeyFrameBegin , 
                                   searchHintKeyFrameEnd );
}

/*!
 * 値を取得する ( フレーム範囲を指定 、検索範囲指定 )
 * @param frame フレーム
 * @param type 補間タイプ
 * @param isEnableLoop ループ設定
 * @param frameBegin 開始フレーム
 * @param frameEnd 終了フレーム
 * @param searchHintKeyFrameBegin キーフレーム配列の検索範囲
 * @param searchHintKeyFrameEnd キーフレーム配列の検索範囲
 * @author Miyake Shunsuke
 * @since 2012.01.05
 */
template <typename T>
TbResult TbAnimationKeyFrameData<T>::GetValueWithSearchHint( ValueResult& result , TbFloat32 frame , TbBool isEnableLoop , TbFloat32 frameBegin , TbFloat32 frameEnd ,
                                                             TbUint32 searchHintKeyFrameBegin , TbUint32 searchHintKeyFrameEnd ) const
{    
    // キーフレームが設定されていない場合は失敗
    if( 0 == m_KeyFrameCount ){
        return TB_E_FAIL;
    }
    
    // 開始フレーム
    if( frame <  frameBegin ){
        return TB_E_FAIL;
    }
    
    // 最終フレームに到達したか
    if( frame > frameEnd ){
        frame = isEnableLoop ? frameBegin : frameEnd;
    }

    Getter getter;
    result.lowerBoundKeyFrameIndex = TbLowerBoundIndex(  TbGetAdapterIterator<TbFloat32>(&m_KeyFrame[searchHintKeyFrameBegin],getter) , 
                                                         TbGetAdapterIterator<TbFloat32>(&m_KeyFrame[searchHintKeyFrameEnd],getter) , 
                                                         frame );

    // 値更新
    if( result.lowerBoundKeyFrameIndex >= 0 ){
        switch( m_InterpolateType )
        {
        case TB_ANIMATION_INTERPOLATE_TYPE_CONSTANT:
            // Constant
            if( result.lowerBoundKeyFrameIndex == 0 ){
                result.value = m_KeyFrame[result.lowerBoundKeyFrameIndex].data;
            }else if( TB_FLOAT32_EQUAL(m_KeyFrame[result.lowerBoundKeyFrameIndex].keyFrame,frame) ){
                result.value = m_KeyFrame[result.lowerBoundKeyFrameIndex].data;
            }else{
                result.value = m_KeyFrame[result.lowerBoundKeyFrameIndex-1].data;
            }
            break;
        case TB_ANIMATION_INTERPOLATE_TYPE_LINEAR:
            // 線形補間
            if( result.lowerBoundKeyFrameIndex == 0 ){
                result.value = m_KeyFrame[result.lowerBoundKeyFrameIndex].data;
            }else{
                TbFloat32 prevFrame = m_KeyFrame[result.lowerBoundKeyFrameIndex-1].keyFrame;
                TbFloat32 t = (( frame - prevFrame ) / ( m_KeyFrame[result.lowerBoundKeyFrameIndex].keyFrame - prevFrame )) ;
                result.value = TbLerp(m_KeyFrame[result.lowerBoundKeyFrameIndex-1].data,
                                      m_KeyFrame[result.lowerBoundKeyFrameIndex].data,
                                      t);
            }
            break;
        default:
            TB_ASSERT(!"unsupported");
            break;
        }
    }
    
    return TB_S_OK;
}

/*!
 * キーフレーム取得
 * @author Miyake Shunsuke
 * @since 2012.01.05
 */
template <typename T>
const typename TbAnimationKeyFrameData<T>::KeyFrame* TbAnimationKeyFrameData<T>::GetKeyFrame( TbUint32 index ) const
{
    if( index >= m_KeyFrameCount ){
        return NULL;
    }
    return &m_KeyFrame[index];
}
