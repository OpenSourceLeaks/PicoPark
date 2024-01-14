/*!
 * @file
 * @brief Dx9用デバイスロスト時に消滅してしまうリソース
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */
 
#ifndef _INCLUDED_TB_LOST_RESOURCE_DX9_H_
#define _INCLUDED_TB_LOST_RESOURCE_DX9_H_

namespace toybox
{

/*!
 * ロストリソース( nullptr )
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
class TbLostResourceDx9
{
public:

    //! コンストラクタ( Root用 )
    TbLostResourceDx9() 
        : m_Prev( nullptr )
        , m_Next( nullptr )
    {
        m_Prev = this;
        m_Next = this;
    }
    
    //! コンストラクタ( Rootに接続 )
    TbLostResourceDx9( TbLostResourceDx9* insert , TbBool canConnectPrev = TB_TRUE ) 
        : m_Prev( nullptr ) 
        , m_Next( nullptr ) 
    {
        if( insert ){
            insert->connect( *this , canConnectPrev );
        }
    }
    
    //! デストラクタ
    virtual ~TbLostResourceDx9()
    {
        // チェーンから除去
        if( m_Prev ){
            m_Prev->m_Next = m_Next;
        }
        if( m_Next ){
            m_Next->m_Prev = m_Prev;
        }
    }
    
public: // 公開関数

    //! 前取得
    TbLostResourceDx9* GetPrev() { return m_Prev; }

    //! 次取得
    TbLostResourceDx9* GetNext() { return m_Next; }
    
    // 解放
    virtual void Release() {}

    // 再構築
    virtual void Recover() {}

private: // 非公開関数

    //! チェーンに繋げる(リングチェーン)
    void connect( TbLostResourceDx9& resource , TbBool canConnectPrev )
    {
        if( canConnectPrev ){
            resource.m_Prev = m_Prev;
            resource.m_Next = this;
            m_Prev->m_Next = &resource;
            m_Prev = &resource;
        }else{
            resource.m_Prev = this;
            resource.m_Next = m_Next;
            m_Next->m_Prev = &resource;
            m_Next = &resource;
        }
    }
    
private:
    TbLostResourceDx9* m_Prev;             //!< 前
    TbLostResourceDx9* m_Next;             //!< 次
};

}

#endif
