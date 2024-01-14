/*!
 * PMXデータ(MMDのモデルファイルフォーマット)
 * @author Miyake_Shunsuke
 * @since 2014.03.31
 */

#include <tb_fwd.h>
#include <graphics/model/tb_model_data_pmx.h>
#include <base/io/tb_file.h>
#include <base/io/stream/tb_const_memory_stream.h>
#include <base/io/stream/tb_stream_binary_reader.h>
#include <base/math/tb_matrix.h>
#include <base/math/tb_math_helper.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector3.h>
#include <base/container/tb_static_set.h>
#include <base/crypt/tb_crc32.h>

namespace toybox
{

namespace
{

    struct Reader
    {
        Reader( TbConstMemoryStream* stream ) 
            : reader(stream)
            , encode(TbModelDataPmx::ENCODE_UTF8)
        {}

        TbStreamBinaryReader reader;
        TbModelDataPmx::Encode encode;
    };

    // 設定
    void LoadLocal( TbModelDataPmx::Setting& dst , Reader& reader )
    {
        TbMemClear(dst);
        dst.dataSize = reader.reader.ReadUint8();
        if( TB_VERIFY(dst.dataSize<=TB_ARRAY_COUNT_OF(dst.data)) ){
            reader.reader.Read(dst.data,0,dst.dataSize);
        }
    }

    // テキスト
    void LoadLocal( TbModelDataPmx::Text& dst , Reader& reader )
    {
        TbMemClear(dst);
        TbChar16 textW[TbModelDataPmx::Text::TEXT_MAX/2];

        TbUint32 textSize = reader.reader.ReadUint32();
        if( TB_VERIFY(textSize+2<TB_ARRAY_COUNT_OF(dst.str)) ){
            if( textSize > 0 ) {
                if( TbModelDataPmx::ENCODE_UTF16 ==  reader.encode ) {
                    reader.reader.Read(textW,0,textSize);
                    textW[(textSize/2)] = L'\0';
                    TbStrCopy(dst.str,TB_ARRAY_COUNT_OF(dst.str),textW);
                }else{
                    reader.reader.Read(dst.str,0,textSize);
                    dst.str[textSize] = '\0';
                }
            }
        }
    }

    // モデル情報
    void LoadLocal( TbModelDataPmx::ModelInfo& dst , Reader& reader )
    {
        LoadLocal(dst.modelName,reader);
        LoadLocal(dst.modelNameEn,reader);
        LoadLocal(dst.comment,reader);
        LoadLocal(dst.commentEn,reader);
    }

