/*!d
 * @file
 * @brief カメラクラス
 * @author Miyake Shunsuke
 * @since 2011.06.10
 */

#ifndef _INCLUDED_TB_CAMERA_H_
#define _INCLUDED_TB_CAMERA_H_

#include <base/math/tb_geometry.h>
#include <base/math/tb_matrix.h>

namespace toybox
{

/*!
 * カメラクラス
 * @author Miyake Shunsuke
 * @since 2011.06.10
 */
class TbCamera
{
public:

    typedef TbCamera Self;

public:

    // コンストラクタ( デバイス直接指定 )
    TbCamera();

    //デストラクタ
    ~TbCamera();

public: // ビュー行列関連
    
    // View変換行列取得
    TbResult GetViewMatrix( TB_OUT TbMatrix& dst ) const;

    // 一括設定
    TbResult Set( const TbVector4& target ,  const TbVector4& pos , const TbVector4& upVec );

    // 注視点設定
    TbResult SetTarget( const TbVector4& target );

    // 注視点取得
    const TbVector4& GetTarget() const { 
        return m_Target; 
    }
    
    // 座標設定
    TbResult SetPosition( const TbVector4& pos );

    // 座標取得
    const TbVector4& GetPosition() const { 
        return m_Position; 
    }

    // 上ベクトル設定
    TbResult SetUpVec( const TbVector4& up );

    // 上ベクトル設定
    const TbVector4& GetUpVec() const { 
        return m_UpVec; 
    }
    
public: // 射影行列

    // 射影行列取得
    TbResult GetProjectionMatrix( TB_OUT TbMatrix& dst ) const;

    // ニア設定
    void SetNear( TbFloat32 near );

    // ファー設定
    void SetFar( TbFloat32 far );
    
public: // 透視射影専用

    // 視野角度設定
    void SetFieldOfView( TbFloat32 deg );

    // アスペクト比設定
    void SetAspect( TbFloat32 aspect );

public: // 正射影専用
    
    // 正射影カメラか
    TbBool IsOrthographic() const {
        return m_BitArray.Test(FLAG_ORTHOGRAPHICS);
    }

    // 正射影カメラか
    void SetOrthographic( TbBool isOrthographic );

    // 正射影サイズ
    void SetOrthographicSize( TbFloat32 w , TbFloat32 h );

public:

    enum Flag
    {
        FLAG_ORTHOGRAPHICS , // 正射影か
        FLAG_MAX , 
    };

private:
    
    TbVector4       m_Target;     // 注視店
    TbVector4       m_Position;   // 位置
    TbVector4       m_UpVec;      // 上ベクトル

private:

    TbBitArray32    m_BitArray;             // ビット配列
    TbFloat32       m_Near;                 // ニア
    TbFloat32       m_Far;                  // ファー

    TbDimensionF32  m_OrthographicSize;     // 正射影サイズ
    TbAngle32       m_FieldOfView;          // 視野角
    TbMatrix        m_ViewMatrix;           // 行列
    TbMatrix        m_OrthoMatrix;          // 正射影行列
    TbMatrix        m_PerspectiveMatrix;    // 透視射影行列

};

}

#endif