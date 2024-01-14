/*!
 * リモート入力サーバー
 * @author teco
 */

#include <tb_fwd.h>
#include <crayon/input/cr_remote_input_server.h>
#include <crayon/input/cr_remote_input_types.h>
#include <input/tb_pad.h>
#include <input/tb_input_setting.h>
#include <base/util/tb_bind.h>
#include <base/crypt/tb_crc32.h>
#include <base/time/tb_tick.h>

#if TB_PLATFORM_IS_WIN
using namespace toybox;

namespace crayon
{

/*!
 * コンストラクタ
 * @author teco
 */
CrRemoteInputServer::CrRemoteInputServer()
    : TbTask(1,"CrRemoteInputServer")
    , m_IsEnableInput(TB_TRUE)
#if CR_REMOTE_INPUT_IS_ENABLE
    , m_NetworkManager(nullptr)
    , m_DatagramSocket(nullptr)
#endif
{
#if CR_REMOTE_INPUT_IS_ENABLE
    AddTaskLine(TB_TASK_LINE_SYSTEM_PRE,&Self::update);

    // ネットワーク初期化
    {
        TbNetworkInitParam initParam;
        initParam.socketMax = 64;
        m_NetworkManager = new TbNetworkManager();
        m_NetworkManager->Initialize(initParam);
    }

    m_DatagramSocket = new TbDatagramSocket();
    m_DatagramSocket->SetReceiveCallback( TbCreateMemFunc(this,&Self::onReceivedDatagramPacket) );
    m_DatagramSocket->Open(CR_REMOTE_INPUT_DATAGRAM_HOST_PORT);
    m_DatagramSocket->SetEnableBroadcast(TB_TRUE);

    m_ServerSocket = new TbServerSocket();
    m_ServerSocket->SetAcceptedCallback( TbCreateMemFunc(this,&Self::onAcceptedClient) );
    m_ServerSocket->Open(CR_REMOTE_INPUT_HOST_PORT,CR_PLAYER_MAX);

    m_BtServerSocket = new TbBtServerSocket();
    m_BtServerSocket->SetAcceptedCallback( TbCreateMemFunc(this,&Self::onAcceptedClient) );
    m_BtServerSocket->Open(CR_PLAYER_MAX);
#endif

    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ConvertTable); ++i ){
        m_ConvertTable[i] = i;
    }
}
    
/*!
 * デストラクタ
 * @author teco
 */
CrRemoteInputServer::~CrRemoteInputServer()
{
#if CR_REMOTE_INPUT_IS_ENABLE
    TB_SAFE_RELEASE(m_ServerSocket);
    TB_SAFE_RELEASE(m_BtServerSocket);
    TB_SAFE_RELEASE(m_DatagramSocket);
    TB_SAFE_RELEASE(m_DatagramSocket);
    TB_SAFE_RELEASE(m_NetworkManager);
#endif
}

/*!
 * トリガーしたか
 * @author teco
 */
TbBool CrRemoteInputServer::IsPress(TbUint32 buttonBit,const TbUint32 padIndex)
{
    if( !IsEnableInput() ){
        return TB_FALSE;
    }
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return (padInfo.correctNow & buttonBit) == buttonBit && (padInfo.correctOld & buttonBit) == 0;
    }
    return TB_FALSE;
}
    
/*!
 * 離したか
 * @author teco
 */
TbBool CrRemoteInputServer::IsRelease(TbUint32 buttonBit,const TbUint32 padIndex)
{
    if( !IsEnableInput() ){
        return TB_FALSE;
    }
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return (padInfo.correctNow & buttonBit) == 0 && (padInfo.correctOld & buttonBit) == buttonBit;
    }
    return TB_FALSE;
}
    
/*!
 * 押しているか
 * @author teco
 */
