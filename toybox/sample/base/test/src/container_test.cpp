/*!
 * 
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#include "fwd.h"
#include "container_test.h"

#include <base/time/tb_tick.h>
#include <base/container/tb_pool.h>
#include <base/util/tb_type_traits.h>
#include <base/container/tb_assoc_array.h>
#include <base/container/tb_str_assoc_array.h>
#include <base/math/tb_random.h>

#pragma warning(disable:4800)
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>

namespace
{

using namespace toybox;

TbFloat64 s_Time1 = 0;
TbFloat64 s_Time2 = 0;
TbFloat64 s_Time3 = 0;

struct Hasher
{
    size_t operator() ( const toybox::TbStringKey<toybox::TbStaticString32>& key ) const {
        return key.GetHash();
    }
};

void init( toybox::TbStr32AssocArray<int>& assocContainer , 
           std::map<toybox::TbStringKey<toybox::TbStaticString32>,int>& mapVal ,
           std::unordered_map<toybox::TbStringKey<toybox::TbStaticString32>,int,Hasher>& hashMapVal )
{
    for( TbSint32 i = 0; i < 10000; ++i ){
        TbSint32 value = TbRandom(10000);
        TbStringKey<TbStaticString32> key( TbStaticString64::Format("aaa%d",value).GetBuffer() );
        assocContainer.Insert( key , value );
        mapVal.insert( std::pair<TbStringKey<TbStaticString32>,int>( key , value ) );
        hashMapVal.insert( std::pair<TbStringKey<TbStaticString32>,int>( key , value ) );
    }
}

TbSint32 getIndex()
{
    static TbSint32 result = 0;
    result = (result + 1) % 6;
    return result;
}

}

/*!
 * コンテナテスト
 * @author Miyake Shunsuke
 * @since 2012.12.8
 */
void TestContainer()
{
    TbInitRandom(105);
    TbShuffleRandom();

    TbStr32AssocArray<int> test( 1000 );
    std::map<TbStringKey<TbStaticString32>,int> test2;
    std::unordered_map<TbStringKey<TbStaticString32>,int,Hasher> test3;
    init( test , test2 , test3 );
    int size = test2.size();

    TbStringKey<TbStaticString32> key1("aaa111");
    TbStringKey<TbStaticString32> key2("aaa311");
    TbStringKey<TbStaticString32> key3("aaa111");
    TbStringKey<TbStaticString32> key4("aaa501");
    TbStringKey<TbStaticString32> key5("aaa401");
    TbStringKey<TbStaticString32> key6("aaa11");
    TbStringKey<TbStaticString32> keyList[6] = {
         key1 , key2 , key3 , key4 , key5 , key6
    };

    int result = 0;
    TbStr32AssocArray<int>::Iterator endIte = test.End();
    TbFloat64 first = TbTick::GetMilliSeconds();
    for( TbSint32 cnt = 0; cnt < 10000; ++cnt ){
        TbStr32AssocArray<int>::Iterator ite = test.Find( keyList[ getIndex() ] );
        if( ite != endIte ){
            result += (*ite).value;
        }
    }
    s_Time1 = TbTick::GetMilliSeconds() - first;
    TbPrintf( "result=%d:%f\n" , result , s_Time1 ); 

    result = 0;
    std::map<TbStringKey<TbStaticString32>,int>::iterator endIteMap = test2.end();
    TbFloat64 second = TbTick::GetMilliSeconds();
    for( TbSint32 cnt = 0; cnt < 10000; ++cnt ){
        std::map<TbStringKey<TbStaticString32>,int>::iterator ite = test2.find( keyList[ getIndex() ] );
        if( ite != endIteMap ){
            result += (*ite).second;
        }
    }
    s_Time2 = TbTick::GetMilliSeconds() - second;
    TbPrintf( "result=%d:%f\n" , result , s_Time2 ); 
}
