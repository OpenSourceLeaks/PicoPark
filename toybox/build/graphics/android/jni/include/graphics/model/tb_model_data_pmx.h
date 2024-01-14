/*!
 * PMXデータ(MMDのモデルファイルフォーマット)
 * @author Miyake_Shunsuke
 * @since 2014.03.31
 */

#ifndef _INCLUDED_TB_MODEL_DATA_PMX_H_
#define _INCLUDED_TB_MODEL_DATA_PMX_H_

#include <base/math/tb_matrix.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector3.h>
#include <base/algorithm/tb_search.h>

// シェーダモデル3.0以下GPU用ビルドか(頂点シェーダの定数レジスタ256制約)
#define TB_PMX_BUILD_FOR_SM_3_0 (1)

namespace toybox
{

// 頂点フォーマット
struct TbVertexPmx
{
    TbVector3  pos;
    TbVector3  norm;
    TbVector2  uv;
    TbUint8    boneIndex[4];
    TbFloat32  boneWeight[4];
};

// PMX用モデルデータ
struct TbModelDataPmx
{
    // 頂点のボーン制御タイプ
    enum VertexBoneType
    {
        VERTEX_BONE_BDEF1 , 
        VERTEX_BONE_BDEF2 , 
        VERTEX_BONE_BDEF4 , 
        VERTEX_BONE_SDEF , 
    };

    enum Encode
    {
        ENCODE_UTF16 ,
        ENCODE_UTF8
    };

    // ヘッダー
    struct Header
    {
        char      tag[4];
        TbFloat32 version;
    };
    
    // 設定
    struct Setting
    {
        TbUint8 dataSize;

        union
        {
            TbUint8     data[8];
            struct {
                TbUint8 encode;             // エンコード方式 0:UTF16 1:UTF8
                TbUint8 unExtendCount;      // 追加UV数
                TbUint8 vertexIndexSize;    // 頂点Indexサイズ(1,2,4)
                TbUint8 texIndexSize;       // テクスチャIndexサイズ(1,2,4)
                TbUint8 materialIndexSize;  // マテリアルIndexサイズ(1,2,4)
                TbUint8 boneIndexSize;      // ボーンIndexサイズ(1,2,4)
                TbUint8 morphIndexSize;     // モーフIndexサイズ(1,2,4)
                TbUint8 rigitIndexSize;     // 剛体Indexサイズ(1,2,4)
            };
        };

        TbUint8 padding_[3];
    };
    
    // テキスト
    struct Text
    {
        static const TbUint32 TEXT_MAX = 512;
        TbChar8  str[TEXT_MAX];
    };
    
    // モデル情報
    struct ModelInfo
    {
        Text modelName;
        Text modelNameEn;
        Text comment;
        Text commentEn;
    };
    
    // 頂点情報
    struct VertexInfo
    {
        // コンストラクタ
        VertexInfo() {
            TbMemClear(*this);
        }

        // 有効な骨数
        TbUint32 GetValidBoneCount() const {
            static const TbUint32 BONE_COUNT_TABLE[] =
            {
                1 , // VERTEX_BONE_BDEF1
                2 , // VERTEX_BONE_BDEF2
                4 , // VERTEX_BONE_BDEF4
                2   // VERTEX_BONE_SDEF
            };
            TbUint32 boneCount = 0;
            if( boneType < TB_ARRAY_COUNT_OF(BONE_COUNT_TABLE) ){
                boneCount = BONE_COUNT_TABLE[boneType];
            }
            return boneCount;
        }

        TbFloat32 pos[3];
        TbFloat32 normal[3];
        TbVector2 uv;
        TbUint32  boneType;
        TbUint32  bone[4];
        TbFloat32 weight[4];
        TbFloat32 c[3];
        TbFloat32 r0[3];
        TbFloat32 r1[3];
        TbFloat32 edgeScale;
    };
    
    // 頂点格納データ
    struct VertexContainer
    {
        static const TbUint32 WEIGHTED_BONE_MAX = 128;

