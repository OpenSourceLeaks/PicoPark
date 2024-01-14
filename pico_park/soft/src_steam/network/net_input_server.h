/*!
 * 入力サーバー
 * @author teco
 */

#ifndef _INCLUDED_NET_INPUT_SERVER_H_
#define _INCLUDED_NET_INPUT_SERVER_H_

#if TB_PLATFORM_IS_WIN

#include <network/tb_network_manager.h>
#include <network/tb_network_all.h>
#include <input/tb_pad.h>

#define NET_INPUT_ENABLE (1)
#else
#define NET_INPUT_ENABLE (0)
#endif

#if NET_INPUT_ENABLE
class NetInputServer : public TbTask , public TbSingletonRef<NetInputServer>
{
    typedef NetInputServer Self;
public:
    
    // コンストラクタ
    NetInputServer();
    
    // デストラクタ
    ~NetInputServer();

public:

    // 入力可能設定
    void SetEnableInput( TbBool isEnable ) {
        m_IsEnableInput = isEnable;
    }

    // 入力可能設定
    TbBool IsEnableInput() const {
        return m_IsEnableInput;
    }

    // パッド数取得
    TbUint32 GetPadCount() const {
        return m_InputInfo.GetCount();
    }

    //トリガーしたか
    TbBool IsPress(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //離したか
    TbBool IsRelease(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //押しているか
    TbBool IsOn(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //押しているか
    TbBool IsOnPre(TbUint32 buttonBit,const TbUint32 padIndex=0);

    //どれか押しているか
    TbBool IsOnAny(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //押しているか
    TbBool IsOnRepeat(TbUint32 buttonBit,const TbUint32 padIndex=0);

    // パッドボタンセッティング
    void SetPadButtonSetting( TbUint32 button , TbUint32 rawButton ) {
        if( button < TB_ARRAY_COUNT_OF(m_ConvertTable) ){
            m_ConvertTable[button] = rawButton;
        }
    }

    // パッドボタンセッティング
    TbUint32 GetPadButtonSetting( TbUint32 button ) const {
        if( button < TB_ARRAY_COUNT_OF(m_ConvertTable) ){
            return m_ConvertTable[button];
        }
        return 0;
    }

    // パッドボタンセッティング
    void ResetPadButtonSetting() {
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ConvertTable); ++i ){
            m_ConvertTable[i] = i;
        }
    }

public:

    //トリガーしたか
    TbBool IsPressRaw(const TbUint32 buttonBit ,const TbUint32 padIndex=0);

    //押しているか
    TbBool IsOnRaw(const TbUint32 buttonBit ,const TbUint32 padIndex=0);

private:

    struct PadInfo
    {
        PadInfo() : isActive(TB_FALSE) , correctNow(0) , correctRepeat(0) , correctOld(0) , id(0) , time(0) {}
        TbBool            isActive;
        toybox::TbPadInfo info;
        TbUint32          correctNow;  // 補正現在の値
        TbUint32          correctRepeat;
        TbUint32          correctOld;  // 補材１フレーム前の入力
        TbUint64          id;
        TbUint64          time;
    };

private:

    // 定期更新
    void update();

    // パケット受信(Datagram)
    void onReceivedDatagramPacket( TbPacket& packet ,const char* fromAddress,TbSint32 port );

    // クライアント受け入れ
    void onAcceptedClient( TbAcceptSocket* socket );

    // パケット受信
    void onReceivedClientPacket( TbPacket& packet , TbUint64 id );

    // クライアント閉じた
    void onClosedClient( TbSocket* socket );

    // 入力インデックス取得
    TbUint32 getInputIndex( TbUint32 padIndex );

    // コンフィグを考慮したボタンビット変換
    TbUint32 convertButtonBit( TbUint32 buttonBit );

private:

    TbBool                      m_IsEnableInput;
    toybox::TbNetworkManager*   m_NetworkManager;

private: 

    toybox::TbDatagramSocket*   m_DatagramSocket; // ブロードキャスト受信用
    toybox::TbServerSocket*     m_ServerSocket;   // サーバーソケット
    toybox::TbBtServerSocket*   m_BtServerSocket; // Bluetooth用サーバー

private:

    TbStaticArray<PadInfo,CMN_PLAYER_MAX> m_InputInfo;
    TbUint32                              m_ConvertTable[TB_PAD_BUTTON_MAX];

};

#endif

#endif
