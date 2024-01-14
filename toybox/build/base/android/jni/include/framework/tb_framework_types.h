/*!
 * フレーム
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_FRAMEWORK_TYPES_H_
#define _INCLUDED_TB_FRAMEWORK_TYPES_H_

namespace toybox
{

// フレームワーク初期化パラメータ
struct TbFrameworkInitParam
{
    enum AllocType
    {
        ALLOC_TYPE_GLOBAL , 
        ALLOC_TYPE_COMMAND_BUFFER , 
        ALLOC_TYPE_MAX , 
    };

    struct AllocParam
    {
        AllocParam() : buf(nullptr) , bufSize(0) 
        {}
        void*    buf;
        TbUint32 bufSize;
    };

    TbFrameworkInitParam()
        : renderViewMax(0)
        , renderPassMax(0)
        , taskLineMax(0)
        , rootPath(nullptr)
        , savePath(nullptr)
    {}

    AllocParam  allocParam[ALLOC_TYPE_MAX]; // アロケータパラメータ
    TbUint32    renderViewMax;              // 最大ビュー数
    TbUint32    renderPassMax;              // レンダリングパス数
    TbUint32    taskLineMax;                // タスクライン数
    const char* rootPath;                   // ルートパス
    const char* savePath;                   // セーブパス
};

// CUI版アプリケーション
struct TbFrameworkCuiInitParam
{
    enum AllocType
    {
        ALLOC_TYPE_GLOBAL , 
        ALLOC_TYPE_MAX , 
    };
    struct AllocParam
    {
        void*    buf;
        TbUint32 bufSize;
    };
    AllocParam  allocParam[ALLOC_TYPE_MAX]; // アロケータパラメータ
    TbUint32    taskLineMax;                // タスクライン数
    const char* rootPath;                   // ルートパス
};

}

#endif
