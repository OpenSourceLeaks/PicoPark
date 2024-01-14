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
TbAnimationKeyFrame<T>::TbAnimationKeyFrame()
    : m_BitArray()
    , m_Data( NULL )
    , m_CurrentValue()
    , m_LowerBoundKeyFrameIndex(KEY_FRAME_INDEX_INVALID)
    , m_FrameBegin(0.0f)
    , m_FrameEnd(0.0f)
    , m_Frame(0.0f)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.01.03
 */
template <typename T>
TbAnimationKeyFrame<T>::~TbAnimationKeyFrame()
{
}

/*!
 * データ設定
 * @author Miyake Shunsuke
 * @since 2012.01.16
 */
template <typename T>
TbResult TbAnimationKeyFrame<T>::SetData( const TbAnimationKeyFrameData<T>* data )
{
    if( !data || data->IsEmpty() ){
        return TB_E_FAIL;
    }
    m_Data = data;
    m_FrameBegin = m_Data->GetFrameBegin();
    m_FrameEnd = m_Data->GetFrameEnd();
    SetFrame( m_FrameBegin );
    return TB_S_OK;
}

/*!
 * フレームステップ
 * @author Miyake Shunsuke
 * @since 2012.01.04
 */
template <typename T>
TbResult TbAnimationKeyFrame<T>::StepFrame( TbFloat32 frame )
{
    if( !m_Data ){
        return TB_E_FAIL;
    }
    TbSint32 candidateKeyFrameBegin = 0;
    TbSint32 candidateKeyFrameEnd = 0;
    if( frame > 0.0f ){ // 再生
        candidateKeyFrameBegin = TbMax<TbSint32>( m_LowerBoundKeyFrameIndex , 0 );
        candidateKeyFrameEnd = GetKeyFrameCount();
    }else{ // 逆再生
        candidateKeyFrameBegin = 0;
        candidateKeyFrameEnd = TbMax<TbSint32>( m_LowerBoundKeyFrameIndex+1 , 0 );
    }
    setFrame( m_Frame + frame , candidateKeyFrameBegin , candidateKeyFrameEnd );
    return TB_S_OK;
}

/*!
 * フレーム強制設定
 * @author Miyake Shunsuke
 * @since 2012.01.04
 */
template <typename T>
TbResult TbAnimationKeyFrame<T>::SetFrame( TbFloat32 frame )
{
    if( !m_Data ){
        return TB_E_FAIL;
    }
    // miyake_todo フレームに変化がない場合は更新しないような対応
    setFrame( frame , 0 , GetKeyFrameCount() );
    return TB_S_OK;
}

/*!
 * フレーム強制設定
 * @param frame 新しいフレーム
 * @param candidateKeyFrameBegin m_LowerBoundKeyFrameIndexの候補(最小)
 * @param candidateKeyFrameEnd m_LowerBoundKeyFrameIndexの候補(最大)
 * @author Miyake Shunsuke
 * @since 2012.01.05
 */
template <typename T>
TbResult TbAnimationKeyFrame<T>::setFrame( TbFloat32 frame , TbSint32 candidateKeyFrameBegin , TbSint32 candidateKeyFrameEnd )
{
    if( !m_Data ){
        return TB_E_FAIL;
    }
    typename TbAnimationKeyFrameData<T>::ValueResult result;
    TbResult retVal = m_Data->GetValueWithSearchHint( result , 
                                                      frame , 
                                                      m_BitArray.Test( FLAG_LOOP ) ,
                                                      m_FrameBegin , 
                                                      m_FrameEnd , 
                                                      candidateKeyFrameBegin , 
                                                      candidateKeyFrameEnd );
    if( TB_SUCCEEDED( retVal ) ){
        m_CurrentValue = result.value;
        m_LowerBoundKeyFrameIndex = result.lowerBoundKeyFrameIndex;
    }
    return retVal;
}

/*!
 * 開始フレーム設定
 * @author Miyake Shunsuke
 * @since 2012.01.05
 */
template <typename T>
TbResult TbAnimationKeyFrame<T>::SetFrameBegin( TbFloat32 frame )
{
    if( frame < 0.0f ){
        return TB_E_FAIL;
    }
    
    m_FrameBegin = frame;
    if( m_Frame < m_FrameBegin ){
        // フレームが開始フレームよりも小さい場合は
        SetFrame(m_FrameBegin);
    }
    
    return TB_S_OK;
}

/*!
 * 最終フレーム設定
 * @author Miyake Shunsuke
 * @since 2012.01.05
 */
template <typename T>
TbResult TbAnimationKeyFrame<T>::SetFrameEnd( TbFloat32 frame )
{
    if( frame < 0.0f ){
        return TB_E_FAIL;
    }
    
    m_FrameEnd = frame;
    if( frame > m_FrameEnd ){
        // フレームが最終フレームより大きい場合は
        SetFrame(m_FrameEnd);
    }
    
    return TB_S_OK;
}

/*!
 * 現在のデータ取得
 * @retval TB_TRUE 有効な値
 * @retval TB_FALSE 無効な値
 * @author Miyake Shunsuke
 * @since 2012.01.03
 */
template <typename T>
TbBool TbAnimationKeyFrame<T>::GetCurrentValue( T& result ) const
{
    if( m_Data ){
        result = m_CurrentValue;
        return TB_TRUE;
    }
    return TB_FALSE;
}
