/*!
 * @file
 * @brief NXでのセーブファイル用ファイルマッパークラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_MAPPER_SAVE_NX_H_
#define _INCLUDED_TB_FILE_MAPPER_SAVE_NX_H_

#include "tb_file_mapper_nx.h"

namespace toybox
{

class TbFileMapperSaveNX : public TbFileMapper
{
    typedef TbFileMapperNX Super;
public:

    // コンストラクタ
    TbFileMapperSaveNX() : Super() 
    {}

    // デストラクタ
    virtual ~TbFileMapperSaveNX()
    {}

public:
    
    // 対応しているかパスIDかどうか
    virtual TbBool IsEnable( TbFilePathId pathId ) const TB_OVERRIDE {
        return pathId == TB_FILE_PATH_ID_PRESET_SAVE;
    }

};

}

#endif
