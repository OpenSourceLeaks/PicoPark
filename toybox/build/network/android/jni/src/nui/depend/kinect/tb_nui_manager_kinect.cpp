/*! 
 * NUI管理クラス(KinectSDK for Windows)
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */

#include <tb_fwd.h>
#include <nui/tb_nui_manager.h>

#if TB_NUIAPI_IS_KINECT
#include <base/util/tb_async_func.h>
#include <nui/tb_nui_depth_frame.h>
#include <nui/tb_nui_color_frame.h>
#include <nui/tb_nui_util.h>

namespace toybox
{

static const TbFloat32 DEPTH_SCALE_TABLE[] = {
    4.0f , // TB_NUI_IMAGE_RESOLUTION_80x60 
    1.0f , // TB_NUI_IMAGE_RESOLUTION_320x240 
    0.0f , // TB_NUI_IMAGE_RESOLUTION_512x424 
    0.5f , // TB_NUI_IMAGE_RESOLUTION_640x480 
    0.0f , // TB_NUI_IMAGE_RESOLUTIO_N960x540
    0.25f , // TB_NUI_IMAGE_RESOLUTION_1280x1024
    0.0f , // TB_NUI_IMAGE_RESOLUTIO_N1920x1080
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(DEPTH_SCALE_TABLE)==TB_NUI_IMAGE_RESOLUTION_MAX);

static const TbFloat32 EPSILON = 1.192092896e-07F;  // float誤差に利用
static const TbUint32 STACK_SIZE = 8;               // スタックサイズ

inline DWORD TbGetNuiInitializeFlag( TbUint32 initializeFlagTb )
{
    static const DWORD COLOR_FLAG_TABLE[2] = { 0 , NUI_INITIALIZE_FLAG_USES_COLOR };
    static const DWORD DEPTH_FLAG_TABLE[2] = { 0 , NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX };
    static const DWORD SKELETON_FLAG_TABLE[2] = { 0 , NUI_INITIALIZE_FLAG_USES_SKELETON };
    DWORD result = COLOR_FLAG_TABLE[!!( TB_NUI_INITIALIZE_BIT_USE_COLOR & initializeFlagTb )] | 
                   DEPTH_FLAG_TABLE[!!( TB_NUI_INITIALIZE_BIT_USE_DEPTH & initializeFlagTb )] | 
                   SKELETON_FLAG_TABLE[!!( TB_NUI_INITIALIZE_BIT_USE_SKELETON & initializeFlagTb )];
    return result;
}

inline TbResult ConvertTbNuiImageResolutionTypeToNUI_IMAGE_RESOLUTION( NUI_IMAGE_RESOLUTION& result , const TbNuiImageResolutionType& type )
{
    static const NUI_IMAGE_RESOLUTION CONVERT_TABLE[] = {
        NUI_IMAGE_RESOLUTION_80x60    , // TB_NUI_IMAGE_RESOLUTION_80x60 
        NUI_IMAGE_RESOLUTION_320x240  , // TB_NUI_IMAGE_RESOLUTION_320x240 
        NUI_IMAGE_RESOLUTION_INVALID  , // 未サポート
        NUI_IMAGE_RESOLUTION_640x480  , // TB_NUI_IMAGE_RESOLUTION_640x480 
        NUI_IMAGE_RESOLUTION_INVALID  , // 未サポート
        NUI_IMAGE_RESOLUTION_1280x960 , // TB_NUI_IMAGE_RESOLUTION_1280x1024
        NUI_IMAGE_RESOLUTION_INVALID  , // 未サポート
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE)==TB_NUI_IMAGE_RESOLUTION_MAX);
    if( 0 <= type && type < TB_ARRAY_COUNT_OF(CONVERT_TABLE) ){
        result = CONVERT_TABLE[ type ];
        return TB_S_OK;
    }
    return TB_E_INVALIDARG;
}

inline void TbConvertVector4ToTbVector4( TbVector4& dst , const Vector4& src ){
    dst.Set( src.x , src.y , src.z );
}

