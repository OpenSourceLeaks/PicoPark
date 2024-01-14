/*! 
 * NUI管理クラス(KinectSDK for Windows)
 * @author Miyake Shunsuke
 * @since 2011.08.04
 */

#include <tb_fwd.h>
#include <nui/tb_nui_manager.h>

#if TB_NUIAPI_IS_KINECT2
#include <base/util/tb_async_func.h>
#include <nui/tb_nui_depth_frame.h>
#include <nui/tb_nui_color_frame.h>
#include <nui/tb_nui_util.h>

namespace toybox
{

static const TbUint32 STACK_SIZE = 8;               // スタックサイズ

enum {
    FUNC_INITIALIZE , 
    FUNC_UPDATE , 
};

// 依存部分

/*!
 * コンストラクタ
 * @author teco
 */
TbNuiManagerDepend::TbNuiManagerDepend()
    : m_AsyncFunc()
    , m_KinectSensor(nullptr)
    , m_CoordinateMapper(nullptr)
    , m_BodyFrameReader(nullptr)
    , m_ColorDepthPoint(nullptr)
    , m_MultiSourceFrameReader(nullptr)
    , m_ColorFrame(nullptr)
    , m_DepthFrame(nullptr)
    , m_BodyIndexFrame(nullptr)
{
    const TbAsyncFunc::FuncType ASYNC_FUNC_PARAM[] = 
    {
        TbCreateMemFunc(this,&Self::onInitialize) ,
        TbCreateMemFunc(this,&Self::onUpdate) ,
    };
    m_AsyncFunc.Initilaize( ASYNC_FUNC_PARAM , TB_ARRAY_COUNT_OF(ASYNC_FUNC_PARAM) , STACK_SIZE );
    m_AsyncFunc.SetEventCallBack( TbAsyncFunc::EVENT_TYPE_FUNC_END ,  
                                  TbCreateMemFunc(this,&Self::onAfterInitialize) );
    m_AsyncFunc.Call( FUNC_INITIALIZE );

    TbDimensionS32 size = TbNuiGetResolution(TB_NUI_IMAGE_RESOLUTION_1920x1080);
    m_ColorDepthPoint = new DepthSpacePoint[size.width*size.height];
}

/*!
 * デストラクタ
 * @author teco
 */
TbNuiManagerDepend::~TbNuiManagerDepend()
{
    TB_SAFE_DELETE_ARRAY(m_ColorDepthPoint);

    TB_SAFE_RELEASE(m_ColorFrame);
    TB_SAFE_RELEASE(m_DepthFrame);
    TB_SAFE_RELEASE(m_BodyIndexFrame);
    TB_SAFE_RELEASE(m_MultiSourceFrameReader);
}

/*!
 * 更新
 * @author teco
 */
void TbNuiManagerDepend::Update()
{
    m_AsyncFunc.Update();
}

/*!
 * 初期化
 * @author teco
 */
TbSint32 TbNuiManagerDepend::onInitialize( TbAsyncFunc& asyncFunc )
{
    enum {
        PHASE_INITIALIZE , 
        PHASE_ENABLE_TRACK = 10 , 
        PHASE_OPEN_COLOR_STREAM = 100 , 
        PHASE_OPEN_DEPTH_STREAM = 200 , 
    };
    HRESULT hr = 0;
    switch( asyncFunc.GetPhase() ){
    case PHASE_INITIALIZE:
        {
            hr = ::GetDefaultKinectSensor(&m_KinectSensor);
            if( hr != S_OK ){
                return TbAsyncFunc::RETURN_YIELD; // 中断
            }
            asyncFunc.AddPhase(1);
        }
        // break;
    case PHASE_INITIALIZE+1:
        {
            hr = m_KinectSensor->Open();
            if( hr != S_OK ){
                return TbAsyncFunc::RETURN_YIELD; // 中断
            }
            asyncFunc.SetPhase(PHASE_ENABLE_TRACK);
        }
        // break;
    case PHASE_ENABLE_TRACK:
        {
            if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_SKELETON )
            {
                hr = m_KinectSensor->get_CoordinateMapper(&m_CoordinateMapper);
                if( FAILED(hr) ){
                    return TbAsyncFunc::RETURN_YIELD; // 中断
                }
                m_KinectSensor->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Color |
                                                           FrameSourceTypes::FrameSourceTypes_Depth | 
                                                           FrameSourceTypes::FrameSourceTypes_BodyIndex,&m_MultiSourceFrameReader);
            }
            asyncFunc.AddPhase(1);
        }
        // break
    case PHASE_ENABLE_TRACK+1:
        {
            if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_SKELETON )
            {
                // スケルトンを取得
                IBodyFrameSource* bodyFrameSource = nullptr;
                hr = m_KinectSensor->get_BodyFrameSource(&bodyFrameSource);
                if( FAILED(hr) ){
                    return TbAsyncFunc::RETURN_YIELD; // 中断
                }
                hr = bodyFrameSource->OpenReader(&m_BodyFrameReader);
                TB_SAFE_RELEASE(bodyFrameSource);
                if( FAILED(hr) ){
                    return TbAsyncFunc::RETURN_YIELD; // 中断
                }
            }
            asyncFunc.SetPhase( PHASE_OPEN_COLOR_STREAM );
        }
        // break;
    case PHASE_OPEN_COLOR_STREAM: // 640x480解像度カラーストリーム
        if( m_Common->GetCreateParam().flagBit & TB_NUI_INITIALIZE_BIT_USE_COLOR )
        {
            m_Common->m_ColorFrame = TB_NEW TbNuiColorFrame( TB_NUI_IMAGE_RESOLUTION_960x540 );
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
            m_Common->m_DepthFrame = TB_NEW TbNuiDepthFrame( TB_NUI_IMAGE_RESOLUTION_512x424 );
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
    }
    m_Common->m_BitArray.SetBit( TbNuiManager::FLAG_READY );
    return TbAsyncFunc::RETURN_FINISH;
}

