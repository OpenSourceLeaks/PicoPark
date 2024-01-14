/*!
 * �X�v���C�g�n���[�e�B���e�B
 * @author teco
 */

#ifndef _INCLUDED_CMN_SPRITE_UTIL_H_
#define _INCLUDED_CMN_SPRITE_UTIL_H_

#include <graphics/sprite/tb_sprite.h>

class CmnSpriteBlink
{
public:
    
    // �R���X�g���N�^
    CmnSpriteBlink() 
        : m_IsOn(TB_FALSE)
        , m_Count(0)
        , m_Interval(0.1f)
        , m_Timer(0.0f)
        , m_Sprite(nullptr)
    {}

    // �f�X�g���N�^
    ~CmnSpriteBlink() {}

public:

    // �J�n
    void Start( TbSprite* sprite ) {
        m_Timer = 0.0f;
        m_Count = 0;
        m_Sprite = sprite;
        m_IsOn = TB_FALSE;
        m_Sprite->SetVisible(m_IsOn);
    }

    // �r�W�[��Ԃ�
    TbBool IsBusy() const {
        return m_Sprite != nullptr;
    }

public:

    // �������
    void Update( TbFloat32 deltatime )
    {
        if( !m_Sprite ) {
            return;
        }

        m_Timer += deltatime;
        if( m_Interval < m_Timer ) {
            if( !m_IsOn ) {
                ++m_Count;
                m_IsOn = TB_TRUE;
            }else{
                m_IsOn = TB_FALSE;
            }
            m_Sprite->SetVisible(m_IsOn);
            m_Timer = 0.0f;
            if( 4 <= m_Count ) {
                m_Sprite = nullptr;
            }
        }
    }

private:
    TbBool      m_IsOn;
    TbUint32    m_Count;
    TbFloat32   m_Interval;
    TbFloat32   m_Timer;
    TbSprite*   m_Sprite;
};

#endif