enum {
    FUNC_INITIALIZE , 
    FUNC_UPDATE , 
};

// 依存部分

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbNuiManagerDepend::TbNuiManagerDepend()
    : m_AsyncFunc()
    , m_ColorDepthPoint(nullptr)
{
    TbMemClear( &m_SkeletonFrame , sizeof(m_SkeletonFrame) );
    const TbAsyncFunc::FuncType ASYNC_FUNC_PARAM[] = 
    {
        TbCreateMemFunc(this,&Self::onInitialize) ,
        TbCreateMemFunc(this,&Self::onUpdate) ,
    };
    m_AsyncFunc.Initilaize( ASYNC_FUNC_PARAM , TB_ARRAY_COUNT_OF(ASYNC_FUNC_PARAM) , STACK_SIZE );
    m_AsyncFunc.SetEventCallBack( TbAsyncFunc::EVENT_TYPE_FUNC_END ,  
                                  TbCreateMemFunc(this,&Self::onAfterInitialize) );
    m_AsyncFunc.Call( FUNC_INITIALIZE );

    const TbDimensionS32& size = TbNuiGetResolution(TB_NUI_IMAGE_RESOLUTION_640x480);
    TbUint32 pointSize = size.width*size.height;
    m_ColorDepthPoint = new TbPointS16[pointSize];
    for( TbUint32 i = 0; i < pointSize; ++i ) {
        m_ColorDepthPoint[i].x = -1;
        m_ColorDepthPoint[i].y = -1;
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbNuiManagerDepend::~TbNuiManagerDepend()
{
    TB_SAFE_DELETE_ARRAY(m_ColorDepthPoint);
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
void TbNuiManagerDepend::Update()
{
    m_AsyncFunc.Update();
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2011.08.03
 */
TbSint32 TbNuiManagerDepend::onInitialize( TbAsyncFunc& asyncFunc )
{
    enum {
        PHASE_INITIALIZE , 
        PHASE_ENABLE_TRACK , 
        PHASE_OPEN_COLOR_STREAM = 100 , 
        PHASE_OPEN_DEPTH_STREAM = 200 , 
    };
    HRESULT hr = 0;
    switch( asyncFunc.GetPhase() ){
    case PHASE_INITIALIZE:
        {
            DWORD flag = TbGetNuiInitializeFlag( m_Common->GetCreateParam().flagBit );
            hr = NuiInitialize( flag );
            if( hr != S_OK ){
                return TbAsyncFunc::RETURN_YIELD; // 中断
            }
            asyncFunc.SetPhase( PHASE_ENABLE_TRACK );
        }
        // break;
    case PHASE_ENABLE_TRACK:
        {
            if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_SKELETON ){
                hr = NuiSkeletonTrackingEnable( NULL , 0 );
                if( hr != S_OK ){
                    return TbAsyncFunc::RETURN_YIELD; // 中断
                }
            }
            asyncFunc.SetPhase( PHASE_OPEN_COLOR_STREAM );
        }
        // break;
    case PHASE_OPEN_COLOR_STREAM: // 640x480解像度深度ストリーム
        if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_COLOR )
        {
            m_Common->m_ColorFrame = TB_NEW TbNuiColorFrame( TB_NUI_IMAGE_RESOLUTION_640x480 );
            if( !TB_VERIFY( m_Common->m_ColorFrame ) ){
                return TbAsyncFunc::RETURN_YIELD; // 中断
            }
        }
        asyncFunc.AddPhase( 1 );
        // break;
    case PHASE_OPEN_COLOR_STREAM + 1:
        if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_COLOR )
        {
            if( TB_S_OK != m_Common->m_ColorFrame->OpenStream() ){
                return TbAsyncFunc::RETURN_YIELD; // 中断
            }
        }
        asyncFunc.SetPhase( PHASE_OPEN_DEPTH_STREAM );
        // break;
    case PHASE_OPEN_DEPTH_STREAM: // 320x240解像度深度ストリーム
        if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_DEPTH )
        {
            m_Common->m_DepthFrame = TB_NEW TbNuiDepthFrame( TB_NUI_IMAGE_RESOLUTION_320x240 );
            if( !TB_VERIFY( m_Common->m_DepthFrame ) ){
                return TbAsyncFunc::RETURN_YIELD; // 中断
            }
        }
        asyncFunc.AddPhase( 1 );
        // break;
    case PHASE_OPEN_DEPTH_STREAM + 1: 
        if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_DEPTH )
        {
            if( TB_S_OK != m_Common->m_DepthFrame->OpenStream() ){
                return TbAsyncFunc::RETURN_YIELD; // 中断
            }
        }
        asyncFunc.AddPhase( 1 );
        // break;
    case PHASE_OPEN_DEPTH_STREAM + 2: // 80x60解像度深度ストリーム
        if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_DEPTH )
        {
//            m_Common->m_DepthFrame80x60 = TB_NEW TbNuiDepthFrame( TB_NUI_IMAGE_RESOLUTION_80x60 );
//            if( !TB_IF_ASSERT( m_Common->m_DepthFrame80x60 ) ){
//                return TbAsyncFunc::RETURN_YIELD; // 中断
//            }
        }
        asyncFunc.AddPhase( 1 );
        // break;
    case PHASE_OPEN_DEPTH_STREAM + 3:
        if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_DEPTH )
        {
//            if( TB_S_OK != m_Common->m_DepthFrame80x60->OpenStream() ){
//                return TbAsyncFunc::RETURN_YIELD; // 中断
//            }
        }
        // break;
    }
    m_Common->m_BitArray.SetBit( TbNuiManager::FLAG_READY );
    return TbAsyncFunc::RETURN_FINISH;
}

