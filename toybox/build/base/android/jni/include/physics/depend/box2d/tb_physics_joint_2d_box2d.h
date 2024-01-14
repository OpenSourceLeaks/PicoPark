/*!
 * 2D形状の依存型
 * @author teco
 */
 
#ifndef _INCLUDED_TB_PHYSICS_JOINT_2D_BOX2D_H_
#define _INCLUDED_TB_PHYSICS_JOINT_2D_BOX2D_H_

namespace toybox
{

class TbPhysicsJoint2d;

class TbPhysicsJoint2dDepend
{
public:
    TbPhysicsJoint2dDepend() : m_World(nullptr) , m_Joint(nullptr) {}
public:
    // 有効か
    TbBool IsValid() const {
        return m_World && m_Joint;
    }
    // 設定
    void Set( b2Joint* joint , b2World* world , TbPhysicsJoint2d* src ) {
        TB_RESQUE(!m_Joint&&!m_World);
        m_World = world;
        m_Joint = joint;
        m_Joint->SetUserData(src);
    }
    // リセット
    void Reset( TbBool isEnableDestroy )
    {
        if( m_Joint && m_World ) {
            if( isEnableDestroy ) {
                m_World->DestroyJoint( m_Joint );
            }
            m_Joint = nullptr;
            m_World = nullptr;
        }
    }
    b2Joint* GetJoint() {
        return m_Joint;
    }
private:
    b2World*            m_World;
    b2Joint*            m_Joint;
};

struct TbPhysicsRevoluteJoint2dDepend
{
    b2RevoluteJointDef jointDef;
};

struct TbPhysicsGearJoint2dDepend
{
    b2GearJointDef jointDef;
};

}

#endif
