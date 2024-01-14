/*!
 * モーション
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */


#include <tb_fwd.h>
#include <graphics/motion/tb_motion_vmd.h>
#include <base/math/tb_curve.h>
#include <base/math/tb_math_helper.h>

namespace toybox
{

namespace
{
    struct CompareBoneFrameFunc
    {
        TbBool operator()( const TbMotionDataVmd::Frame* keyFrame , TbFloat32 frame ) const {
            return static_cast<TbFloat32>(keyFrame->frame) < frame;
        }
        TbBool operator()( TbFloat32 frame , const TbMotionDataVmd::Frame* keyFrame ) const {
            return frame < static_cast<TbFloat32>(keyFrame->frame);
        }
    };

    // ボーン行列更新
    void updateBoneMatrix( TbMatrix& dst , const TbMotionVmd::BoneNode& node , const TbFloat32 frame )
    {
        TbFloat32 t = 0.0f;
        TbFloat32 tx = 0.0f;
        TbFloat32 ty = 0.0f;
        TbFloat32 tz = 0.0f;
        TbFloat32 tq = 0.0f;
        const TbMotionDataVmd::Frame* keyFrame = nullptr;
        const TbMotionDataVmd::Frame* nextKeyFrame = nullptr;
        if( node.motion ) {
            TbQuaternion q;
            TbVector3 pos;
            TbMotionDataVmd::Bone::KeyFrameList::ConstIterator beginIte = node.motion->keyFrame.Begin();
            TbMotionDataVmd::Bone::KeyFrameList::ConstIterator endIte = node.motion->keyFrame.End();
            TbMotionDataVmd::Bone::KeyFrameList::ConstIterator ite = TbUpperBound(beginIte,endIte,frame,CompareBoneFrameFunc());
            dst = node.bone->localPoseMatrix;
            if( ite == beginIte ){
                // 最初フレーム
                keyFrame = *beginIte;
                pos = keyFrame->pos;
                q = keyFrame->quat;
            }else if( ite != endIte ) {
                keyFrame = *(ite-1);
                nextKeyFrame = *(ite);
                t = (frame-static_cast<TbFloat32>(keyFrame->frame))/static_cast<TbFloat32>(nextKeyFrame->frame-keyFrame->frame);
                tx = TbBezier(t,keyFrame->interpolateParamX1,keyFrame->interpolateParamX2);
                ty = TbBezier(t,keyFrame->interpolateParamY1,keyFrame->interpolateParamY2);
                tz = TbBezier(t,keyFrame->interpolateParamZ1,keyFrame->interpolateParamZ2);
                tq = TbBezier(t,keyFrame->interpolateParamQ1,keyFrame->interpolateParamQ2);
                TbFloat32 x = TbLerp(keyFrame->pos.GetX(),nextKeyFrame->pos.GetX(),tx);
                TbFloat32 y = TbLerp(keyFrame->pos.GetY(),nextKeyFrame->pos.GetY(),ty);
                TbFloat32 z = TbLerp(keyFrame->pos.GetZ(),nextKeyFrame->pos.GetZ(),tz);
                q = TbQuaternion::Slerp(keyFrame->quat,nextKeyFrame->quat,tq);
                pos.Set( x , y , z );
            }else{
                // 最終フレーム
                keyFrame = *(endIte-1);
                pos = keyFrame->pos;
                q = keyFrame->quat;
            }
            dst.Unit();
            pos += node.offsetPos;
            if( node.parent ) {
                // 親が存在する場合は親の位置を差し引く
                dst.Translate(pos+node.bone->pos-node.parent->bone->pos);
            }else{
                dst.Translate(pos+node.bone->pos);
            }
            dst = dst * q.GetMatrix();
        }else{
            dst = node.bone->localPoseMatrix;
            dst.AddTranslation( node.offsetPos );
        }

    }

}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
TbMotionVmd::TbMotionVmd()
    : m_Frame(0.0f)
    , m_Skeleton(nullptr)
    , m_MotionData(nullptr)
    , m_Node(nullptr)
    , m_MatrixBuffer(nullptr)
{
    SetEnableIK(TB_TRUE);
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
TbMotionVmd::~TbMotionVmd()
{
    release();
}

/*!
 * 解放
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::release()
{
    TB_SAFE_DELETE_ARRAY(m_Node);
    TB_SAFE_DELETE_ARRAY(m_MatrixBuffer);
}

/*!
 * 対象となる骨情報設定
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::SetSkeleton( const TbSkeletonPmx* skeleton )
{
    if( m_Skeleton != skeleton ){
        m_BitArray.SetBit(FLAG_CHANGED_BONE);
    }
    m_Skeleton = skeleton;
    setup();
}

/*!
 * 適用するモーション設定
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::SetMotion( const TbMotionDataVmd* motion )
{
    if( motion != m_MotionData ){
        m_BitArray.SetBit(FLAG_CHANGED_MOTION);
    }
    m_MotionData = motion;
    setup();
}

/*!
 * 進行
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::StepFrame( TbFloat32 frame )
{
    if( !m_MotionData ){
        return;
    }
    m_Frame += frame;
    if( m_Frame >= m_FrameEnd ){
        // フレームが最大フレームに到達
        if( m_BitArray.Test(FLAG_LOOP) ){
            m_Frame = m_FrameBegin; // とりあえずループ
        }else{
            m_Frame = m_FrameEnd;
        }
    }
    updateMatrix();
}

/*!
 * フレーム設定
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::SetFrame( TbFloat32 frame )
{
    if( !m_MotionData ){
        return;
    }
    m_Frame = frame;
    if( m_Frame >= static_cast<TbFloat32>(m_MotionData->frameMax) ){
        m_Frame = 0.0f; // とりあえず0.0f
    }
    updateMatrix();
}

/*!
 * フレーム設定
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
void TbMotionVmd::SetFrameBegin( TbFloat32 frame )
{
    m_FrameBegin = TbClamp( frame , 0.0f , m_MotionData ? m_MotionData->frameMax : 0.0f );
    if( m_Frame < m_FrameBegin ) {
        SetFrame(m_FrameBegin);
    }
}

/*!
 * フレーム設定
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
void TbMotionVmd::SetFrameEnd( TbFloat32 frame )
{
    m_FrameEnd = TbClamp( frame , 0.0f , m_MotionData ? m_MotionData->frameMax : 0.0f );
}

/*!
 * セットアップ
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::setup()
{
    if( m_BitArray.Test(FLAG_CHANGED_BONE) ){
        release();
        if( m_Skeleton ){
            TbUint32 boneCount = m_Skeleton->GetBoneCount();
            m_Node = TB_NEW BoneNode[boneCount];
            m_MatrixBuffer = TB_NEW TbMatrix[boneCount];
            for( TbUint32 i = 0; i < boneCount; ++i ) {
                const TbModelDataPmx::BoneInfo& bone = *m_Skeleton->GetBone(i);
                m_Node[i].bone = &bone;
                m_Node[i].matrix = &m_MatrixBuffer[i];
                if( bone.parent ) {
                    m_Node[i].parent = &m_Node[bone.parentBoneIndex];
                }
            }
        }
    }
    if( m_BitArray.Test(FLAG_CHANGED_MOTION) ){
        TbUint32 boneCount = m_Skeleton ? m_Skeleton->GetBoneCount() : 0;
        for( TbUint32 i = 0; i < boneCount; ++i ) {
            m_Node[i].motion = nullptr;
            if( m_MotionData ){
                const TbModelDataPmx::BoneInfo& bone = *m_Skeleton->GetBone(i);
                m_Node[i].motion = m_MotionData->GetBone(bone.nameHash);
            }
        }
        m_FrameBegin = 0.0f;
        m_FrameEnd = m_MotionData ? m_MotionData->frameMax : 0.0f;
    }
    m_BitArray.SetBit(FLAG_CHANGED_BONE,TB_FALSE);
    m_BitArray.SetBit(FLAG_CHANGED_MOTION,TB_FALSE);
}

/*!
 * 姿勢行列更新
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::updateMatrix()
{
    if( !m_Skeleton || !m_MotionData ) {
        return;
    }

    // ローカル行列更新
    TbUint32 boneCount = m_Skeleton->GetBoneCount();
    for( TbUint32 i = 0; i < boneCount; ++i ){
        updateBoneMatrix( m_Node[i].localMatrix , m_Node[i] , m_Frame );
    }

    // グローバル座標系更新
    for( TbUint32 i = 0; i < boneCount; ++i ){
        updateGlobalMatrix(m_Node[i]);
    }

    // IK更新
    if( IsEnableIK() ){
        updateIK();
    }
}

/*!
 * グローバル座標更新
 * @author Miyake_Shunsuke
 * @since 2014.04.10
 */
void TbMotionVmd::updateGlobalMatrix( BoneNode& node )
{
    // 初期グローバル座標系設定
    BoneNode* parent = node.parent;
    *node.matrix = node.localMatrix;
    while( parent ){
        *node.matrix = parent->localMatrix * *node.matrix;
        parent = parent->parent;
    }
}


/*!
 * IK更新(Cyclic-Coordinate-Descent（CCD）法)
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbMotionVmd::updateIK()
{
    TbUint32 boneCount = m_Skeleton->GetBoneCount();
    for( TbUint32 i = 0; i < boneCount; ++i ){

#if 1
        if( 31 != i ) {
            continue;
        }
#endif


        BoneNode& node = m_Node[i];
        if( !(node.bone->flag & TbModelDataPmx::BoneInfo::FLAG_BIT_IK) ) {
            continue;
        }

        // エフェクター
        BoneNode& effector = m_Node[node.bone->ikTargetBoneIndex];
        TbVector4 pos;
        effector.localMatrix.GetTranslation(pos);
        effector.localMatrix.Unit();
        pos.SetXZ(0.0f,0.0f);
        effector.localMatrix.SetTranslation(pos);
        TbVector4 effectorPos;
        updateGlobalMatrix(effector);
        effector.matrix->GetTranslation(effectorPos);

        // 目標座標
        TbVector4 targetPos;
        node.matrix->GetTranslation(targetPos);

        // IK計算
        for( TbUint32 loop = 0; loop < node.bone->ikLoopCount; ++loop ) {
            for( TbUint32 link = 0; link < node.bone->ikLinkCount; ++link ) {
                const TbModelDataPmx::IkLink& ikLink = node.bone->ikLink[link];
                BoneNode& joint = m_Node[ikLink.linkBoneIndex];

                // ジョイントの座標(グローバル座標)
                TbVector4 jointPos;
                joint.matrix->GetTranslation(jointPos);
  
                // ジョイント基準のローカル座標系に変換
                TbMatrix invJointMatrx;
                joint.matrix->Invert(invJointMatrx);
                TbVector4 localEffectorPos = invJointMatrx * effectorPos;
                TbVector4 localTargetPos = invJointMatrx * targetPos;
                
                // ジョイントから見た、エフェクターと目標点までの方向
                TbVector4 joint2effector = localEffectorPos;
                TbVector4 joint2target = localTargetPos;
                joint2effector.Normalize();
                joint2target.Normalize();

                TbFloat32 rad;
                TbCalcAngleRad(rad,joint2effector,joint2target);
                if( rad < TB_FLOAT32_EPSILON ) {
                    // 同じ方向だったら特にしない
                    continue;
                }

                if( rad > node.bone->ikConstraintRad ){
                    rad = node.bone->ikConstraintRad;
                }

                TbFloat32 deg = TbRadToDeg(rad);

                // ターゲットの位置に向かって向くようにする
                TbVector4 rotAxis = joint2effector.Cross(joint2target);
                rotAxis.Normalize();
                TbQuaternion quat( rotAxis , TbRadToAng32(rad) );
                TbVector4 localJointPos;
                joint.localMatrix.GetTranslation(localJointPos);
                joint.localMatrix = joint.localMatrix * quat.GetMatrix();

                TbVector4 testPos = quat.GetMatrix() * localEffectorPos;
                testPos.Normalize();
                TbCalcAngleRad(rad,testPos,joint2target);

                // 角度制限がある場合は、オイラー角に分解して角度制限をかける
                if( ikLink.constraintRadFlag )
                {
                    TbVector3 eular = joint.localMatrix.GetEulerRadXYZ();
                    eular.SetX( TbClamp( eular.GetX() , ikLink.constraintRadMin[0] , ikLink.constraintRadMax[0] ) );
                    eular.SetY( TbClamp( eular.GetY() , ikLink.constraintRadMin[1] , ikLink.constraintRadMax[1] ) );
                    eular.SetZ( TbClamp( eular.GetZ() , ikLink.constraintRadMin[2] , ikLink.constraintRadMax[2] ) );
                    TbFloat32 degX = TbRadToDeg(eular.GetX());
                    TbFloat32 degY = TbRadToDeg(eular.GetY());
                    TbFloat32 degZ = TbRadToDeg(eular.GetZ());
                    joint.localMatrix.Unit();
                    joint.localMatrix.RotateZ( TbRadToAng32(eular.GetZ()) );
                    joint.localMatrix.RotateY( TbRadToAng32(eular.GetY()) );
                    joint.localMatrix.RotateX( TbRadToAng32(eular.GetX()) );
                    joint.localMatrix.SetTranslation(localJointPos);
                }

                // エフェクターとリンクボーンのグローバル座標を更新しておく
//                   updateGlobalMatrix(joint);
//                    updateGlobalMatrix(effector);

                // グローバル座標系更新
                for( TbUint32 i = 0; i < boneCount; ++i ){
                    updateGlobalMatrix(m_Node[i]);
                }

                effector.matrix->GetTranslation(effectorPos);
            }
            // ゴールに到達したら終了
            TbFloat32 delta = (effectorPos-targetPos).LengthSq();
            if( delta < 0.0001f ){
                int a = 0;
                break;
            }
        }
    }
}

}