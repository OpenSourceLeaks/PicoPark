/*!
 * モーション
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */

#ifndef _INCLUDED_TB_MOTION_VMD_H_
#define _INCLUDED_TB_MOTION_VMD_H_

#include <graphics/skeleton/tb_skeleton_pmx.h>
#include <graphics/model/tb_model_data_pmx.h>
#include <graphics/motion/tb_motion_data_vmd.h>

namespace toybox
{

class TbMotionVmd
{
public:

    struct BoneNode
    {
        BoneNode()
            : parent(nullptr) , bone(nullptr) , motion(nullptr) , matrix(nullptr)
        {
            localMatrix.Unit();
        }
        BoneNode*                       parent;         // 親
        const TbModelDataPmx::BoneInfo* bone;           // ボーン情報
        const TbMotionDataVmd::Bone*    motion;         // モーション情報
        TbMatrix                        localMatrix;    // ローカル行列
        TbMatrix*                       matrix;         // 行列

        TbVector3                       offsetPos;      // オフセット座標
    };

public:

    // コンストラクタ
    TbMotionVmd();

    // デストラクタ
    ~TbMotionVmd();

public:

    // 対象となるボーンモデル定
    void SetSkeleton( const TbSkeletonPmx* bone );

    // 適用するモーション設定
    void SetMotion( const TbMotionDataVmd* data );

    // 進行
    void StepFrame( TbFloat32 frame );

    // フレーム設定
    void SetFrame( TbFloat32 frame );

    // フレーム設定
    void SetFrameBegin( TbFloat32 frame );

    // フレーム設定
    void SetFrameEnd( TbFloat32 frame );

    // フレーム取得
    TbFloat32 GetFrame() const {
        return m_Frame;
    }

    // ループ設定
    void SetLoop( TbBool isLoop ) {
        m_BitArray.SetBit(FLAG_LOOP,isLoop);
    }

    // ループ設定
    void SetLoop( TbBool isLoop , TbFloat32 begin , TbFloat32 end ) {
        m_BitArray.SetBit(FLAG_LOOP,isLoop);
        SetFrameBegin(begin);
        SetFrameEnd(end);
    }

    // IK処理を有効にする
    void SetEnableIK( TbBool isEnableIK ){
        m_BitArray.SetBit(FLAG_IK,isEnableIK);
    }

    TbBool IsEnableIK() const {
        return m_BitArray.Test(FLAG_IK);
    }

public:

    // オフセット座標設定
    void SetOffsetPos( TbUint32 boneIndex , const TbVector3& pos ) {
        if( m_Skeleton && boneIndex < m_Skeleton->GetBoneCount() ) {
            m_Node[boneIndex].offsetPos = pos;
        }
    }

public:

    // 姿勢行列取得
    const TbMatrix* GetMatrix( TbUint32 boneIndex ) const {
        if( m_Skeleton && boneIndex < m_Skeleton->GetBoneCount() ) {
            return &m_MatrixBuffer[boneIndex];
        }
        return nullptr;
    }

    // 姿勢変換行列取得
    void GetTransformMatrix( TbMatrix43& dst ,TbUint32 boneIndex ) const {
        if( m_Skeleton && boneIndex < m_Skeleton->GetBoneCount() ) {
            dst = m_MatrixBuffer[boneIndex] * m_Skeleton->GetBone(boneIndex)->invGlobalPoseMatrix;
        }
    }

    // 姿勢行列バッファ取得
    const TbMatrix* GetMatrixBuffer() const {
        return m_MatrixBuffer;
    }

private:

    // 解放
    void release();

    // セットアップ
    void setup();

    // 姿勢行列更新
    void updateMatrix();

    // グローバル座標更新
    void updateGlobalMatrix( BoneNode& node );

    // IK更新
    void updateIK();

private:

    enum Flag
    {
        FLAG_LOOP , 
        FLAG_IK ,
        FLAG_CHANGED_BONE , // 骨が変わった
        FLAG_CHANGED_MOTION , // 骨が変わった
    };
    
private:
    
    TbBitArray32                    m_BitArray;     // ビット配列
    TbFloat32                       m_Frame;        // フレーム
    TbFloat32                       m_FrameBegin;   // ループフレームの開始
    TbFloat32                       m_FrameEnd;     // ループフレームの末端

private:

    const TbSkeletonPmx*            m_Skeleton;     // 適用する骨データ
    const TbMotionDataVmd*          m_MotionData;   // 適用するモーションデータ
    BoneNode*                       m_Node;         // 計算用骨
    TbMatrix*                       m_MatrixBuffer; // 行列バッファ

};


}

#endif
