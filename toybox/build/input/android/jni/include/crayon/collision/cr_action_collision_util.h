/*!
 * コリジョン
 * @author teco
 */

#ifndef _INCLUDED_CR_ACTION_COLLISION_UTIL_H_
#define _INCLUDED_CR_ACTION_COLLISION_UTIL_H_

#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector3.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_array.h>
#include <base/util/tb_function.h>
#include <crayon/cr_util.h>

namespace crayon
{

class CrActionCollisionUtil
{
public:

    static TbBool IsCollidedRect(
        TbRectF32 colli1,
        TbRectF32 colli2,
        const TbVector2* ofs1 = nullptr,
        const TbVector2* ofs2 = nullptr)
    {
        if (ofs1) {
            colli1.x += ofs1->GetX();
            colli1.y += ofs1->GetY();
        }
        if (ofs2) {
            colli2.x += ofs2->GetX();
            colli2.y += ofs2->GetY();
        }
        if (colli1.x < (colli2.x + colli2.width) &&
            colli2.x < (colli1.x + colli1.width) &&
            colli1.y < (colli2.y + colli2.height) &&
            colli2.y < (colli1.y + colli1.height))
        {
            return TB_TRUE;
        }
        return TB_FALSE;
    }

};

}

#endif