/*!
 * 初期化後処理
 * @author Miyake Shunsuke
 * @since 2011.08.03
 */
TbSint32 TbNuiManagerDepend::onAfterInitialize( TbAsyncFunc& asyncFunc , const TbUint32 funcIndex )
{
    if( FUNC_INITIALIZE == funcIndex ){
        asyncFunc.Call( FUNC_UPDATE );
    }
    return 1;
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.08.03
 */
TbSint32 TbNuiManagerDepend::onUpdate( TbAsyncFunc& asyncFunc )
{
    static const TbNuiSkeletonTrackState TRACK_STATE[] = {
        TB_NUI_SKELETON_TRACKED_NOT , 
        TB_NUI_SKELETON_TRACKED_POS , 
        TB_NUI_SKELETON_TRACKED , 
    };

    HRESULT hr = NuiSkeletonGetNextFrame( INFINITE , &m_SkeletonFrame );
    if( SUCCEEDED(hr) ){
        m_Common->m_TrackedSkeletonCount = 0;
        TbConvertVector4ToTbVector4( m_Common->m_GravityNormal , m_SkeletonFrame.vNormalToGravity );
        // スケルトン情報をコピー
        for( TbSint32 i = 0; i < TB_NUI_SKELETON_MAX; ++i ){
            TbNuiSkeleton& skeleton = m_Common->m_Skeleton[i];
            skeleton.trackedSkeleton = NULL;

            // 座標
            const NUI_SKELETON_DATA& skeletonData = m_SkeletonFrame.SkeletonData[i];
            TbConvertVector4ToTbVector4( skeleton.pos , skeletonData.Position );

            // 追跡状態
            skeleton.trackState = TRACK_STATE[skeletonData.eTrackingState];

            // クリップフラグ更新
            skeleton.bitArray.Clear();
            if( skeletonData.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_TOP ){
                skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_TOP );
            }
            if( skeletonData.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_BOTTOM ){
                skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_BOTTOM );
            }
            if( skeletonData.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_LEFT ){
                skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_LEFT );
            }
            if( skeletonData.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_RIGHT ){
                skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_RIGHT );
            }
            // 追跡スケルトン情報
            if( skeleton.trackState == TB_NUI_SKELETON_TRACKED ){
                TB_ASSERT( m_Common->m_TrackedSkeletonCount < TB_NUI_TRACKED_SKELETON_MAX );
                skeleton.trackedSkeleton = &m_Common->m_TrackedSkeleton[ m_Common->m_TrackedSkeletonCount ];
                for( TbUint32 posIndex = 0; posIndex < TB_NUI_SKELETON_POS_COUNT; ++posIndex ){
                    TbMemCopy( skeleton.trackedSkeleton->skeletonPos , skeletonData.SkeletonPositions , sizeof(skeleton.trackedSkeleton->skeletonPos) );
                }
                ++m_Common->m_TrackedSkeletonCount;
            }
        }
    }
    return TbAsyncFunc::RETURN_YIELD;
}

