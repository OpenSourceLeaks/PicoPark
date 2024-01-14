/*!
 * 多数決用入力
 * @author teco
 * @since 2011.09.14
 */

#include "fwd.h"
#include "sys_majority_input.h"
    
static const TbFloat32 TIME_LIMIT = 1.0f;

/*! 
 * コンストラクタ
 * @author teco
 * @since 2013.10.26
 */
SysMajorityInput::SysMajorityInput()
    : m_Now()
    , m_Old()
    , m_InputCount(0)
    , m_InputRate(0.0f)
{
    TbFill(m_CurrentRate,0.0f);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.10.26
 */
SysMajorityInput::~SysMajorityInput()
{
}
        
/*! 
 * 初期化
 * @author teco
 * @since 2013.10.26
 */
void SysMajorityInput::Initialize( TbUint32 count , TbFloat32 rate )
{
    m_InputParam.Clear();
    m_InputParam.SetCapacity(count);
    m_InputParam.PushBack(count,InputParam());
    m_InputCount = count;
    m_InputRate = rate;
    m_Now.Clear();
    m_Old.Clear();
}

/*!
 * 更新
 * @author teco
 * @since 2013.10.26
 */
void SysMajorityInput::Update( TbFloat32 deltatime )
{
    m_Old = m_Now;
    m_Now.Clear();
    for( TbSint32 i = 0; i < SysInput::BUTTON_MAX; ++i ){
        update( static_cast<SysInput::Button>(i) , deltatime );
    }
}

/*!
 * ボタンを押しているか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysMajorityInput::IsOn( SysInput::Button button  ) const
{
    return m_Now.Test(button);
}

/*! 
 * ボタンを押したか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysMajorityInput::IsPress( SysInput::Button button ) const
{
    return (m_Now.Test(button) & !m_Old.Test(button));
}

/*!
 * ボタンを話したか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysMajorityInput::IsRelease( SysInput::Button button ) const
{
    return (!m_Now.Test(button) & m_Old.Test(button));
}

/*!
 * 入力更新
 * @author teco
 * @since 2013.10.26
 */
void SysMajorityInput::update( SysInput::Button button , TbFloat32 deltatime )
{
    TbUint32 count = 0;
    for( TbUint32 i = 0; i < m_InputCount; ++i ) {
        if( SysInput::IsOn(button,i) ){
#if 0 // 有効時間制は評判悪いので廃止
            if( m_Old.Test(button) ){
                ++count;
            }else if( m_InputParam[i].time[button] < TIME_LIMIT ) { // 入力して一定時間以内なら有効
                ++count;
            }
            m_InputParam[i].time[button] += deltatime;
#else
            ++count;
#endif
        }else{
            m_InputParam[i].time[button] = 0.0f;
        }
    }

    TbFloat32 rate = static_cast<TbFloat32>(count)/static_cast<TbFloat32>(m_InputCount);
    if( m_Old.Test(button) ){
        TbFloat32 border = m_InputRate/2.0f;
        // 一定以上離さないと離したことにはならない
        if( (border) <= rate ) {
            m_Now.SetBit(button);
        }
        m_CurrentRate[button] = TbMin( 1.0f , (rate-border)/(m_InputRate-border) );
    }else{
        // 一定以上押していると押すこととする
        if( m_InputRate <= rate ) {
            m_Now.SetBit(button);
        }
        m_CurrentRate[button] = TbMin( 1.0f , rate/m_InputRate );
    }
}

