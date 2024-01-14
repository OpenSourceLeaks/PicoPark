/*!
 * Windowの依存部分
  * @author teco
 */
 
#ifndef _INCLUDED_TB_WINDOW_IOS_H_
#define _INCLUDED_TB_WINDOW_IOS_H_

namespace toybox
{

class TbWindow;
class TbWindowDependImpl;

/*!
 * @class
 */
class TbWindowDepend
{
public:

    // コンストラクタ
    TbWindowDepend();

    // デストラクタ
    ~TbWindowDepend();
    
public:
    
    // 実装部取得
    TbWindowDependImpl* GetImpl() {
        return m_Impl;
    }
    
    // 実装部取得
    const TbWindowDependImpl* GetImpl() const {
        return m_Impl;
    }

private:
    
    TbWindowDependImpl* m_Impl;

};

}

#endif
