/*!
 * アクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTOR_H_
#define _INCLUDED_ACTOR_H_

#include <base/actor/tb_actor.h>
#include <base/math/tb_geometry.h>
#include <base/util/tb_dynamic_cast.h>

class ActorFactory;
class Stage;

class Actor : public toybox::TbActor
{
    friend class ActorFactory;

public:

    // コンストラクタ
    Actor()
    {}

public:

    TB_DEFINE_BASE_CLASS(Actor);

public:

    // ステージ設定
    void SetStage( Stage* stage ) {
        m_Stage = stage;
    }

    // ステージ取得
    Stage* GetStage() {
        return m_Stage;
    }

    // ステージ取得
    Stage* GetStage() const {
        return m_Stage;
    }

private:

    TbBitArray32    m_BitArray;         // ビット配列
    Stage*          m_Stage;            // 所属ステージ

};

#endif
