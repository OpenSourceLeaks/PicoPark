/*!
 * VMDデータ(MMDのモーションファイルフォーマット)
 * @author Miyake_Shunsuke
 * @since 2014.03.31
 */

#include <tb_fwd.h>
#include <graphics/motion/tb_motion_data_vmd.h>
#include <base/io/stream/tb_const_memory_stream.h>
#include <base/io/stream/tb_stream_binary_reader.h>
#include <base/crypt/tb_crc32.h>
#include <base/string/tb_string_converter.h>
#include <base/math/tb_matrix.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector3.h>
#include <base/math/tb_quaternion.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_set.h>
#include <base/algorithm/tb_sort.h>

namespace toybox
{

namespace
{

void LoadLocal( TbMotionDataVmd::Frame& dst , TbStreamBinaryReader& reader )
{
    TbMemClear(dst);
    reader.Read(dst.boneName);
    dst.boneNameHash = TbCrc32::GetHash(dst.boneName,TbStrBufSize(dst.boneName));
    dst.frame = reader.ReadUint32();
    reader.Read(dst.pos);
    reader.Read(dst.quat);
    TbSint8 param[64];
    TbFloat32 normParam[64];
    reader.Read(param);
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(normParam); ++i ) {
        normParam[i] = static_cast<TbFloat32>(param[i])/127.0f;
    }
    dst.interpolateParamX1.Set(normParam[0],normParam[4]);
    dst.interpolateParamX2.Set(normParam[8],normParam[12]);
    dst.interpolateParamY1.Set(normParam[1],normParam[5]);
    dst.interpolateParamY2.Set(normParam[9],normParam[13]);
    dst.interpolateParamZ1.Set(normParam[2],normParam[6]);
    dst.interpolateParamZ2.Set(normParam[10],normParam[14]);
    dst.interpolateParamQ1.Set(normParam[3],normParam[7]);
    dst.interpolateParamQ2.Set(normParam[11],normParam[15]);
}

void LoadLocal( TbMotionDataVmd::Header& dst , TbStreamBinaryReader& reader )
{
    reader.Read(dst.tag);
    reader.Read(dst.name);
    reader.Read(dst.dataMax);
};

}

/*!
 * 読みこみ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbMotionDataVmd::Load( const void* buffer , TbSizeT bufferSize )
{
    TbConstMemoryStream stream(buffer,bufferSize);
    TbStreamBinaryReader reader(&stream);
    LoadLocal(header,reader);
    TbStaticSet<TbUint32,256> boneSet;
    if( header.dataMax > 0 ){
        frameData = TB_NEW Frame[header.dataMax];
        for( TbUint32 i = 0; i < header.dataMax; ++i ) {
            LoadLocal(frameData[i],reader);
            boneSet.Insert(frameData[i].boneNameHash);
            if( frameData[i].frame > frameMax ) {
                frameMax = frameData[i].frame;
            }
        }
    }
    // フレームをボーン別に分解
    if( !boneSet.IsEmpty() ){
        boneCount = boneSet.GetCount();
        bone = TB_NEW Bone[ boneCount ];
        TbStaticSet<TbUint32,256>::Iterator ite = boneSet.Begin();
        TbStaticSet<TbUint32,256>::Iterator endIte = boneSet.End();
        TbUint32 boneIndex = 0;
        for( ; ite != endIte; ++ite , ++boneIndex ){
            for( TbUint32 i = 0; i < header.dataMax; ++i ) {
                if( frameData[i].boneNameHash == *ite ){
                    bone[boneIndex].Add( &frameData[i] );
                }
            }
            TbInsertionSort(bone[boneIndex].keyFrame.Begin(),
                            bone[boneIndex].keyFrame.End(),
                            CompareFrameFunc());

        }
    }
}

}

