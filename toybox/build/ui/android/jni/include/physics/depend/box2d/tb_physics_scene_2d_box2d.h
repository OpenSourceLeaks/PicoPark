/*!
 * 物理系シーンの依存クラス
 * @author Miyake Shunsuke
 * @since 2011.05.06
 */
 
#ifndef _INCLUDED_TB_PHYSICS_SCENE_H_
#define _INCLUDED_TB_PHYSICS_SCENE_H_

 namespace toybox
 {

class TbPhysicsScene2dDepend
{
    friend class TbPhysicsScene2d;
public:
    // コンストラクタ
    TbPhysicsScene2dDepend();
    // デストラクタ
    ~TbPhysicsScene2dDepend();
public:
    // ワールド取得
    b2World& GetWorld() {
        return m_World;
    }
    // 背景取得
    b2Body* GetBackGround() {
        return m_BackGround;
    }
private:
    b2World    m_World;
    b2Body*    m_BackGround;
};

}

#endif