/*!
 * 初期化後処理
 * @author teco
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
 * @author teco
 */
TbSint32 TbNuiManagerDepend::onUpdate( TbAsyncFunc& asyncFunc )
{
    onUpdateBodyFrame();
    onUpdateMultiSource();
    return TbAsyncFunc::RETURN_YIELD;
}

/*! 
 * 骨格フレーム更新
 */
void TbNuiManagerDepend::onUpdateBodyFrame()
{
    static const JointType JOINT_TYPE_TABLE[] =
    {
        JointType_SpineBase     ,    // TB_NUI_SKELETON_POS_HIP_C       , // 腰中心
        JointType_SpineMid      ,    // TB_NUI_SKELETON_POS_SPINE       , // 背骨
        JointType_SpineShoulder ,    // TB_NUI_SKELETON_POS_SHOULDER_C  , // 肩中心
        JointType_Head          ,    // TB_NUI_SKELETON_POS_HEAD        , // 頭
        JointType_ShoulderLeft  ,    // TB_NUI_SKELETON_POS_SHOULDER_L  , // 肩左
        JointType_ElbowLeft     ,   // TB_NUI_SKELETON_POS_ELBOW_L     , // 肘左
        JointType_WristLeft     ,    // TB_NUI_SKELETON_POS_WRIST_L     , // 手首左
        JointType_HandLeft      ,   // TB_NUI_SKELETON_POS_HAND_L      , // 手左
        JointType_ShoulderRight ,    // TB_NUI_SKELETON_POS_SHOULDER_R  , // 肩右
        JointType_ElbowRight    ,   // TB_NUI_SKELETON_POS_ELBOW_R     , // 肘右
        JointType_WristRight    ,    // TB_NUI_SKELETON_POS_WRIST_R     , // 手首右
        JointType_HandRight     ,    // TB_NUI_SKELETON_POS_HAND_R      , // 手右
        JointType_HipLeft       , // TB_NUI_SKELETON_POS_HIP_L       , // 腰左
        JointType_KneeLeft      ,  // TB_NUI_SKELETON_POS_KNEE_L      , // 膝左
        JointType_AnkleLeft     ,   // TB_NUI_SKELETON_POS_ANKLE_L     , // 足首左
        JointType_FootLeft      ,    // TB_NUI_SKELETON_POS_FOOT_L      , // 足左
        JointType_HipRight      ,    // TB_NUI_SKELETON_POS_HIP_R       , // 腰右
        JointType_KneeRight     ,    // TB_NUI_SKELETON_POS_KNEE_R      , // 膝右
        JointType_AnkleRight    ,    // TB_NUI_SKELETON_POS_ANKLE_R     , // 足首右
        JointType_FootRight     // TB_NUI_SKELETON_POS_FOOT_R      , // 足右
    };
    IBodyFrame* bodyFrame = nullptr;
    IBody* bodies[BODY_COUNT] = {0};

    HRESULT hr = m_BodyFrameReader ? m_BodyFrameReader->AcquireLatestFrame(&bodyFrame) : E_FAIL;
    if(FAILED(hr)) {
        return;
    }

    m_Common->m_TrackedSkeletonCount = 0;
    INT64 time = 0;
    hr = bodyFrame->get_RelativeTime(&time);
    if (SUCCEEDED(hr))
    {
        hr = bodyFrame->GetAndRefreshBodyData(TB_ARRAY_COUNT_OF(bodies), bodies);
    }
    for (int i = 0; i < TB_ARRAY_COUNT_OF(bodies); ++i)
    {
        IBody* body = bodies[i];
        TbNuiSkeleton& skeleton = m_Common->m_Skeleton[i];
        BOOLEAN isTracked = FALSE;
        if( body ){
            body->get_IsTracked(&isTracked);
        }
        if(isTracked)
        {
            skeleton.trackState = TB_NUI_SKELETON_TRACKED;
            TB_ASSERT( m_Common->m_TrackedSkeletonCount < TB_NUI_TRACKED_SKELETON_MAX );
            skeleton.trackedSkeleton = &m_Common->m_TrackedSkeleton[ m_Common->m_TrackedSkeletonCount ];
            skeleton.bitArray.Clear();
            ++m_Common->m_TrackedSkeletonCount;

            // 骨格座標
            Joint joints[JointType_Count];
            hr = body->GetJoints( TB_ARRAY_COUNT_OF(joints) , joints );
            if( SUCCEEDED(hr))
            {
                for (int j = 0; j < TB_ARRAY_COUNT_OF(skeleton.trackedSkeleton->skeletonPos); ++j)
                {
                    const CameraSpacePoint& pos = joints[JOINT_TYPE_TABLE[j]].Position;
                    skeleton.trackedSkeleton->skeletonPos[j].Set(pos.X,pos.Y,pos.Z);
                }
            }
            skeleton.pos = skeleton.trackedSkeleton->skeletonPos[TB_NUI_SKELETON_POS_HIP_C];

            // クリップフラグ
            DWORD clippedEdge = 0;
            hr = body->get_ClippedEdges(&clippedEdge);
            if( SUCCEEDED(hr)) {
                if( FrameEdge_Top & clippedEdge )
                {
                    skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_TOP );
                }
                if( FrameEdge_Bottom & clippedEdge )
                {
                    skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_BOTTOM );
                }
                if( FrameEdge_Left & clippedEdge )
                {
                    skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_LEFT );
                }
                if( FrameEdge_Right & clippedEdge )
                {
                    skeleton.bitArray.SetBit( TbNuiSkeleton::FLAG_CLIPPED_RIGHT );
                }
            }
        }
        else
        {
            skeleton.bitArray.Clear();
            skeleton.pos.Clear();
            skeleton.trackedSkeleton = nullptr;
            skeleton.trackState = TB_NUI_SKELETON_TRACKED_NOT;
        }
    }
    for (int i = 0; i < TB_ARRAY_COUNT_OF(bodies); ++i){
        TB_SAFE_RELEASE(bodies[i]);
    }
    TB_SAFE_RELEASE(bodyFrame);
}

