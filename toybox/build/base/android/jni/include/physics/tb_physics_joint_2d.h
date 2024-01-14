/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 * @since 2011.05.06
 */
 
#ifndef _INCLUDED_TB_PHYSICS_JOINT_2D_H_
#define _INCLUDED_TB_PHYSICS_JOINT_2D_H_

#include <physics/tb_physics_types.h>
#include <base/math/tb_vector2.h>
#include <base/util/tb_ref_counter.h>
#include "./depend/tb_physics_joint_2d_depend.h"

namespace toybox
{

class TbPhysicsBody2d;
class TbPhysicsScene2d;

class TbPhysicsJoint2d
{
public:

    // コンストラクタ
    TbPhysicsJoint2d();

    // デストラクタ
    virtual ~TbPhysicsJoint2d();

private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbPhysicsJoint2d);

public:

    // 後始末
    void Finalize() {
        Destroy();
        ResetBody();
    }

    // 適用
    virtual void Create( TbPhysicsScene2d* scene ) = 0;

    // 破棄
    virtual void Destroy() = 0;

    // 依存するアクターからの破棄通知
    virtual void NotifyDestroy();

    // 生成済み
    TbBool IsCreated() const;

    // 接続したモノ同士の衝突をするか
    void SetCollideConnected( TbBool isConnected ) {
        m_BitArray.SetBit(FLAG_COLLIDE_CONNECTED);
    }

    // 接続したモノ同士の衝突をするか
    TbBool IsCollideConnected() const {
        return m_BitArray.Test(FLAG_COLLIDE_CONNECTED);
    }

public:

    // 依存型取得
    TbPhysicsJoint2dDepend& GetDepend() {
        return m_Depend;
    }

    // 依存型取得
    const TbPhysicsJoint2dDepend& GetDepend() const {
        return m_Depend;
    }

protected:

    // 依存ボディー追加
    void SetBody( TbUint32 index , TbPhysicsBody2d* body );

    // 依存ボディー取得
    TbPhysicsBody2d* GetBody( TbUint32 index ) {
        if( index < TB_ARRAY_COUNT_OF(m_Bodys) ){
            return m_Bodys[index];
        }
        return nullptr;
    }

    // 依存アクター除去
    void ResetBody();

private:

    enum Flag
    {
        FLAG_COLLIDE_CONNECTED ,
        FLAG_MAX
    };

    static const TbSint32 BODY_MAX = 2;

private:
    TbPhysicsBody2d*        m_Bodys[BODY_MAX];
    TbBitArray32            m_BitArray;
    TbPhysicsJoint2dDepend  m_Depend;
};

// 回転ジョイント
class TbPhysicsRevoluteJoint2d : public TbPhysicsJoint2d
{
    typedef TbPhysicsJoint2d Super;
public:

    // コンストラクタ
    TbPhysicsRevoluteJoint2d();

    // デストラクタ
    ~TbPhysicsRevoluteJoint2d();

public:

    // 初期化
    void Initialize( TbPhysicsBody2d* actor1 , TbPhysicsBody2d* actor2 );

    // 制限
    void SetLimit( TbFloat32 minRad , TbFloat32 maxRad );

    // アンカー
    void SetAnchor( const TbVector2& pos1 , const TbVector2& pos2 );

public:

    // 適用
    virtual void Create( TbPhysicsScene2d* scene );

    // 破棄
    virtual void Destroy();

private:

    TbPhysicsRevoluteJoint2dDepend  m_Depend;

};

// ジョイント同士を連動させる
class TbPhysicsGearJoint2d : public TbPhysicsJoint2d
{
    typedef TbPhysicsJoint2d Super;
public:

    // コンストラクタ
    TbPhysicsGearJoint2d();

    // デストラクタ
    ~TbPhysicsGearJoint2d();

public:

    // 初期化
    void Initialize( TbPhysicsBody2d* actor1 , TbPhysicsBody2d* actor2 ,
                     TbPhysicsJoint2d* joint1 , TbPhysicsJoint2d* joint2 );

    // 適用率設定
    void SetRatio( TbFloat32 ratio );

public:

    // 適用
    virtual void Create( TbPhysicsScene2d* scene );

    // 破棄
    virtual void Destroy();

private:

    TbPhysicsJoint2d*           m_Joints[2];
    TbPhysicsGearJoint2dDepend  m_Depend;
};

}

#endif