    // 頂点
    void LoadLocal( TbModelDataPmx::VertexContainer& dst , Reader& reader , TbUint32 tmpUvExtendCount , TbUint32 boneIndexSize ) 
    {
        dst.uvExtendCount = tmpUvExtendCount;
        dst.vertexCount = reader.reader.ReadUint32();
        dst.list = TB_NEW TbModelDataPmx::VertexInfo[dst.vertexCount];
        if( dst.uvExtendCount > 0 ){
            dst.uvExtendList = TB_NEW TbVector2[dst.vertexCount*dst.uvExtendCount];
        }
        for( TbUint32 i = 0; i < dst.vertexCount; ++i ) {
            reader.reader.Read(dst.list[i].pos);
            reader.reader.Read(dst.list[i].normal);
            reader.reader.Read(dst.list[i].uv);
            if( dst.uvExtendCount > 0 ) {
                reader.reader.Read( &dst.uvExtendList[i*dst.uvExtendCount] , 
                             0 , 
                             sizeof(TbVector2)*dst.uvExtendCount ); 
            }
            reader.reader.Read(&dst.list[i].boneType,0,1);
            switch( dst.list[i].boneType )
            {
            case TbModelDataPmx::VERTEX_BONE_BDEF1:
                reader.reader.Read(&dst.list[i].bone[0],0,boneIndexSize);
                dst.list[i].weight[0] = 1.0f;
                break;
            case TbModelDataPmx::VERTEX_BONE_BDEF2:
                reader.reader.Read(&dst.list[i].bone[0],0,boneIndexSize);
                reader.reader.Read(&dst.list[i].bone[1],0,boneIndexSize);
                reader.reader.Read(dst.list[i].weight[0]);
                dst.list[i].weight[1] = 1.0f - dst.list[i].weight[0];
                break;
            case TbModelDataPmx::VERTEX_BONE_BDEF4:
                reader.reader.Read(&dst.list[i].bone[0],0,boneIndexSize);
                reader.reader.Read(&dst.list[i].bone[1],0,boneIndexSize);
                reader.reader.Read(&dst.list[i].bone[2],0,boneIndexSize);
                reader.reader.Read(&dst.list[i].bone[3],0,boneIndexSize);
                reader.reader.Read(dst.list[i].weight);
                break;
            case TbModelDataPmx::VERTEX_BONE_SDEF:
                reader.reader.Read(&dst.list[i].bone[0],0,boneIndexSize);
                reader.reader.Read(&dst.list[i].bone[1],0,boneIndexSize);
                reader.reader.Read(dst.list[i].weight[0]);
                reader.reader.Read(dst.list[i].c);
                reader.reader.Read(dst.list[i].r0);
                reader.reader.Read(dst.list[i].r1);
                break;
            default:
                TB_ASSERT(!"Invalid Type");
                break;
            };
            reader.reader.Read(dst.list[i].edgeScale);
        }
    }

    
    // インデックスリスト
    void LoadLocal( TbModelDataPmx::IndexContainer& dst ,  Reader& reader , TbUint32 vertexIndexSize ) 
    {
        dst.count = reader.reader.ReadUint32();
        dst.list = TB_NEW TbUint16[dst.count];
        for( TbUint32 i = 0; i < dst.count; ++i ) {
            TbUint32 index = 0;
            reader.reader.Read(&index,0,vertexIndexSize);
            dst.list[i] = static_cast<TbUint16>(index);
        }
    }

    // テクスチャ
    void LoadLocal( TbModelDataPmx::TextureContainer& dst ,  Reader& reader ) 
    {
        dst.count = reader.reader.ReadUint32();
        TB_ASSERT(dst.count <= TB_ARRAY_COUNT_OF(dst.pathList));
        for( TbUint32 i = 0; i < dst.count; ++i ) {
            LoadLocal(dst.pathList[i],reader);
        }
    }

    // マテリアル(個別)
    void LoadLocal( TbModelDataPmx::MaterialInfo& dst ,  Reader& reader , TbUint32 texIndexSize ) 
    {
        TbMemClear(dst);
        LoadLocal(dst.name,reader);
        LoadLocal(dst.nameEn,reader);
        reader.reader.Read(dst.diffuse);
        reader.reader.Read(dst.specular);
        reader.reader.Read(dst.specularCoef);
        reader.reader.Read(dst.ambient);
        dst.flag = reader.reader.ReadUint8();
        reader.reader.Read(dst.edgeColor);
        reader.reader.Read(dst.edgeSize);
        reader.reader.Read(&dst.textureIndex,0,texIndexSize);
        reader.reader.Read(&dst.sphereTextureIndex,0,texIndexSize);
        dst.sphereMode = reader.reader.ReadUint8();
        dst.toonFlag = reader.reader.ReadUint8();
        if( dst.toonFlag ) {
            dst.toonTextureIndex = reader.reader.ReadUint8();
        }else{
            reader.reader.Read(&dst.toonTextureIndex,0,texIndexSize); 
        }
        LoadLocal(dst.memo,reader);
        dst.indexCount = reader.reader.ReadUint32();
    }

    // マテリアル
    void LoadLocal( TbModelDataPmx::MaterialContainer& dst , Reader& reader , TbUint32 texIndexSize ) 
    {
        dst.count = reader.reader.ReadUint32();
        TB_ASSERT(dst.count<TB_ARRAY_COUNT_OF(dst.list));
        for( TbUint32 i = 0; i < dst.count; ++i ) {
            LoadLocal(dst.list[i],reader,texIndexSize);
        }
    }

