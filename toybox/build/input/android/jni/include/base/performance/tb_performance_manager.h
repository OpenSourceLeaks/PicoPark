/*!
 * @file
 * @brief パフォーマンス管理
 */

#ifndef _INCLUDED_TB_PERFORMANCE_MANAGER_H_
#define _INCLUDED_TB_PERFORMANCE_MANAGER_H_

#include <base/util/tb_singleton.h>

namespace toybox
{

class TbPerforcemanceManger : public TbSingletonRef<TbPerforcemanceManger>
{
public:

    static const TbUint32 INFO_MAX = 256;

public:

    // コンストラクタ
    TbPerforcemanceManger();

    // デストラクタ
    ~TbPerforcemanceManger();

private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbPerforcemanceManger);

public:

    // 名前設定
    void SetName( TbUint32 index , const char* name );

    // 計測
    void Begin( TbUint32 index );

    // 終了
    void End( TbUint32 index);

private:

    struct Info
    {
        Info() : result(0.0f) , beginMsec(0.0f) {}
        TbFloat32        result;
        TbFloat32        beginMsec;
        TbStaticString64 name;
    };

private:

    Info m_Info[INFO_MAX];

};

}

#endif