        // コンストラクタ
        VertexContainer()
            : vertexCount(0)
            , uvExtendCount(0)
            , list(nullptr)
            , uvExtendList(nullptr)
        {
        }

        // デストラクタ
        ~VertexContainer()
        {
            TB_SAFE_DELETE_ARRAY(list);
            TB_SAFE_DELETE_ARRAY(uvExtendList);
        }

        TbUint32         vertexCount;   // 頂点数
        TbUint32         uvExtendCount; // 拡張UV数
        VertexInfo*      list;          // 頂点
        TbVector2*       uvExtendList;  // 拡張UV
    };

        // 面格納データ(3頂点で1ポリゴン)
    struct IndexContainer
    {
        // コンストラクタ
        IndexContainer() : count(0) , list(nullptr) {}

        // デストラクタ
        ~IndexContainer() { TB_SAFE_DELETE_ARRAY(list); }

        TbUint32  count;
        TbUint16* list;
    };

    // テクスチャ情報格納
    struct TextureContainer
    {
        static const TbSizeT MAX = 50;
        TbUint32    count;
        Text        pathList[MAX];
    };

    // マテリアル
    struct MaterialInfo
    {
        // ウェイト対象のボーンインデックス取得
        TbUint8 GetWeigtedBoneIndex( TbUint32 bone ) {
            TbUint32 index = TbFind(weightedBoneList,weightedBoneList+weightedBoneCount,bone)-weightedBoneList;
            TB_ASSERT(index<weightedBoneCount);
            return static_cast<TbUint8>(index);
        }

        Text name;
        Text nameEn;
        TbFloat32    diffuse[4];
        TbFloat32    specular[3];
        TbFloat32    specularCoef;
        TbFloat32    ambient[3];
        TbUint32     flag;
        TbFloat32    edgeColor[4];
        TbFloat32    edgeSize;
        TbUint32     textureIndex;
        TbUint32     sphereTextureIndex;
        TbUint32     sphereMode;
        TbUint32     toonFlag;
        TbUint32     toonTextureIndex;
        Text         memo;
        TbUint32     indexCount;

        // シェーダモデル3.0以下用の変数
        TbUint32     weightedBoneCount;
        TbUint32     weightedBoneList[20];
    };

    // マテリアル格納
    struct MaterialContainer
    {
        static const TbSizeT MAX = 128;
        TbUint32       count;
        MaterialInfo   list[MAX];
    };
    
    struct BoneInfo;

    // IKリンク
    struct IkLink
    {
        TbUint32        linkBoneIndex;
        BoneInfo*       linkBone;
        TbUint32        constraintRadFlag;
        TbFloat32       constraintRadMin[3];
        TbFloat32       constraintRadMax[3];
    };

    // 骨格情報
    struct BoneInfo
    {
        enum FlagBit
        {
            FLAG_BIT_TARGET_BONE    = TB_BIT(0) ,
            FLAG_BIT_ROT            = TB_BIT(1) ,
            FLAG_BIT_TRANSLATE      = TB_BIT(2) ,
            FLAG_BIT_VISIBLE        = TB_BIT(3) ,
            FLAG_BIT_CONTROL        = TB_BIT(4) ,
            FLAG_BIT_IK             = TB_BIT(5) ,
            FLAG_BIT_LOCAL_ADD      = TB_BIT(7) ,
            FLAG_BIT_ROT_ADD        = TB_BIT(8) ,
            FLAG_BIT_TRANSLATE_ADD  = TB_BIT(9) ,
            FLAG_BIT_AXIS_FIX       = TB_BIT(10) ,
            FLAG_BIT_AXIS_LOCAL     = TB_BIT(11) ,
            FLAG_BIT_AFTER_PHYSIX   = TB_BIT(12) ,
            FLAG_BIT_EXTERNAL_PARENT= TB_BIT(13) ,
        };

