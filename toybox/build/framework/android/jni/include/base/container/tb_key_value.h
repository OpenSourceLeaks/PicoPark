/*!
 * KeyValueの構造体
 * @author Miyake_Shunsuke
 * @since 2012.06.12
 */

#ifndef _INCLUDED_TB_KEY_VALUE_H_
#define _INCLUDED_TB_KEY_VALUE_H_

namespace toybox
{

template <typename Key,typename Value>
class TbKeyValue
{
public:
    typedef Key KeyType;
    typedef Value ValueType;
public:

    // コンストラクタ
    TbKeyValue( const Key& inKey , const Value& inValue  )
        : key(inKey) , value(inValue)
    {}

    // コピーコンストラクタ
    TbKeyValue( const TbKeyValue& keyValue )
        : key(keyValue.key) , value(keyValue.value)
    {}

public:
    Key   key;
    Value value;
};

};

#endif
