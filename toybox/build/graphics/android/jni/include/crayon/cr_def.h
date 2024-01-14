/*!
 * crayon全体定義
 * @author teco
 */

#ifndef _INCLUDED_CR_DEF_H_
#define _INCLUDED_CR_DEF_H_

// 最大プレイヤー数
#define CR_PLAYER_MAX (10)

// スマホなどでリモート操作が可能か
#define CR_REMOTE_INPUT_IS_ENABLE (0)

// 奥行き
#define CR_SPRITE_Z_FRONT_7 (-0.7f)
#define CR_SPRITE_Z_FRONT_6 (-0.6f)
#define CR_SPRITE_Z_FRONT_5 (-0.5f)
#define CR_SPRITE_Z_FRONT_4 (-0.4f)
#define CR_SPRITE_Z_FRONT_3 (-0.3f)
#define CR_SPRITE_Z_FRONT_2 (-0.2f)
#define CR_SPRITE_Z_FRONT_1 (-0.1f)
#define CR_SPRITE_Z_BACK_1  (0.1f)
#define CR_SPRITE_Z_BACK_2  (0.2f)
#define CR_SPRITE_Z_BACK_3  (0.3f)

namespace crayon
{

// 接触面
enum CrContactFace
{
    CR_CONTACT_INVALID = -1 ,
    CR_CONTACT_UP , 
    CR_CONTACT_DOWN , 
    CR_CONTACT_LEFT ,
    CR_CONTACT_RIGHT , 
    CR_CONTACT_MAX ,
};

}

#endif