// 共通部分

/*! 
 * スケルトン座標を深度画像にマッピング
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbResult TbNuiManager::TransformSkeletonToDepth( TbNuiDepthPos& depthPos , const TbVector4& skeletonPos )
{
    depthPos.resolutionType = TB_NUI_IMAGE_RESOLUTION_320x240;
    if( skeletonPos.GetZ() > EPSILON){
        depthPos.x = static_cast<TbSint32>( 160.0f + skeletonPos.GetX() * ( NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / skeletonPos.GetZ() ) );
        depthPos.y = static_cast<TbSint32>( 120.0f - skeletonPos.GetY() * ( NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / skeletonPos.GetZ() ) );
        depthPos.depth = static_cast<TbUint16>(skeletonPos.GetZ()*1000.0f);
    } else {
        depthPos.x = 0;
        depthPos.y = 0;
        depthPos.depth = 0;
    }
    return TB_S_OK;
}

/*!
 * 深度座標をスケルトン座標に変換
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbResult TbNuiManager::TransformDepthToSkeleton( TbVector4& skeletonPos , const TbNuiDepthPos& depthPos )
{
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(DEPTH_SCALE_TABLE) == TB_NUI_IMAGE_RESOLUTION_MAX );
    if( depthPos.resolutionType < 0 || depthPos.resolutionType >= TB_NUI_IMAGE_RESOLUTION_MAX ){
        return TB_E_INVALIDARG;
    }

    skeletonPos.SetZ( static_cast<TbFloat32>(depthPos.depth) / 1000.0f );
    skeletonPos.SetX( (static_cast<TbFloat32>(depthPos.x)*DEPTH_SCALE_TABLE[depthPos.resolutionType] - 0.5f) * ( NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240 * skeletonPos.GetZ() ) * 320.0f );
    skeletonPos.SetY( (0.5f - static_cast<TbFloat32>(depthPos.y)*DEPTH_SCALE_TABLE[depthPos.resolutionType]) * ( NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240 * skeletonPos.GetZ() ) * 240.0f );
    skeletonPos.SetW( 1.0f );
    return TB_S_OK;
}

/*! 
 * スケルトン座標をカラー画像にマッピング
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbResult TbNuiManager::TransformSkeletonToColor( TbNuiColorPos& colorPos , const TbVector4& skeletonPos )
{
    TbNuiDepthPos depthPos;
    TransformSkeletonToDepth( depthPos , skeletonPos );
    TransformDepthToColor( colorPos , depthPos );
    return TB_S_OK;
}

/*!
 * 深度座標をカラー座標に変換
 * @author Miyake Shunsuke
 * @since 2011.08.24
 */
