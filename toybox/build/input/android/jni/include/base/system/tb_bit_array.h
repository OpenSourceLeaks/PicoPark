/*!
 * @file
 * @brief フラグ用ビット配列クラス
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_BIT_ARRAY_H_
#define _INCLUDED_TB_BIT_ARRAY_H_

namespace toybox
{

template<typename T>
class TbBitArray
{
public:

    //コンストラクタ
    TbBitArray(const T bitArray = 0):m_BitArray(bitArray){}

    //デストラクタ
    ~TbBitArray(void){};

public: // 演算子

    // 添字演算子
    TbBool operator[]( const T bit ) {
        return Test(bit);
    }

    // ==比較演算子
    TbBool operator==( const TbBitArray<T>& bitArray ){
        return m_BitArray == bitArray.m_BitArray;
    }

    // !=比較演算子
    TbBool operator!=( const TbBitArray<T>& bitArray ){
        return m_BitArray != bitArray.m_BitArray;
    }

    // &ビット演算
    T operator&( T bit ) const {
        return m_BitArray & bit;
    }

    // |ビット演算
    T operator|( T bit ) const {
        return m_BitArray | bit;
    }

    //! &=ビット演算
    T operator&=( T bit ) {
        m_BitArray &= bit;
        return m_BitArray;
    }

    //! |=ビット演算
    T operator|=( T bit ) {
        m_BitArray |= bit;
        return m_BitArray;
    }

public:

    //! クリア
    void Clear() {
        m_BitArray = 0;
    }

    // 空かどうか
    TbBool IsEmpty() const {
        return (0 == m_BitArray);
    }

    /*!
     * ビット設定
     * @paramb bit ビット番号
     */
    void SetBit(const T bit,const TbBool flag = TB_TRUE )
    {
        // チェック
        if( !TB_VERIFY( bit < sizeof(T)*8 ) ){
            return;
        }
        if(flag){
            m_BitArray |= (static_cast<T>(1) << bit);
        }else{
            m_BitArray &= ~(static_cast<T>(1) << bit);
        }
    }
    
    /*!
     * ビットリセット
     * @paramb bit ビット番号
     */
    void ResetBit(const T bit )
    {
        // チェック
        if( !TB_VERIFY( bit < sizeof(T)*8 ) ){
            return;
        }
        m_BitArray &= ~(static_cast<T>(1) << bit);
    }

    /*!
     * ビットチェック
     */
    TbBool  Test(const T bit) const{return (m_BitArray & (static_cast<T>(1) << bit)) != 0;}
    
private:
    T m_BitArray;
};

typedef TbBitArray<TbUint8>     TbBitArray8;
typedef TbBitArray<TbUint16>    TbBitArray16;
typedef TbBitArray<TbUint32>    TbBitArray32;
typedef TbBitArray<TbUint64>    TbBitArray64;

}

#endif