/*!
 * @file
 * @brief ビット入力
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_BIT_INPUT_H_
#define _INCLDUED_TB_BIT_INPUT_H_

#include <input/tb_input_setting.h>

namespace toybox
{

template <typename T>
class TbBitInput
{
public:

    // コンストラクタ
    TbBitInput() {
        TbFill(m_Timer,0.0f);
        TbFill(m_RepeatTimer,0.0f);
    }

public:

    // 更新
    void Update( TbFloat32 deltatime ) {
        m_RepeatBitArray.Clear();
        m_OldBitArray = m_NowBitArray;
        OnUpdate(deltatime);
        m_NowBitArray = m_FutureBitArray;
        for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Timer); ++i ){
            T bit = TB_BIT(i);
            if( IsOn(bit) ){
                m_RepeatTimer[i] += deltatime;
                TbFloat32 repeatTime = (m_RepeatFirstBitArray&bit) ? TbInputSetting::GetRepeatSec() : TbInputSetting::GetRepeatSecFirst();
                if( repeatTime <= m_RepeatTimer[i] || IsPress(bit) ) {
                    m_RepeatBitArray |= bit;
                    m_RepeatFirstBitArray |= bit;
                    m_RepeatTimer[i] = 0.0f;
                }
            }else{
                m_RepeatBitArray &= ~bit;
                m_RepeatFirstBitArray &= ~bit;
                m_RepeatTimer[i] = 0.0f;
            }
            m_Timer[i] += deltatime;
        }
    }

    //トリガーしたか
    TbBool IsPress(const T bit ) const {
        return ((m_NowBitArray&bit)==bit) && ((m_OldBitArray&bit)==0);
    }
    
    //離したか
    TbBool IsRelease(const T bit ) const {
        return ((m_NowBitArray&bit)==0) && ((m_OldBitArray&bit)==bit);
    }
    
    //押しているか
    TbBool IsOn(const T bit) const {
        return ((m_NowBitArray&bit)==bit);
    }

    //押しているか
    TbBool IsOnRepeat(const T bit) const {
        return ((m_RepeatBitArray&bit)==bit);
    }

    //どれか押しているか
    TbBool IsOnAny(const T bit) const;

    //押し続けている時間取得
    TbFloat32 GetPressTime(const TbUint32 index) const {
        if( IsOn(TB_BIT(index)) ){
            return m_Timer[index];
        }
        return 0;
    }

    //話し続けている時間取得
    TbFloat32 GetReleaseTime(const TbUint32 index) const {
        if( !IsOn(TB_BIT(index)) ){
            return m_Timer[index];
        }
        return 0.0f;
    }

protected:

    // 更新
    virtual void OnUpdate( TbFloat32 deltatime ) = 0;

    // 押す
    void Press( TbUint32 index ) {
        m_FutureBitArray.SetBit(index);
        if( index < TB_ARRAY_COUNT_OF(m_Timer) ){
            m_Timer[index] = 0.0f;
        }
    }

    // 離す
    void Release( TbUint32 index ) {
        m_FutureBitArray.ResetBit(index);
        if( index < TB_ARRAY_COUNT_OF(m_Timer) ){
            m_Timer[index] = 0.0f;
        }
    }

    // 入力を強制リセット
    void Reset( TbUint32 index ) {
        m_NowBitArray.ResetBit(index);
        m_OldBitArray.ResetBit(index);
        m_FutureBitArray.ResetBit(index);
        m_RepeatFirstBitArray.ResetBit(index);
        m_RepeatBitArray.ResetBit(index);
        if( index < TB_ARRAY_COUNT_OF(m_Timer) ){
            m_Timer[index] = 0.0f;
        }
    }

    // 強制的にタイマーリセット
    void ResetTimer( const TbUint32 index ) {
        if( index < TB_ARRAY_COUNT_OF(m_Timer) ){
            m_Timer[index] = 0.0f;
        }
    }

private:

    TbBitArray<T> m_NowBitArray;
    TbBitArray<T> m_FutureBitArray;
    TbBitArray<T> m_OldBitArray;
    TbBitArray<T> m_RepeatFirstBitArray;
    TbBitArray<T> m_RepeatBitArray;
    TbFloat32     m_Timer[sizeof(T)*8];
    TbFloat32     m_RepeatTimer[sizeof(T)*8];

};

typedef TbBitInput<TbUint32> TbBitInput32;

}

#endif