/*! 
 * マルチソース更新
 */
void TbNuiManagerDepend::onUpdateMultiSource()
{
#if 0
    TB_SAFE_RELEASE(m_ColorFrame);
    TB_SAFE_RELEASE(m_DepthFrame);
    TB_SAFE_RELEASE(m_BodyIndexFrame);
    IMultiSourceFrame* frame = nullptr;
    HRESULT hr = m_MultiSourceFrameReader->AcquireLatestFrame(&frame);
    if( FAILED(hr) ){
        return;
    }
//    if (SUCCEEDED(hr))
    {
        IDepthFrameReference* depthFrameReference = nullptr;
        hr = frame->get_DepthFrameReference(&depthFrameReference);
        if (SUCCEEDED(hr))
        {
            hr = depthFrameReference->AcquireFrame(&m_DepthFrame);
        }
        TB_SAFE_RELEASE(depthFrameReference);
    }
//    if (SUCCEEDED(hr))
    {
        IColorFrameReference* colorFrameReference = nullptr;
        hr = frame->get_ColorFrameReference(&colorFrameReference);
        if (SUCCEEDED(hr))
        {
            hr = colorFrameReference->AcquireFrame(&m_ColorFrame);
        }
        TB_SAFE_RELEASE(colorFrameReference);
    }
//    if (SUCCEEDED(hr))
    {
        IBodyIndexFrameReference* bodyIndexFrameReference = NULL;
        hr = frame->get_BodyIndexFrameReference(&bodyIndexFrameReference);
        if (SUCCEEDED(hr))
        {
            hr = bodyIndexFrameReference->AcquireFrame(&m_BodyIndexFrame);
        }
        TB_SAFE_RELEASE(bodyIndexFrameReference);
    }
    TB_SAFE_RELEASE(frame);
#endif
}

