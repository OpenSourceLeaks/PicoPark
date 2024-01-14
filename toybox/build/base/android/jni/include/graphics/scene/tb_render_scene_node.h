/*!
 * 描画シーンノード
 * @author Miyake Shunsuke
 * @since 2013.05.07
 */

#ifndef _INCLUDED_TB_RENDER_SCENE_NODE_H_
#define _INCLUDED_TB_RENDER_SCENE_NODE_H_

#include <base/math/tb_vector4.h>
#include <base/math/tb_matrix.h>
#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>

namespace toybox
{

class TbRenderScene;

class TbRenderSceneNode
{
public:

    friend  class TbRenderScene;
    typedef TbStaticFunction32<void (TbRenderSceneNode*)>    SteppedCallback; // 更新コールバック

protected:
    
    // コンストラクタ
    TbRenderSceneNode()
      : m_Pos()
      , m_Scene(nullptr)
      , m_Next(nullptr)
      , m_Prev(nullptr)
    {
        m_Pos.SetW(1.0f);
        m_Scale.Set(1.0f,1.0f,1.0f,1.0f);
        SetEnableStep(TB_TRUE);
        SetEnableDraw(TB_TRUE);
    }

public:
     
    // 更新
    void Step( TbFloat32 deltatime ) {
        if( IsEnableStep() ) {
            OnStepImpl(deltatime);
        }
    }
    
    // 後処理
    void Post() {
        if( IsEnableStep() ) {
            OnPostImpl();
            if( m_SteppedCallback.IsValid() ) {
                m_SteppedCallback(this);
            }
        }
    }

    // 描画
    void Draw( TbUint32 passIndex ) {
        if( IsEnableDraw() ) {
            OnDrawImpl(passIndex);
        }
    }

public:

    // 名前設定
    void SetName( const char* name ) {
        m_Name = name;
    }

    // 名前設定
    const char* GetName() const {
        return m_Name.GetBuffer();
    }

    // 更新可能か
    void SetEnableStep( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_STEP , isEnable );
    }
    
    // 更新可能か
    TbBool IsEnableStep() const {
        return m_BitArray.Test( FLAG_ENABLE_STEP );
    }

    // 描画可能か
    void SetEnableDraw( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_DRAW , isEnable );
    }

    // 描画可能か
    TbBool IsEnableDraw() const {
        return m_BitArray.Test(FLAG_ENABLE_DRAW);
    }

    // 更新コールバック設定
    void SetSteppedCallback( const SteppedCallback& cb ) {
        m_SteppedCallback = cb;
    }

public:

    // 位置取得
    const TbVector4& GetPos() const {
        return m_Pos;
    }

    // 位置設定
    void SetPos( const TbVector4& pos ) {
        m_Pos = pos;
        m_BitArray.SetBit(FLAG_UPDATE_MATRIX);
    }

    // 位置加算
    void AddPos( const TbVector4& pos ) {
        m_Pos += pos;
        m_BitArray.SetBit(FLAG_UPDATE_MATRIX);
    }
    
    // 回転角度設定(Degree)
    void SetRotDeg( const TbVector4& rot ) {
        m_Rot = rot;
        m_BitArray.SetBit(FLAG_UPDATE_MATRIX);
    }

    // 回転角度加算(Degree)
    void AddRotDeg( const TbVector4& rot ) {
        m_Rot += rot;
        m_BitArray.SetBit(FLAG_UPDATE_MATRIX);
    }

    // スケール設定
    void SetScale( const TbVector4& scale ) {
        m_Scale = scale;
        m_BitArray.SetBit(FLAG_UPDATE_MATRIX);
    }

    // スケール加算
    void AddScale( const TbVector4& scale ) {
        m_Scale += scale;
        m_BitArray.SetBit(FLAG_UPDATE_MATRIX);
    }

    // 姿勢行列取得
    const TbMatrix& GetPoseMatrix() {
        if( m_BitArray.Test(FLAG_UPDATE_MATRIX) ){
            m_Matrix.Unit();
            m_Matrix.Translate(m_Pos);
            m_Matrix.Scale(m_Scale.GetX(),m_Scale.GetY(),m_Scale.GetZ());
            m_Matrix.RotateZ(TbDegToAng32(m_Rot.GetZ()));
            m_Matrix.RotateY(TbDegToAng32(m_Rot.GetY()));
            m_Matrix.RotateX(TbDegToAng32(m_Rot.GetX()));
            m_BitArray.ResetBit(FLAG_UPDATE_MATRIX);
        }
        return m_Matrix;
    }

public:

    // シーンに登録
    void Enter( TbRenderScene* scene );

    // シーンから解除
    void Exit();

    // 所属しているシーン取得
    TbRenderScene* GetScene() {
        return m_Scene;
    }

    // 所属しているシーン取得
    const TbRenderScene* GetScene() const {
        return m_Scene;
    }
    
protected:

    // 更新
    virtual void OnStepImpl( TbFloat32 deltatime ) {}
    
    // 後処理
    virtual void OnPostImpl() {}

    // 描画
    virtual void OnDrawImpl(TbUint32 passIndex ) {}

private:

    enum Flag
    {
        FLAG_ENABLE_STEP , 
        FLAG_ENABLE_DRAW , 
        FLAG_UPDATE_MATRIX ,
        FLAG_MAX , 
    };
    
private:
    
    // 所属シーン設定
    void setScene( TbRenderScene* scene ) {
        m_Scene = scene;
    }

private:

    TbBitArray32        m_BitArray; // ビット配列
    TbStaticString32    m_Name;     // 名前

private:

    TbVector4           m_Pos;              // 位置
    TbVector4           m_Rot;              // 回転
    TbVector4           m_Scale;            // 拡大
    TbMatrix            m_Matrix;           // 姿勢行列
    SteppedCallback     m_SteppedCallback;  // 更新コールバック
    
private:
    
    TbRenderScene*         m_Scene;         // 所属ワールド
    TbRenderSceneNode*     m_Next;          // 次
    TbRenderSceneNode*     m_Prev;          // 前

};
}

#endif
