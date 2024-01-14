/*!
 * 物理アクター
 * @author Miyake Shunsuke
 * @since 2015.03.06
 */
 
#ifndef _INCLUDED_TB_PHYSICS_ACTOR_2D_H_
#define _INCLUDED_TB_PHYSICS_ACTOR_2D_H_

#include <base/math/tb_vector2.h>
#include <physics/tb_physics_types.h>
#include <physics/tb_physics_shape_2d.h>
#include <physics/tb_physics_joint_2d.h>
#include "./depend/tb_physics_body_2d_depend.h"

namespace toybox
{

class TbPhysicsScene2d;

class TbPhysicsBody2d
{
public:

    enum Type
    {
        TYPE_STATIC , 
        TYPE_DYNAMIC ,
        TYPE_MAX
    };

public:

    // コンストラクタ
    TbPhysicsBody2d();

    // デストラクタ
    virtual ~TbPhysicsBody2d();

private:
    
    // コピー代入禁止
    TB_DISALLOW_COPY_AND_ASSIGN(TbPhysicsBody2d);

public:

    // シーンに入る
    void Enter( TbPhysicsScene2d* scene ) ;

    // シーンから外れる
    void Exit();

public:

    // 型設定
    void SetType( Type type )
    {
        m_Type = type;
    }

    // シーン取得
    TbPhysicsScene2d* GetScene() {
        return m_Scene;
    }

    // シーン取得
    const TbPhysicsScene2d* GetScene() const {
        return m_Scene;
    }

    // 形状設定
    void SetShape( TbPhysicsShapeBase2d* shape ) {
        m_Shape = shape;
    }

    // 重力影響度
    void SetGravityScale( TbFloat32 gravityScale ) {
        m_GravityScale = gravityScale;
    }

    // 初期位置設定
    void SetInitPos( const TbVector2& pos ) {
        m_InitPos = pos;
    }

    // 初期位置設定
    void SetInitPos( const TbFloat32 x , const TbFloat32 y ) {
        m_InitPos.Set( x , y );
    }

    // 初期位置取得
    const TbVector2& GetInitPos() const {
        return m_InitPos;
    }

    // 初期位置設定
    void SetInitVelo( const TbVector2& velo ) {
        m_InitVelo = velo;
    }

    // 初期位置設定
    void SetInitVelo( const TbFloat32 x , const TbFloat32 y ) {
        m_InitVelo.Set( x , y );
    }

    // 初期角度設定
    void SetInitAngle( TbRadian rad ) {
        m_InitAngle = rad;
    }

    // 初期角度取得
    TbRadian GetInitAngle() const {
        return m_InitAngle;
    }

    // 初期角度設定
    void SetInitVeloAngle( TbRadian rad ) {
        m_InitVeloAngle = rad;
    }

    // 速度
    void SetVelo( const TbVector2& velo );

    // 回転速度
    void SetVeloAngle( TbRadian rot );

    // 回転減衰
    void SetDampingAngle( TbFloat32 value );
    
    // 座標取得
    TbVector2 GetPos() const;
    
    // 角度取得
    TbRadian GetAngle() const;

    // ユーザーデータ設定
    void SetUserData( void* ptr ) {
        m_UserData = ptr;
    }

    // レイヤー設定
    void SetLayer( TbSint32 layer ) {
        m_Layer = layer;
    }

    // レイヤー取得
    TbSint32 GetLayer() const {
        return m_Layer;
    }

    // ユーザーデータ取得
    void* GetUserData() {
        return m_UserData;
    }

    // ユーザーデータ取得
    const void* GetUserData() const {
        return m_UserData;
    }

public:

    // 有効設定
    void SetActive( TbBool isActive );

    // 有効かどうか
    TbBool IsActive() const;

    // 動いている
    TbBool IsAwake() const;

public: // 依存コード

    TbPhysicsBody2dDepend& GetDepend() {
        return m_Depend;
    }
    
    const TbPhysicsBody2dDepend& GetDepend() const {
        return m_Depend;
    }

private:

    Type                   m_Type;
    TbSint32               m_Layer;
    TbPhysicsScene2d*      m_Scene;
    TbPhysicsShapeBase2d*  m_Shape;
    TbVector2              m_InitPos;       // 初期位置
    TbVector2              m_InitVelo;      // 初速度
    TbRadian               m_InitAngle;     // 初期角度
    TbRadian               m_InitVeloAngle;     // 緒角速度
    TbFloat32              m_DampingAngle;  // 減衰
    TbFloat32              m_GravityScale;
    void*                  m_UserData;

private:
    TbPhysicsBody2dDepend m_Depend;
};

}

#endif
