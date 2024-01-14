/*!
 * 仮想入力クラス
 * @author Miyake Shunsuke
 * @since 2011.05.05
 */

#ifndef _INCLUDED_TB_VIRTUAL_INPUT_H_
#define _INCLUDED_TB_VIRTUAL_INPUT_H_

#include <base/math/tb_vector4.h>

namespace toybox
{

typedef TbPoolId TbVirtualInputId;
#define TB_VIRTUAL_INPUT_ID_INVALID TB_POOL_ID_INVALID;

/*!
 * 仮想入力
 */
class TbVirtualInput
{
public:

    // コンストラクタ
    TbVirtualInput()
        : m_BitArray()
    {}
    
    // デストラクタ
    ~TbVirtualInput(){}

private: // コピーと代入禁止

    TB_DISALLOW_COPY_AND_ASSIGN(TbVirtualInput);

public: // 外部向け関数

    // デバッグかどうか
    void SetDebug( TbBool isDebug ) {
        m_BitArray.SetBit( FLAG_DEBUG , isDebug ); 
    }

    // デバッグ用かどうか
    TbBool IsDebug() const { 
        return m_BitArray.Test( FLAG_DEBUG ); 
    }

    // F32型入力データ取得
    virtual TbFloat32 GetF32( TbUint32 index ) const { 
        return 0.0f; 
    }

    // S32型入力データ取得
    virtual TbSint32 GetS32( TbUint32 index ) const { 
        return 0; 
    }

    // ベクトル入力データ取得
    virtual TbVector4 GetVec( TbUint32 index ) const { 
        return TbVector4(); 
    } 

    // 押している(インデックス値から)
    virtual TbBool IsOnFromIndex( TbUint32 index ) const { 
        return TB_FALSE; 
    };

    // 押した(インデックス値から)
    virtual TbBool IsPressFromIndex( TbUint32 index ) const { 
        return TB_FALSE; 
    };

    // 離した(インデックス値から)
    virtual TbBool IsReleaseFromIndex( TbUint32 index ) const { 
        return TB_FALSE; 
    };

    // 押している(ビット値から)
    virtual TbBool IsOnFromBit( TbUint32 bit ) const { 
        return TB_FALSE;
    };

    // どれか押している(ビット値から)
    virtual TbBool IsOnAnyFromBit( TbUint32 bit ) const { 
        return TB_FALSE; 
    };

    // どれか押した(ビット値から)
    virtual TbBool IsPressAnyFromBit( TbUint32 bit ) const { 
        return TB_FALSE; 
    };

    // どれか離した(ビット値から)
    virtual TbBool IsReleaseAnyFromBit( TbUint32 bit ) const { 
        return TB_FALSE; 
    };

protected: // 派生クラス向け関数

    // 更新
    virtual void Update() = 0;

private:

    enum {
        FLAG_DEBUG ,
    };

private:

    TbBitArray32 m_BitArray;    // ビット配列

private:

    friend class TbVirtualInputManager;

};

}

#endif
