/*!
 * PMX用骨格クラス
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#ifndef _INCLUDED_TB_SKELEETON_PMX_H_
#define _INCLUDED_TB_SKELEETON_PMX_H_

#include <graphics/model/tb_model_data_pmx.h>

namespace toybox
{

class TbSkeletonPmx
{
public:

    // コンストラクタ
    TbSkeletonPmx() : m_Data(nullptr) {}

public:

    // データ設定
    void SetData( TbModelDataPmx::BoneContainer* data ) {
        m_Data = data;
    }
    
    // 骨数取得
    TbUint32 GetBoneCount() const {
        return m_Data ? m_Data->count : 0;
    }

    // 骨取得
    const TbModelDataPmx::BoneInfo* GetBone( TbUint32 index ) const {
        if( index < GetBoneCount() ) {
            return &m_Data->list[index];
        }
        return nullptr;
    }

private:
    TbModelDataPmx::BoneContainer* m_Data;
};

}

#endif