TbBool CrRemoteInputServer::IsOn(TbUint32 buttonBit,const TbUint32 padIndex)
{
    if( !IsEnableInput() ){
        return TB_FALSE;
    }
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return (padInfo.correctNow & buttonBit) == buttonBit;
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool CrRemoteInputServer::IsOnPre(TbUint32 buttonBit,const TbUint32 padIndex)
{
    if( !IsEnableInput() ){
        return TB_FALSE;
    }
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return (padInfo.correctOld & buttonBit) == buttonBit;
    }
    return TB_FALSE;
}

/*!
 * どれか押しているか
 * @author teco
 */
TbBool CrRemoteInputServer::IsOnAny(TbUint32 buttonBit,const TbUint32 padIndex)
{
    if( !IsEnableInput() ){
        return TB_FALSE;
    }
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return padInfo.correctNow & buttonBit;
    }
    return TB_FALSE;
}
    
/*!
 * 押しているか
 * @author teco
 */
TbBool CrRemoteInputServer::IsOnRepeat(TbUint32 buttonBit,const TbUint32 padIndex)
{
    if( !IsEnableInput() ){
        return TB_FALSE;
    }
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return padInfo.correctRepeat & buttonBit;
    }
    return TB_FALSE;
}

/*!
 * トリガーしたか
 * @author teco
 */
TbBool CrRemoteInputServer::IsPressRaw(TbUint32 buttonBit,const TbUint32 padIndex)
{
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return (padInfo.info.now & buttonBit) == buttonBit && (padInfo.info.old & buttonBit) == 0;
    }
    return TB_FALSE;
}

/*!
 * 押しているか
 * @author teco
 */
TbBool CrRemoteInputServer::IsOnRaw(TbUint32 buttonBit,const TbUint32 padIndex)
{
    TbUint32 inputIndex = getInputIndex(padIndex);
    if( inputIndex < m_InputInfo.GetCount() )
    {
        PadInfo& padInfo = m_InputInfo[inputIndex];
        return (padInfo.info.now & buttonBit) == buttonBit;
    }
    return TB_FALSE;
}

#if CR_REMOTE_INPUT_IS_ENABLE

/*!
 * パケット受信
 * @author teco
 */
void CrRemoteInputServer::onReceivedDatagramPacket( TbPacket& packet ,const char* fromAddress,TbSint32 port )
{
    TB_ASSERT(0 <= port);
    if( packet.main == TB_PACKET_MAIN_COMMON && 
        packet.sub == TB_PACKET_SUB_PING )
    {
        // PINGで返す
        TbPacketPing ping;
        m_DatagramSocket->Send(fromAddress,static_cast<TbUint16>(port),&ping);
    }
    else if( packet.main == CR_REMOTE_INPUT_PACKET_MAIN )
    {
        // UDPで入力受信
        TbUint32 id = TbCrc32::GetHash(fromAddress);
        onReceivedClientPacket( packet , id );
    }
}

/*!
 * 定期処理
 * @author teco
 */
void CrRemoteInputServer::update()
{
    TbForIterator( it , m_InputInfo ) 
    {
        TbPadInfo& padInfo = it->info;
        padInfo.repeat = 0;
        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(it->info.repeatMilliSec); ++i ) {
            if( padInfo.now & TB_BIT(i) ){
                padInfo.repeatMilliSec[i] += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds());
                TbFloat32 repeatTime = ( padInfo.repeatFirst & TB_BIT( i ) ) ? TbInputSetting::GetRepeatMilliSec() : TbInputSetting::GetRepeatMilliSecFirst();;
                if( !( padInfo.old & TB_BIT( i ) ) ){
                    padInfo.repeat |= TB_BIT( i );
                }else if( padInfo.repeatMilliSec[ i ] > repeatTime ){
                    padInfo.repeat |= TB_BIT( i );
                    padInfo.repeatFirst |= TB_BIT( i );
                    padInfo.repeatMilliSec[ i ] -= repeatTime;
                }
            }else{
                padInfo.repeatMilliSec[i] = 0.0f;
                padInfo.repeatFirst &= ~TB_BIT(i);
            }
        }
        it->info.old = it->info.now;
        it->correctRepeat = convertButtonBit(it->info.repeat);
        it->correctOld = convertButtonBit(it->info.old);
    }
}

