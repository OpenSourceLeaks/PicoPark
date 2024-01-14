/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 * @since 2011.05.06
 */
 
#ifndef _INCLUDED_TB_PHYSICS_ACTOR_2D_BOX2D_H_
#define _INCLUDED_TB_PHYSICS_ACTOR_2D_BOX2D_H_

 namespace toybox
 {

class TbPhysicsBody2dDepend
{
    friend class TbPhysicsBody2d;
public:
    
    // コンストラクタ
    TbPhysicsBody2dDepend();

    // デストラクタ
    ~TbPhysicsBody2dDepend();

public:

    // ボディ取得
    b2Body* GetBody() {
        return m_Body;
    }

    // ボディ取得
    const b2Body* GetBody() const {
        return m_Body;
    }

private:
    b2Body*     m_Body;
};

 }

#endif
