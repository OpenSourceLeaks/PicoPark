/*!
 * アクタークラス
 * @author Miyake Shunsuke
 * @since 2013.03.20
 */

#ifndef _INCLUDED_TB_ACTOR_H_
#define _INCLUDED_TB_ACTOR_H_

#include <base/math/tb_vector4.h>
#include <base/math/tb_vector2.h>
#include <base/string/tb_static_string.h>
#include <base/container/tb_array.h>
#include <base/util/tb_ref_counter.h>
#include <base/util/tb_dynamic_cast.h>

namespace toybox
{

class TbActorScene;

class TbActor : public TbRefCounter
{
    friend class TbActorScene;
public:

    class Iterator
    {
    public:
        
        Iterator( TbActor* self )
            : m_Top(self)
            , m_Self(self)
        {
        }

    public:

        // 代入演算子
        Iterator& operator=( const Iterator& src ){
            m_Self = src.m_Self;
            m_Top = src.m_Top;
            return *this;
        }

        // 前置インクリメントのオーバーロード
        Iterator& operator++() {
            if( m_Self && m_Self->m_Next != m_Top ){
                m_Self = m_Self->m_Next;
            }else{
                m_Self = nullptr;
            }
            return *this;
        }

        // 後置インクリメントのオーバーロード
        Iterator operator++(int) {
            Iterator result = *this;
            if( m_Self && m_Self != m_Top ){
                m_Self = m_Self->m_Next;
            }else{
                m_Self = nullptr;
            }
            return result;
        }

        // 比較演算子
        TbBool operator==( const Iterator& src ) const {
            return m_Self == src.m_Self;
        }

        // 比較演算子
        TbBool operator!=( const Iterator& src ) const {
            return m_Self != src.m_Self;
        }

    public:

        // 取得演算子
        TbActor* operator*() {
            return m_Self;
        }
        
        // 取得演算子
        const TbActor* operator*() const {
            return m_Self;
        }

    private:
        TbActor* m_Self;
        TbActor* m_Top;
    };

public:
    
    // コンストラクタ
    TbActor()
      : m_Pos()
      , m_Scene(nullptr)
      , m_Next(nullptr)
      , m_Prev(nullptr)
      , m_LayerIndex(0U)
      , m_LayerIndexDefault(0U)
    {
        m_Pos.SetW(1.0f);
    }

public:

    TB_DEFINE_BASE_CLASS(TbActor);

protected:

    // デストラクタ
    virtual ~TbActor() {
        exitForce();
    }

protected:

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime ) {}

    // ロジック後処理
    virtual void OnPost( TbFloat32 deltatime ) {}

    // 描画更新
    virtual void OnDraw( TbFloat32 deltatime ) {}

    // シーンに登録された
    virtual void OnEnter( void* userData ) {}

    // シーンから出た
    virtual void OnExit() {}

    // 処理中断
    virtual void OnSuspend() {}

    // 処理復帰
    virtual void OnResume() {}

    // 描画中断
    virtual void OnSuspendDraw() {}

    // 描画復帰
    virtual void OnResumeDraw() {}

public:

    // 名前設定
    void SetName( const char* name ) {
        m_Name = name;
    }

    // 名前設定
    const char* GetName() const {
        return m_Name.GetBuffer();
    }

    // 位置取得
    const TbVector4& GetPos() const {
        return m_Pos;
    }

    // 2次元で取得
    TbVector2 GetPosXY() const {
        return TbVector2(m_Pos.GetX(),m_Pos.GetY());
    }

    // X座標取得
    TbFloat32 GetPosX() const {
        return m_Pos.GetX();
    }

    // Y座標取得
    TbFloat32 GetPosY() const {
        return m_Pos.GetY();
    }

    // 位置設定
    void SetPos( const TbVector4& pos ) {
        m_Pos = pos;
    }

    // 位置設定
    void SetPos( const TbVector2& pos ) {
        m_Pos.Set( pos.GetX() , pos.GetY() , 0.0f );
    }

    // 位置設定
    void SetPos( TbFloat32 x , TbFloat32 y ) {
        m_Pos.Set( x , y , 0.0f );
    }

    // 位置設定
    void SetPos( TbFloat32 x , TbFloat32 y , TbFloat32 z ) {
        m_Pos.Set( x , y , z );
    }