/*!
 * クライアント受け入れ
 * @author teco
 */
void CrRemoteInputServer::onAcceptedClient( TbAcceptSocket* socket )
{
    PadInfo* padInfo = nullptr;
    
    // 新規登録
    TbUint64 id = TbCrc32::GetHash(socket->GetAddress());
    TbForIterator( it , m_InputInfo ) {
        if( !it->id == id ){
            // 保険
            padInfo = &(*it);
            break;
        }
        if( !it->isActive ){
            padInfo = &(*it);
            break;
        }
    }

    if( !padInfo && !m_InputInfo.IsFull() ){
        PadInfo info;
        m_InputInfo.PushBack(info);
        padInfo = &m_InputInfo.GetBack();
    }

    if( !padInfo ) {
        return;
    }
    
    padInfo->isActive = TB_TRUE;
    padInfo->id = id; ; // reinterpret_cast<TbUint64>(socket);
    padInfo->time = 0;

    socket->SetReceiveCallback( TbBind(&Self::onReceivedClientPacket,this,_1,padInfo->id) );
    socket->SetClosedCallback( TbCreateMemFunc(this,&Self::onClosedClient) );
}

/*!
 * パケット受信
 * @author teco
 */
void CrRemoteInputServer::onReceivedClientPacket( TbPacket& packet , TbUint64 id )
{
    if( packet.main != CR_REMOTE_INPUT_PACKET_MAIN ) {
        return;
    }

    switch( packet.sub )
    {
    case CR_REMOTE_INPUT_PACKET_SUB_INPUT1:
        {
            CrRemotePacketInput1& inputPacket = reinterpret_cast<CrRemotePacketInput1&>(packet);
            TbPrintf("(%d)inputPacket.key=%d\n",inputPacket.id,inputPacket.key);

            PadInfo* padInfo = nullptr;
            TbForIterator( it , m_InputInfo ) {
                if( it->isActive && it->id == id ){
                    padInfo = &(*it);
                    break;
                }
            }

            if( padInfo && ( padInfo->time < inputPacket.id ) )
            {
                // 最後に取得した値よりも大きなものを取得
                padInfo->info.now = inputPacket.key;
                padInfo->time = packet.id;
                padInfo->correctNow = convertButtonBit(padInfo->info.now);
            }
        }
        break;
    }
}

/*!
 * クライアント閉じた
 * @author teco
 */
void CrRemoteInputServer::onClosedClient( TbSocket* socket )
{
    TbPacketPing ping;
    auto func = TbBind(&Self::onReceivedClientPacket,this,_1,_2);
    func(ping,2);

    TbUint64 id = TbCrc32::GetHash(socket->GetAddress());
    TbForIterator( it , m_InputInfo ) {
        if( it->isActive && it->id == id ){
            it->isActive = TB_FALSE;
            it->id = 0;
            it->time = 0;
            it->info = TbPadInfo();
            it->correctNow = 0;
            it->correctRepeat = 0;
            it->correctOld = 0;
            break;
        }
    }
}
#endif

/*!
 * 入力インデックス取得
 * @author teco
 */
TbUint32 CrRemoteInputServer::getInputIndex( TbUint32 padIndex )
{
    TbSint32 index = padIndex;
    index -= TbMin<TbSint32>( CR_PLAYER_MAX , TbPad::GetPadCount() );
    if( index < 0 ) {
        index += CR_PLAYER_MAX;
    }
    return index;
}

/*!
 * コンフィグを考慮したボタンビット変換
 * @author teco
 */
TbUint32 CrRemoteInputServer::convertButtonBit( TbUint32 buttonBit )
{
    TbUint32 result = 0;
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ConvertTable); ++i ){
        if( buttonBit & TB_BIT(m_ConvertTable[i] ) ) {
            result |= TB_BIT(i);
        }
    }
    return result;
}

}
#endif