        Text           name;
        Text           nameEn;
        TbUint32       nameHash;
        TbVector3      pos;
        TbUint32       parentBoneIndex;
        BoneInfo*      parent;
        TbUint32       transformHie;
        TbUint32       flag;
        TbFloat32      offset[3];
        TbUint32       targetBoneIndex;
        BoneInfo*      targetBone;
        BoneInfo*      offsetParent;
        TbFloat32      offsetParentRatio;           // 付与率
        TbVector3      fixedVec[3];       // 方向ベクトル
        TbVector3      localVecX;         // X軸の方向ベクトル
        TbVector3      localVecZ;         // Z軸の方向ベクトル
        TbUint32       externalParentKey;
        TbUint32       ikTargetBoneIndex;
        BoneInfo*      ikTargetBone;
        TbUint32       ikLoopCount;
        TbFloat32      ikConstraintRad;
        TbUint32       ikLinkCount;
        IkLink         ikLink[16];        // 最大4つまで
        TbBool         isKnee;
        TbMatrix       localPoseMatrix;       // ローカル姿勢行列
        TbMatrix       globalPoseMatrix;      // グローバル姿勢行列
        TbMatrix       invGlobalPoseMatrix;   // グローバル姿勢逆行列
    };

    // ボーンデータ
    struct BoneContainer
    {
        // コンストラクタ
        BoneContainer() : count(0) , list(nullptr) {}

        // デストラクタ
        ~BoneContainer() { TB_SAFE_DELETE_ARRAY(list); }

        TbUint32   count;
        BoneInfo*  list;
    };

    // 無効なインデックス取得
    static TbUint32 GetInvalidIndex( TbUint32 indexSize ) {
        static const TbUint32 INVALID_TABLE[] = 
        {
            0xFF , 
            0xFFFF ,
            0xFFFFFF ,
            0xFFFFFFFF ,
        };
        if( indexSize-1 < TB_ARRAY_COUNT_OF(INVALID_TABLE) ) {
            return INVALID_TABLE[indexSize-1];
        }
        return INVALID_TABLE[3];
    }

    // 有効なインデックスか
    static TbBool IsValidIndex( TbUint32 indexSize , TbUint32 index ) {
        return index != GetInvalidIndex(indexSize);
    }
    
        // テクスチャ数取得
    TbUint32 GetTexCount() const {
        return tex.count;
    }

    // テクスチャファイル名取得
    const char* GetTexFileName( TbUint32 index ) const {
        if( index < tex.count ) {
            return tex.pathList[index].str;
        }
        return nullptr;
    }

    // 有効な頂点インデックスか
    TbBool IsValidVertexIndex( TbUint32 index ) {
        return IsValidIndex(setting.vertexIndexSize,index);
    }

    // 有効なテクスチャインデックスか
    TbBool IsValidTexIndex( TbUint32 index ) {
        return IsValidIndex(setting.texIndexSize,index);
    }

    // 有効なマテリアルインデックスか
    TbBool IsValidMaterialIndex( TbUint32 index ) {
        return IsValidIndex(setting.materialIndexSize,index);
    }

    // 有効なボーンインデックスか
    TbBool IsValidBoneIndex( TbUint32 index ) {
        return IsValidIndex(setting.boneIndexSize,index);
    }
    
    // 有効なモーフインデックスか
    TbBool IsValidMorphIndex( TbUint32 index ) {
        return IsValidIndex(setting.morphIndexSize,index);
    }

    // 有効な剛体インデックスか
    TbBool IsValidRigitIndex( TbUint32 index ) {
        return IsValidIndex(setting.rigitIndexSize,index);
    }

    // 読みこみ
    void Load( const void* buffer , TbSizeT bufferSize );
    
    Header             header;
    Setting            setting;
    ModelInfo          modelInfo;
    VertexContainer    vertex;
    IndexContainer     index;
    TextureContainer   tex;
    MaterialContainer  material;
    BoneContainer      bone;    
};

}

#endif