    // 読みこみ
    void LoadLocal(  TbModelDataPmx::BoneContainer& dst , Reader& reader , TbUint32 boneIndexSize , TbUint8 encode ) 
    {

        dst.count = reader.reader.ReadUint32();
        dst.list = TB_NEW TbModelDataPmx::BoneInfo[dst.count];
        TbUint32 invalidIndex = TbModelDataPmx::GetInvalidIndex(boneIndexSize);

        // パース
        for( TbUint32 i = 0; i < dst.count; ++i ) {
            TbModelDataPmx::BoneInfo& bone = dst.list[i];
            TbMemClear(bone);
            // 名前
            LoadLocal(bone.name,reader);
            LoadLocal(bone.nameEn,reader);
            bone.nameHash = TbCrc32::GetHash(bone.name.str,TbStrBufSize(bone.name.str));

            // 位置
            reader.reader.Read(bone.pos);
            // 親
            reader.reader.Read(&bone.parentBoneIndex,0,boneIndexSize);
            if( invalidIndex != bone.parentBoneIndex ){
                bone.parent = &dst.list[bone.parentBoneIndex];
            }
            // 変形階層
            reader.reader.Read(bone.transformHie);
            // フラグ
            reader.reader.Read(&bone.flag,0,2);
            // 
            if( !(bone.flag & TbModelDataPmx::BoneInfo::FLAG_BIT_TARGET_BONE) ) {
                reader.reader.Read(bone.offset);
            }else{
                reader.reader.Read(&bone.targetBoneIndex,0,boneIndexSize);
                if( invalidIndex != bone.targetBoneIndex ) {
                    bone.targetBone = &dst.list[bone.targetBoneIndex];
                }
            }
            // 付与情報
            if( bone.flag & (TbModelDataPmx::BoneInfo::FLAG_BIT_ROT_ADD|TbModelDataPmx::BoneInfo::FLAG_BIT_TRANSLATE_ADD) ) {
                TbUint32 offsetParentIndex = 0;
                reader.reader.Read(&offsetParentIndex,0,boneIndexSize);
                reader.reader.Read(bone.offsetParentRatio);
            }
            // 固定軸
            if( bone.flag & TbModelDataPmx::BoneInfo::FLAG_BIT_AXIS_FIX ) {
                reader.reader.Read(bone.fixedVec);
            }
            // ローカル軸
            if( bone.flag & TbModelDataPmx::BoneInfo::FLAG_BIT_AXIS_LOCAL ) {
                reader.reader.Read(bone.localVecX);
                reader.reader.Read(bone.localVecZ);
            }else{
                bone.localVecX.SetX(1.0f);
                bone.localVecX.SetZ(1.0f);
            }

            // 外部親
            if( bone.flag & TbModelDataPmx::BoneInfo::FLAG_BIT_EXTERNAL_PARENT ) {
                reader.reader.Read(bone.externalParentKey);
            }
            // IK情報
            if( bone.flag & TbModelDataPmx::BoneInfo::FLAG_BIT_IK ) {
                reader.reader.Read(&bone.ikTargetBoneIndex,0,boneIndexSize);
                if( invalidIndex != bone.ikTargetBoneIndex ){
                    bone.ikTargetBone = &dst.list[bone.ikTargetBoneIndex];
                }
                reader.reader.Read(bone.ikLoopCount);
                reader.reader.Read(bone.ikConstraintRad);
                reader.reader.Read(bone.ikLinkCount);
                for( TbUint32 i = 0; i < bone.ikLinkCount; ++i ) {
                    reader.reader.Read(&bone.ikLink[i].linkBoneIndex,0,boneIndexSize);
                    if( invalidIndex != bone.ikLink[i].linkBoneIndex ){
                        bone.ikLink[i].linkBone = &dst.list[bone.ikLink[i].linkBoneIndex];
                    }
                    reader.reader.Read(&bone.ikLink[i].constraintRadFlag,0,1);
                    if( bone.ikLink[i].constraintRadFlag ) {
                        reader.reader.Read(bone.ikLink[i].constraintRadMin);
                        reader.reader.Read(bone.ikLink[i].constraintRadMax);
                    }
                }
            }
        }
        
        // セットアップ
        for( TbUint32 i = 0; i < dst.count; ++i ) {
            TbModelDataPmx::BoneInfo& bone = dst.list[i];
            bone.localPoseMatrix.Unit();
            bone.localPoseMatrix.Translate(bone.pos);
            TbModelDataPmx::BoneInfo* parent = bone.parent;
            if( parent ) {
                bone.localPoseMatrix.Translate(-parent->pos);
            }
        }

        // グローバル座標系
        for( TbUint32 i = 0; i < dst.count; ++i ) {
            TbModelDataPmx::BoneInfo& bone = dst.list[i];
            TbModelDataPmx::BoneInfo* parent = bone.parent;
            bone.globalPoseMatrix = bone.localPoseMatrix;
            while( parent ){
                bone.globalPoseMatrix = parent->localPoseMatrix * bone.globalPoseMatrix;
                parent = parent->parent;
            }
            bone.globalPoseMatrix.Invert( bone.invGlobalPoseMatrix );
        }
    }


