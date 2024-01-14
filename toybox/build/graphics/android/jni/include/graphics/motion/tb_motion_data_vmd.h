/*!
 * VMDデータ(MMDのモーションファイルフォーマット)
 * @author Miyake_Shunsuke
 * @since 2014.03.31
 */

#ifndef _INCLUDED_TB_MOTION_DATA_VMD_
#define _INCLUDED_TB_MOTION_DATA_VMD_

#include <base/crypt/tb_crc32.h>
#include <base/string/tb_string_converter.h>
#include <base/math/tb_matrix.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector3.h>
#include <base/math/tb_quaternion.h>
#include <base/container/tb_array.h>
#include <base/algorithm/tb_sort.h>

namespace toybox
{

class TbStreamBinaryReader;

struct TbMotionDataVmd
{
    struct Header
    {
        TbChar8  tag[30];
        TbChar8  name[20];
        TbUint32 dataMax;
    };

    struct Frame
    {
        TbChar8         boneName[15];
        TbUint32        boneNameHash;
        TbChar8         padding;
        TbUint32        frame;
        TbVector3       pos;
        TbQuaternion    quat;
        TbVector2       interpolateParamX1,interpolateParamX2; // X軸用補間パラメータ
        TbVector2       interpolateParamY1,interpolateParamY2; // Y軸用補間パラメータ
        TbVector2       interpolateParamZ1,interpolateParamZ2; // Z軸用補間パラメータ
        TbVector2       interpolateParamQ1,interpolateParamQ2; // 回転用補間パラメータ
    };

    struct Bone
    {
        typedef TbArray<const Frame*> KeyFrameList;

        Bone() {
            TbMemClear(name);
            nameHash = 0;
            keyFrame.SetEnableSetCapacityAuto(TB_TRUE);
            keyFrame.SetCapacity(32);
        }

        void Add(const  Frame* frame ) {
            TB_ASSERT(frame);
            keyFrame.PushBack(frame);
            if( TbCrc32::INVALID == nameHash ) {
                TbStrCopy(name,TB_ARRAY_COUNT_OF(name),frame->boneName);
                nameHash = frame->boneNameHash;
            }
        }

        char            name[16];
        TbUint32        nameHash;
        KeyFrameList    keyFrame;
    };

    struct CompareBoneHashFunc
    {
        TbBool operator()( const Bone& bone , TbUint32 hash ) const {
            return bone.nameHash < hash;
        }
        TbBool operator()( TbUint32 hash , const Bone& bone ) const {
            return hash < bone.nameHash;
        }
    };

    struct CompareFrameFunc
    {
        TbBool operator()( const Frame* left , const Frame* right ) const {
            return left->frame < right->frame;
        }
    };

    // コンストラクタ
    TbMotionDataVmd() : frameData(nullptr) , frameMax(0) , boneCount(0) , bone(nullptr) {}

    // デストラクタ
    ~TbMotionDataVmd()
    {
        TB_SAFE_DELETE_ARRAY(frameData);
        TB_SAFE_DELETE_ARRAY(bone);
    }

    // 読みこみ
    void Load( const void* buffer , TbSizeT bufferSize );

    // 骨情報取得
    const Bone* GetBone( TbUint32 nameHash ) const
    {
        for( TbUint32 i = 0; i < boneCount; ++i ) {
            if( bone[i].nameHash == nameHash ) {
                return &bone[i];
            }
        }
#if 0
        TbMotionDataVmdBone* result = TbLowerBound( bone,
                                                    bone+boneCount,
                                                    nameHash,
                                                    CompareBoneHashFunc());
        if( result != bone+boneCount && result->nameHash == nameHash ){
            return result;
        }
#endif
        return nullptr;
    }

    Header   header;
    Frame*   frameData;  // フレームデータ
    TbUint32 frameMax;   // 最大フレーム
    TbUint32 boneCount;
    Bone*    bone;
};

}

#endif

