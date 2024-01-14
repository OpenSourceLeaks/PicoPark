/*!
 * 共通ユーティリティ関数軍
 * @author teco
 */

#ifndef _INCLUDED_CR_UTIL_H_
#define _INCLUDED_CR_UTIL_H_

#include "cr_def.h"
#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>

namespace crayon
{

class CrUtil
{
public:

    // 接触面毎の法線
    static const toybox::TbVector2& GetContactFaceNormal( CrContactFace face ) {
        static const toybox::TbVector2 OFS[] = 
        {
            toybox::TbVector2(0.0f,-1.0f) , // CR_CONTACT_UP
            toybox::TbVector2(0.0f,1.0f) , // CR_CONTACT_DOWN
            toybox::TbVector2(-1.0f,0.0f) , // CR_CONTACT_LEFT
            toybox::TbVector2(1.0f,0.0f) , // CR_CONTACT_RIGHT
        };
        return OFS[face];
    }

    // 接触タイプの逆指定
    static CrContactFace GetReverseContactFace( CrContactFace type )
    {
        if( 0 <= type && type < CR_CONTACT_MAX ) {
            static const CrContactFace TABLE[] = {
                CR_CONTACT_DOWN ,
                CR_CONTACT_UP , 
                CR_CONTACT_RIGHT , 
                CR_CONTACT_LEFT 
            };
            return TABLE[type];
        }
        return CR_CONTACT_INVALID;
    }

    // 接触面毎の法線
    static CrContactFace GetContactFace( const toybox::TbVector2& normal ) {
        static const toybox::TbVector2 OFS[] = 
        {
            toybox::TbVector2(0.0f,-1.0f) , // CR_CONTACT_UP
            toybox::TbVector2(0.0f,1.0f) , // CR_CONTACT_DOWN
            toybox::TbVector2(-1.0f,0.0f) , // CR_CONTACT_LEFT
            toybox::TbVector2(1.0f,0.0f) , // CR_CONTACT_RIGHT
        };
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(OFS); ++i ){
            if( OFS[i] == normal ) {
                return static_cast<CrContactFace>(i);
            }
        }
        return CR_CONTACT_INVALID;
    }

};

}

#endif