    // 古いシェーダモデル用に最適化(マテリアル分割)
    void OptimizeForOldShaderModel( TbModelDataPmx::MaterialContainer& dst , const TbModelDataPmx::VertexContainer& vertex , const TbModelDataPmx::IndexContainer& indexBuffer )
    {
        static const TbUint32 BONE_MATRIX_MAX = 16;
        struct NewMaterialInfo
        {
            void Set( TbModelDataPmx::MaterialInfo& s , const TbStaticSet<TbUint32,32>& weightedBoneSet , TbUint32 tmpIndexCount )
            {
                src = s;
                src.indexCount = tmpIndexCount;
                TbStaticSet<TbUint32,32>::ConstIterator ite = weightedBoneSet.Begin();
                TbStaticSet<TbUint32,32>::ConstIterator endIte = weightedBoneSet.End();
                for( ; ite != endIte; ++ite ) {
                    src.weightedBoneList[src.weightedBoneCount++] = *ite;
                }
            };
            TbModelDataPmx::MaterialInfo src;
        };
        TbUint32 newMaterialCount = 0;
        NewMaterialInfo* newMaterial = TB_NEW NewMaterialInfo[256]; // 最大分割1024
        TbUint32 startIndex = 0;
        for( TbUint32 i = 0; i < dst.count; ++i ){
            TbUint32 offsetIndex = 0;
            TbStaticSet<TbUint32,32> weightedBoneList;
            for( TbUint32 j = 0; j < dst.list[i].indexCount; ++j ){
                const TbModelDataPmx::VertexInfo& targetVertex = vertex.list[indexBuffer.list[startIndex+j]];
                TbUint32 boneCount = targetVertex.GetValidBoneCount();
                for( TbUint32 boneIdx = 0; boneIdx < boneCount; ++boneIdx ){
                    weightedBoneList.Insert(targetVertex.bone[boneIdx]);
                    if( weightedBoneList.GetCount() >= BONE_MATRIX_MAX ) {
                        j = j-j%3;
                        newMaterial[newMaterialCount++].Set( dst.list[i] , weightedBoneList , j-offsetIndex );
                        // やり直す
                        offsetIndex = j;
                        weightedBoneList.Clear();
                        --j;
                        break;
                    }
                }
            }
            newMaterial[newMaterialCount++].Set( dst.list[i] , weightedBoneList , dst.list[i].indexCount-offsetIndex );
            weightedBoneList.Clear();
            startIndex += dst.list[i].indexCount;
        }
        dst.count = newMaterialCount;
        for( TbUint32 i = 0; i < newMaterialCount; ++i ) {
            dst.list[i] = newMaterial[i].src;
        }
        TB_SAFE_DELETE_ARRAY(newMaterial);
    };

}

/*!
 * 読みこみ
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbModelDataPmx::Load( const void* buffer , TbSizeT bufferSize )
{
    TbConstMemoryStream stream(buffer,bufferSize);
    Reader reader(&stream);
    reader.reader.Read(header);    
    LoadLocal(setting,reader);
    reader.encode = static_cast<Encode>(setting.encode);

    LoadLocal(modelInfo,reader);
    LoadLocal(vertex,reader,setting.unExtendCount,setting.boneIndexSize);
    LoadLocal(index,reader,setting.vertexIndexSize);
    LoadLocal(tex,reader);
    LoadLocal(material,reader,setting.texIndexSize);
    LoadLocal(bone,reader,setting.boneIndexSize,setting.encode);

    // 最適化
#if TB_PMX_BUILD_FOR_SM_3_0
    OptimizeForOldShaderModel(material,vertex,index);
#endif
}

}