TbResult TbNuiManager::TransformDepthToColor( TB_INOUT TbNuiColorPos& colorPos , const TbNuiDepthPos& depthPos )
{
    LONG colorX = 0;
    LONG colorY = 0;
    NUI_IMAGE_RESOLUTION resolution;
    if( TB_SUCCEEDED( ConvertTbNuiImageResolutionTypeToNUI_IMAGE_RESOLUTION( resolution , colorPos.resolutionType ) ) ){
        LONG depthX = static_cast<LONG>(depthPos.x * DEPTH_SCALE_TABLE[ depthPos.resolutionType ]);
        LONG depthY = static_cast<LONG>(depthPos.y * DEPTH_SCALE_TABLE[ depthPos.resolutionType ]);
        HRESULT hr = NuiImageGetColorPixelCoordinatesFromDepthPixel( resolution , 
                                                                     NULL , 
                                                                     depthX ,
                                                                     depthY ,
                                                                     depthPos.depth << 3 , 
                                                                     &colorX ,
                                                                     &colorY );
        if( SUCCEEDED(hr) ){
            colorPos.x = colorX;
            colorPos.y = colorY;
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * カラー座標を深度座標に変換
 * @author teco
 */
TbResult TbNuiManager::TransformColorToDepth( TB_INOUT TbNuiDepthPos& depthPos , TbSint16* playerIndex , const TbNuiColorPos& colorPos )
{
    TB_RESQUE_RET(depthPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_320x240,TB_E_FAIL);
    TB_RESQUE_RET(colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_640x480,TB_E_FAIL);
    depthPos.x = 0;
    depthPos.y = 0;
    depthPos.depth = 0;
    if( !m_DepthFrame->IsReady() ) {
        return TB_E_FAIL;
    }

    const TbDimensionS32& res = TbNuiGetResolution(TB_NUI_IMAGE_RESOLUTION_640x480);
    const TbPointS16& point = m_Depend.m_ColorDepthPoint[colorPos.y*res.width+colorPos.x];
    if( point.x < 0 || point.y < 0 ){
        return TB_E_FAIL;
    }
    depthPos.depth = m_DepthFrame->GetDepth(point.x,point.y,playerIndex);
    return TB_S_OK;
}


/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbNuiManager::TbNuiManager( const TbNuiCreateParam& param )
    : m_BitArray()
    , m_CreateParam( param )
    , m_GravityNormal(0.0f,-1.0f,0.0f,0.0f)
    , m_ConnectedDeviceCount( 0 )
    , m_TrackedSkeletonCount(0)
    , m_ColorFrame( NULL )
    , m_DepthFrame( NULL )
    , m_Depend()
{
    m_Depend.setCommon( this );
    TbMemClear( &m_Skeleton , sizeof(m_Skeleton) );
    TbMemClear( &m_TrackedSkeleton , sizeof(m_TrackedSkeleton) );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbNuiManager::~TbNuiManager()
{
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
void TbNuiManager::Update()
{
    m_Depend.Update();
    if( m_ColorFrame ){
        m_ColorFrame->Update();
    }
    if( m_DepthFrame ){
        m_DepthFrame->Update();
        
        if( m_ColorFrame->IsReady() && m_DepthFrame->IsReady() && TB_SUCCEEDED(m_DepthFrame->Lock()) ){
            const TbDimensionS32& depthRes = m_DepthFrame->GetResolution();
            const TbDimensionS32& colorRes = TbNuiGetResolution( TB_NUI_IMAGE_RESOLUTION_640x480 );

            for( TbSint32 y = 0; y < depthRes.height; ++y ) 
            {
                for( TbSint32 x = 0; x < depthRes.width; ++x ) 
                {
                    LONG colorX = 0;
                    LONG colorY= 0;
                    TbUint16 depth = m_DepthFrame->GetDepth(x,y,nullptr);
                    HRESULT hr = NuiImageGetColorPixelCoordinatesFromDepthPixel( NUI_IMAGE_RESOLUTION_640x480 , 
                                                                                 NULL , 
                                                                                 x ,
                                                                                 y ,
                                                                                 depth << 3 , 
                                                                                 &colorX ,
                                                                                 &colorY );
                    {
                        TbPointS16& dst = m_Depend.m_ColorDepthPoint[colorY*colorRes.width+colorX];
                        dst.x = x;
                        dst.y = y;
                    }
                    if( colorX < colorRes.width-1 ){
                        TbPointS16& dst = m_Depend.m_ColorDepthPoint[colorY*colorRes.width+colorX+1];
                        dst.x = x;
                        dst.y = y;
                    }
                    if( colorY < colorRes.height-1 ){
                        TbPointS16& dst = m_Depend.m_ColorDepthPoint[(colorY+1)*colorRes.width+colorX];
                        dst.x = x;
                        dst.y = y;
                    }
                    if( colorX < colorRes.width-1 && colorY < colorRes.height-1 ){
                        TbPointS16& dst = m_Depend.m_ColorDepthPoint[(colorY+1)*colorRes.width+colorX+1];
                        dst.x = x;
                        dst.y = y;
                    }
                }
            }
            m_DepthFrame->Unlock();
        }
    }
}
    
/*!
 * スケルトンを取得
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
const TbNuiSkeleton* TbNuiManager::GetSkeleton( TbUint32 index ) const
{
    if( 0 <= index && index < TB_NUI_SKELETON_MAX ){
        return &m_Skeleton[ index ];
    }
    return NULL;
}
    
/*!
 * 深度フレームを取得
 * @author Miyake Shunsuke
 */
TbNuiDepthFrame* TbNuiManager::GetDepthFrame( TbNuiImageResolutionType resolution , TbUint32 index )
{
    if( index != 0 ){
        return NULL;
    }
    if( m_DepthFrame && resolution == m_DepthFrame->GetResolutionType() ){
        return m_DepthFrame;
    }
    return NULL;
}

/*!
 * 深度フレームを取得
 * @author Miyake Shunsuke
 */
TbNuiDepthFrame* TbNuiManager::GetDepthFrame( TbUint32 index )
{
    if( index != 0 ){
        return NULL;
    }
    return m_DepthFrame;
}

/*!
 * 深度フレームを取得
 * @author Miyake Shunsuke
 */
const TbNuiDepthFrame* TbNuiManager::GetDepthFrame( TbNuiImageResolutionType resolution , TbUint32 index ) const
{
    if( index != 0 ){
        return NULL;
    }
    if( m_DepthFrame && resolution == m_DepthFrame->GetResolutionType() ){
        return m_DepthFrame;
    }
    return NULL;
}

/*!
 * 深度フレームを取得
 * @author Miyake Shunsuke
 */
const TbNuiDepthFrame* TbNuiManager::GetDepthFrame( TbUint32 index ) const
{
    if( index != 0 ){
        return NULL;
    }
    return m_DepthFrame;
}

/*!
 * RGBフレームを取得
 * @author Miyake Shunsuke
 */
TbNuiColorFrame* TbNuiManager::GetColorFrame( TbNuiImageResolutionType resolution , TbUint32 index )
{
    if( index != 0 ){
        return NULL;
    }
    if( m_ColorFrame && resolution == m_ColorFrame->GetResolutionType() ){
        return m_ColorFrame;
    }
    return NULL;
}

/*!
 * RGBフレームを取得
 * @author Miyake Shunsuke
 */
TbNuiColorFrame* TbNuiManager::GetColorFrame( TbUint32 index )
{
    if( index != 0 ){
        return NULL;
    }
    return m_ColorFrame;
}

/*!
 * RGBフレームを取得
 * @author Miyake Shunsuke
 */
const TbNuiColorFrame* TbNuiManager::GetColorFrame( TbUint32 index ) const
{
    if( index != 0 ){
        return NULL;
    }
    return m_ColorFrame;
}


/*!
 * 傾き加える
 * @author Miyake Shunsuke
 * @since 2012.08.10
 */
TbResult TbNuiManager::AddTiltMotorAngleDeg( const TbFloat32 deg )
{
    TbFloat32 src;
    if( TB_SUCCEEDED( GetTiltMotorAngleDeg( src ) ) ){
        return SetTiltMotorAngleDeg( src + deg );
    }
    return TB_E_FAIL;
}

/*!
 * 傾き取得
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbResult TbNuiManager::SetTiltMotorAngleDeg( const TbFloat32 deg )
{
    LONG src = static_cast<LONG>(deg);
    NuiCameraElevationSetAngle(src);
    return TB_S_OK;
}

/*!
 * 傾き設定
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbResult TbNuiManager::GetTiltMotorAngleDeg( TbFloat32& deg ) const
{
    LONG src = 0;
    NuiCameraElevationGetAngle( &src );
    deg = static_cast<TbFloat32>(src);
    return TB_S_OK;
}

}
#endif