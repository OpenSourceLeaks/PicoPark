/*!
 * ���̓T�[�o�[
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
    
    // �R���X�g���N�^
    NetInputServer();
    
    // �f�X�g���N�^
    ~NetInputServer();

public:

    // ���͉\�ݒ�
    void SetEnableInput( TbBool isEnable ) {
        m_IsEnableInput = isEnable;
    }

    // ���͉\�ݒ�
    TbBool IsEnableInput() const {
        return m_IsEnableInput;
    }

    // �p�b�h���擾
    TbUint32 GetPadCount() const {
        return m_InputInfo.GetCount();
    }

    //�g���K�[������
    TbBool IsPress(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //��������
    TbBool IsRelease(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //�����Ă��邩
    TbBool IsOn(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //�����Ă��邩
    TbBool IsOnPre(TbUint32 buttonBit,const TbUint32 padIndex=0);

    //�ǂꂩ�����Ă��邩
    TbBool IsOnAny(TbUint32 buttonBit,const TbUint32 padIndex=0);
    
    //�����Ă��邩
    TbBool IsOnRepeat(TbUint32 buttonBit,const TbUint32 padIndex=0);

    // �p�b�h�{�^���Z�b�e�B���O
    void SetPadButtonSetting( TbUint32 button , TbUint32 rawButton ) {
        if( button < TB_ARRAY_COUNT_OF(m_ConvertTable) ){
            m_ConvertTable[button] = rawButton;
        }
    }

    // �p�b�h�{�^���Z�b�e�B���O
    TbUint32 GetPadButtonSetting( TbUint32 button ) const {
        if( button < TB_ARRAY_COUNT_OF(m_ConvertTable) ){
            return m_ConvertTable[button];
        }
        return 0;
    }

    // �p�b�h�{�^���Z�b�e�B���O
    void ResetPadButtonSetting() {
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ConvertTable); ++i ){
            m_ConvertTable[i] = i;
        }
    }

public:

    //�g���K�[������
    TbBool IsPressRaw(const TbUint32 buttonBit ,const TbUint32 padIndex=0);

    //�����Ă��邩
    TbBool IsOnRaw(const TbUint32 buttonBit ,const TbUint32 padIndex=0);

private:

    struct PadInfo
    {
        PadInfo() : isActive(TB_FALSE) , correctNow(0) , correctRepeat(0) , correctOld(0) , id(0) , time(0) {}
        TbBool            isActive;
        toybox::TbPadInfo info;
        TbUint32          correctNow;  // �␳���݂̒l
        TbUint32          correctRepeat;
        TbUint32          correctOld;  // ��ނP�t���[���O�̓���
        TbUint64          id;
        TbUint64          time;
    };

private:

    // ����X�V
    void update();

    // �p�P�b�g��M(Datagram)
    void onReceivedDatagramPacket( TbPacket& packet ,const char* fromAddress,TbSint32 port );

    // �N���C�A���g�󂯓���
    void onAcceptedClient( TbAcceptSocket* socket );

    // �p�P�b�g��M
    void onReceivedClientPacket( TbPacket& packet , TbUint64 id );

    // �N���C�A���g����
    void onClosedClient( TbSocket* socket );

    // ���̓C���f�b�N�X�擾
    TbUint32 getInputIndex( TbUint32 padIndex );

    // �R���t�B�O���l�������{�^���r�b�g�ϊ�
    TbUint32 convertButtonBit( TbUint32 buttonBit );

private:

    TbBool                      m_IsEnableInput;
    toybox::TbNetworkManager*   m_NetworkManager;

private: 

    toybox::TbDatagramSocket*   m_DatagramSocket; // �u���[�h�L���X�g��M�p
    toybox::TbServerSocket*     m_ServerSocket;   // �T�[�o�[�\�P�b�g
    toybox::TbBtServerSocket*   m_BtServerSocket; // Bluetooth�p�T�[�o�[

private:

    TbStaticArray<PadInfo,CMN_PLAYER_MAX> m_InputInfo;
    TbUint32                              m_ConvertTable[TB_PAD_BUTTON_MAX];

};

#endif

#endif