// 共通部分

/*! 
 * スケルトン座標を深度画像にマッピング
 * @author Miyake Shunsuke
 */
TbResult TbNuiManager::TransformSkeletonToDepth( TbNuiDepthPos& depthPos , const TbVector4& skeletonPos )
{
    TB_RESQUE_RET(depthPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_512x424,TB_E_FAIL);
    depthPos.x = 0;
    depthPos.y = 0;
    depthPos.depth = 0;
    if( m_Depend.m_CoordinateMapper ) {
        CameraSpacePoint bodyPoint = { skeletonPos.GetX() , skeletonPos.GetY() , skeletonPos.GetZ() };
        DepthSpacePoint depthPoint = {0};
        HRESULT hr = m_Depend.m_CoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);
        if( SUCCEEDED(hr) ){
            depthPos.x = static_cast<TbSint32>(depthPoint.X);
            depthPos.y = static_cast<TbSint32>(depthPoint.Y);
            depthPos.depth = static_cast<TbUint16>(skeletonPos.GetZ()*1000.0f); // 要確認
            depthPos.resolutionType = TB_NUI_IMAGE_RESOLUTION_512x424;
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * 深度座標をスケルトン座標に変換
 * @author Miyake Shunsuke
 */
TbResult TbNuiManager::TransformDepthToSkeleton( TbVector4& skeletonPos , const TbNuiDepthPos& depthPos )
{
    TB_RESQUE_RET(depthPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_512x424,TB_E_FAIL);
    skeletonPos.Clear();
    if( m_Depend.m_CoordinateMapper ) {
        CameraSpacePoint bodyPoint = {0};
        DepthSpacePoint depthPoint = {static_cast<float>(depthPos.x),static_cast<float>(depthPos.y)};
        HRESULT hr = m_Depend.m_CoordinateMapper->MapDepthPointToCameraSpace(depthPoint,depthPos.depth,&bodyPoint);
        if( SUCCEEDED(hr) ){
            skeletonPos.Set(bodyPoint.X,bodyPoint.Y,bodyPoint.Z,1.0f);
            return TB_S_OK;
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*! 
 * スケルトン座標をカラー画像にマッピング
 * @author Miyake Shunsuke
 * @since 2011.07.28
 */
TbResult TbNuiManager::TransformSkeletonToColor( TbNuiColorPos& colorPos , const TbVector4& skeletonPos )
{
    TB_RESQUE_RET(colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_1920x1080 || 
                  colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540,TB_E_FAIL);
    colorPos.x = 0;
    colorPos.y = 0;
    if( m_Depend.m_CoordinateMapper )
    {
        TbFloat32 scale = colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540 ? 0.5f : 1.0f;
        ColorSpacePoint colorPoint = {0};
        CameraSpacePoint camerahPoint = {skeletonPos.GetX(),skeletonPos.GetY(),skeletonPos.GetZ()};
        HRESULT hr = m_Depend.m_CoordinateMapper->MapCameraPointToColorSpace(camerahPoint,&colorPoint);
        if( SUCCEEDED(hr) && !TbIsInfinite(colorPoint.X) && !TbIsInfinite(colorPoint.Y) ){
            colorPos.x = static_cast<TbSint32>(colorPoint.X*scale);
            colorPos.y = static_cast<TbSint32>(colorPoint.Y*scale);
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * 深度座標をカラー座標に変換
 * @author teco
 */
TbResult TbNuiManager::TransformDepthToColor( TB_INOUT TbNuiColorPos& colorPos , const TbNuiDepthPos& depthPos )
{
    TB_RESQUE_RET(depthPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_512x424,TB_E_FAIL);
    TB_RESQUE_RET(colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_1920x1080 || 
                  colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540,TB_E_FAIL);
    colorPos.x = 0;
    colorPos.y = 0;
    TbFloat32 scale = colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540 ? 0.5f : 1.0f;
    if( m_Depend.m_CoordinateMapper ) {
        ColorSpacePoint colorPoint = {0};
        DepthSpacePoint depthPoint =  {static_cast<float>(depthPos.x),static_cast<float>(depthPos.y)};
        HRESULT hr = m_Depend.m_CoordinateMapper->MapDepthPointToColorSpace(depthPoint,depthPos.depth,&colorPoint);
        if( SUCCEEDED(hr) && !TbIsInfinite(colorPoint.X) && !TbIsInfinite(colorPoint.Y) ){
            colorPos.x = static_cast<TbSint32>(colorPoint.X*scale);
            colorPos.y = static_cast<TbSint32>(colorPoint.Y*scale);
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * カラー座標を深度座標に変換
 * @author teco
 */
TbResult TbNuiManager::TransformColorToDepth( TB_INOUT TbNuiDepthPos& depthPos , TbSint16* skeletonIndex , const TbNuiColorPos& colorPos )
{
    TB_RESQUE_RET(depthPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_512x424,TB_E_FAIL);
    TB_RESQUE_RET(colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_1920x1080 || 
                  colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540,TB_E_FAIL);
    depthPos.x = 0;
    depthPos.y = 0;
    depthPos.depth = 0;
    TbSint32 scale = colorPos.resolutionType==TB_NUI_IMAGE_RESOLUTION_960x540 ? 2 : 1;
    if( !m_DepthFrame->IsReady() ) {
        return TB_E_FAIL;
    }
    const TbDimensionS32& res = TbNuiGetResolution(TB_NUI_IMAGE_RESOLUTION_1920x1080);
    const DepthSpacePoint& point = m_Depend.m_ColorDepthPoint[colorPos.y*scale*res.width+colorPos.x*scale];
    if( TbIsInfinite(point.X) || TbIsInfinite(point.Y) ) {
        return TB_E_FAIL;
    }
    depthPos.x = static_cast<TbSint32>(point.X);
    depthPos.y = static_cast<TbSint32>(point.Y);
    depthPos.depth = m_DepthFrame->GetDepth(point.X,point.Y,skeletonIndex);
    return TB_S_OK;
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
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
 */
TbNuiManager::~TbNuiManager()
{
    TB_SAFE_DELETE(m_DepthFrame);
    TB_SAFE_DELETE(m_ColorFrame);
}

/*!
 * 更新
 * @author Miyake Shunsuke
 */
void TbNuiManager::Update()
{
    m_Depend.Update();

    if( m_ColorFrame ){
        m_ColorFrame->Update();
    }

    if( m_DepthFrame ){
        m_DepthFrame->Update();

        if( IsReady() && m_ColorFrame->IsReady() && m_DepthFrame->IsReady() )
        {
            const TbDimensionS32& depthRes = m_DepthFrame->GetResolution();
            const TbDimensionS32& colorRes = TbNuiGetResolution(TB_NUI_IMAGE_RESOLUTION_1920x1080);
            UINT16* buf = reinterpret_cast<UINT16*>(m_DepthFrame->GetBuffer());
            HRESULT hr = m_Depend.m_CoordinateMapper->MapColorFrameToDepthSpace( depthRes.width*depthRes.height,
                                                                    buf,
                                                                    colorRes.width*colorRes.height,
                                                                    m_Depend.m_ColorDepthPoint);
        }
    }
}
    
/*!
 * スケルトンを取得
 * @author Miyake Shunsuke
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
 */
TbResult TbNuiManager::AddTiltMotorAngleDeg( const TbFloat32 deg )
{
    return TB_E_FAIL;
}

/*!
 * 傾き取得
 * @author Miyake Shunsuke
 */
TbResult TbNuiManager::SetTiltMotorAngleDeg( const TbFloat32 deg )
{
    return TB_S_OK;
}

/*!
 * 傾き設定
 * @author Miyake Shunsuke
 */
TbResult TbNuiManager::GetTiltMotorAngleDeg( TbFloat32& deg ) const
{
    deg = 0;
    return TB_S_OK;
}

}
#endif