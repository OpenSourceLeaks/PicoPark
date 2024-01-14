/*!
 * バイナリ書き込み用便利バッファ
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */

#ifndef _INCLUDED_TB_BINARY_WRITE_BUFFER_H_
#define _INCLUDED_TB_BINARY_WRITE_BUFFER_H_

#include <base/container/tb_array.h>
#include <base/algorithm/tb_search.h>

namespace toybox
{

class TbBinaryWriteBuffer : private TbArray<TbUint8>
{
public:

    // コンストラクタ
    TbBinaryWriteBuffer( TbUint32 capacity ) {
        SetCapacity(capacity);
        SetEnableSetCapacityAuto(TB_FALSE); // 最大値は超えないように

        m_StringInfos.SetCapacity(200);
        m_StringInfos.SetEnableSetCapacityAuto(TB_TRUE);
    }

    // デストラクタ
    ~TbBinaryWriteBuffer() {}

public:

    // バッファ取得
    const void* GetBuffer() const {
        return &GetFront();
    }

    // バッファサイズ取得
    TbUint32 GetBufferSize() const {
        return GetCount();
    }

    // 文字列ポインタ解決
    void ResolveStringPtr()
    {
        TbAddr top = reinterpret_cast<TbAddr>(&GetFront());
        TbArray<StringInfo>::Iterator ite = m_StringInfos.Begin();
        TbArray<StringInfo>::Iterator endIte = m_StringInfos.End();
        for( ; ite != endIte; ++ite ) {
            TbUint32 bufSize = ite->str.GetBufferSize();
            char* buf = Add<char>(bufSize);
            TbStrCopy(buf,bufSize,ite->str.GetBuffer());

            TbAddr offset = getOffset(buf);
            TbArray<char**>::Iterator targetIte = ite->targetPtr.Begin();
            TbArray<char**>::Iterator targetEndIte = ite->targetPtr.End();
            for( ; targetIte != targetEndIte; ++targetIte ) {
                **targetIte = reinterpret_cast<char*>(offset);
            }
        }
        m_StringInfos.Clear();
    }

public:

    // 拡張
    template <typename T>
    T* Add( TbUint32 count = 1 , const T* data = NULL , T** targetVar = NULL ) {
        TbUint32 size = GetCount();
        TbUint32 incSize = TB_ALIGN_ROUND_UP(sizeof(T)*count,4);
        TB_VERIFY(PushBack(incSize,0));
        T* result = reinterpret_cast<T*>(&GetAt(size));
        if( data ) {
            TbMemCopy(result,data,sizeof(T)*count);
        }
        if( targetVar ){
            *targetVar = reinterpret_cast<T*>(getOffset(result));
        }
        return result;
    }
    
    // 文字列ポインター追加
    void AddStringPtr( const char* string , const char** targetVar ) {
        if( string && targetVar ){
            TbArray<StringInfo>::Iterator ite = TbFind(m_StringInfos,string);
            StringInfo* target = NULL;
            if( ite != m_StringInfos.End() ){
                target = &(*ite);
            }else{
                m_StringInfos.PushBack(StringInfo());
                target = &m_StringInfos.GetBack();
                target->str = string;
            }
            target->targetPtr.PushBack(const_cast<char**>(targetVar));
        }
    }

private:

    struct StringInfo
    {
        StringInfo() : str() , targetPtr() 
        {
            targetPtr.SetEnableSetCapacityAuto(TB_TRUE);
        }
        // 比較演算子
        TbBool operator==( const char* src ) {
            return str == src;
        }
        TbString str;
        TbArray<char**> targetPtr;
    };

    // オフセット取得
    TbAddr getOffset( void* ptr ){
        if( !TB_VERIFY(!IsEmpty()) ){
            return 0;
        }
        TbAddr ptrAddr(reinterpret_cast<TbAddr>(ptr));
        TbAddr top = reinterpret_cast<TbAddr>(&GetFront());
        if( !TB_VERIFY(top<=ptrAddr) ){
            return 0;
        }
        return ptrAddr - top;
    }

private:

    TbArray<StringInfo>     m_StringInfos;

};

}

#endif