    // X座標設定
    void SetPosX( TbFloat32 x ) {
        m_Pos.SetX( x );
    }

    // Y座標設定
    void SetPosY( TbFloat32 y ) {
        m_Pos.SetY( y );
    }

    // Z座標設定
    void SetPosZ( TbFloat32 z ) {
        m_Pos.SetZ( z );
    }

    // 位置加算
    void AddPos( const TbVector4& pos ) {
        m_Pos += pos;
    }
    
    // 位置加算
    void AddPos( const TbVector2& pos ) {
        m_Pos.Add( pos.GetX() , pos.GetY() , 0.0f );
    }

    // 位置加算
    void AddPos( TbFloat32 x , TbFloat32 y , TbFloat32 z ) {
        m_Pos.Add( x , y , z );
    }

    // 回転角度設定(Degree)
    void SetRotDeg( const TbVector4& rot ) {
        m_Rot = rot;
    }

    // 回転角度加算(Degree)
    void AddRotDeg( const TbVector4& rot ) {
        m_Rot += rot;
    }

    // スケール設定
    void SetScale( const TbVector4& scale ) {
        m_Scale = scale;
    }

    // スケール加算
    void AddScale( const TbVector4& scale ) {
        m_Scale += scale;
    }

public:

    // シーンに登録
    void Enter( TbActorScene* scene , TbUint32 layerIndex , void* userData );

    // シーンに登録
    void Enter( TbActorScene* scene , void* userData ) {
        Enter( scene , m_LayerIndexDefault , userData );
    }

    // シーンから解除
    void Exit();

    // 処理中断
    void Suspend() {
        if( m_Scene ) {
            m_BitArray.SetBit(FLAG_PAUSE);
            OnSuspend();
        }
    }

    // 処理中断中か
    TbBool IsSuspended() const {
        return m_BitArray.Test(FLAG_PAUSE);
    }

    // 処理復帰
    void Resume() {
        if( m_Scene ) {
            m_BitArray.SetBit(FLAG_PAUSE,TB_FALSE);
            OnResume();
        }
    }

    // 描画処理中断
    void SuspendDraw() {
        if( m_Scene ) {
            m_BitArray.SetBit(FLAG_PAUSE_DRAW);
            OnSuspendDraw();
        }
    }

    // 描画処理復帰
    void ResumeDraw() {
        if( m_Scene ) {
            m_BitArray.SetBit(FLAG_PAUSE_DRAW,TB_FALSE);
            OnResumeDraw();
        }
    }

    // シーンに登録されている
    TbBool IsEntered() const {
        return m_Scene != NULL;
    }

    // 所属シーン取得
    TbActorScene* GetScene() {
        return m_Scene;
    }

    // 所属シーン取得
    const TbActorScene* GetScene() const {
        return m_Scene;
    }

    // デフォルトのレイヤー番号設定
    void SetLayerIndexDefault( TbUint32 index ) {
        m_LayerIndexDefault = index;
    }

    // レイヤー番号取得
    TbUint32 GetLayerIndex() const {
        return m_LayerIndex;
    }

private:

    enum Flag
    {
        FLAG_PAUSE ,
        FLAG_PAUSE_DRAW ,
        FLAG_EXIT , 
        FLAG_MAX , 
    };

private:

    // 更新
    void update( TbFloat32 deltatime )
    {
        OnUpdate(deltatime);
        if( m_BitArray.Test(FLAG_EXIT) ){
            exitForce();
        }
    }

    // 後処理
    void post( TbFloat32 deltatime )
    {
        OnPost(deltatime);
        if( m_BitArray.Test(FLAG_EXIT) ){
            exitForce();
        }
    }

    // 即時に解除
    void exitForce();

private:

    TbBitArray32        m_BitArray; // ビット配列
    TbStaticString32    m_Name;     // 名前

private:

    TbVector4           m_Pos;      // 位置
    TbVector4           m_Rot;      // 回転
    TbVector4           m_Scale;    // 拡大
    
private:

    TbActorScene*               m_Scene;                // 所属ワールド
    TbActor*                    m_Next;                 // 次
    TbActor*                    m_Prev;                 // 前
    TbUint32                    m_LayerIndex;           // レイヤー
    TbUint32                    m_LayerIndexDefault;    

};

}

#endif
