/*!
 * アクター
 * @author teco
 */

#ifndef _INCLUDED_CR_ACTOR_H_
#define _INCLUDED_CR_ACTOR_H_

#include <base/actor/tb_actor.h>
#include <base/math/tb_geometry.h>
#include <base/util/tb_dynamic_cast.h>
#include <crayon/actor/cr_actor_factory.h>

namespace crayon
{

class CrStage;

class CrActor : public toybox::TbActor
{
    friend class CrActorFactory;
    typedef CrActor         Self;
    typedef toybox::TbActor Super;

public:

    // コンストラクタ
    CrActor()
        : m_CreateThreshold(-1.0f)
    {}

public:

    TB_DEFINE_SUB_CLASS(Self,Super)

public:

    // ステージ設定
    void SetStage( CrStage* stage ) {
        m_Stage = stage;
    }

    // ステージ取得
    CrStage* GetStage() {
        return m_Stage;
    }

    // ステージ取得
    CrStage* GetStage() const {
        return m_Stage;
    }

    // 生成閾値
    void SetCreateThreshold(TbFloat32 threshold) {
        m_CreateThreshold = threshold;
    }

    // 生成閾値
    TbFloat32 GetCreateThreshold() const {
        return m_CreateThreshold;
    }

public:

    // セットアップ
    virtual void Setup( const CrActorFactory::UserData& userData ) {}

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param )
    { 
        return 0; 
    }


private:

    toybox::TbBitArray32    m_BitArray;         // ビット配列
    CrStage*                m_Stage;            // 所属ステージ
    TbFloat32               m_CreateThreshold;  // 生成タイミング

};

}

#endif
