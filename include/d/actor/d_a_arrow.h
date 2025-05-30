#ifndef D_A_ARROW_H
#define D_A_ARROW_H

#include "d/actor/d_a_player.h"
#include "d/d_cc_d.h"
#include "d/d_particle.h"
#include "f_op/f_op_actor_mng.h"

/**
 * @ingroup actors-unsorted
 * @class daArrow_c
 * @brief Arrow
 *
 * @details
 *
 */
class daArrow_c : public fopAc_ac_c {
public:
    enum ArrowType_e {
        ARROW_TYPE_NORMAL = 0, // Needs verification
        ARROW_TYPE_BOMB = 1,
        ARROW_TYPE_LIGHT = 2,
        ARROW_TYPE_SLING = 4,
    };

    /* 80499C6C */ int createHeap();
    /* 80499D8C */ void atHitCallBack(dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);
    /* 80499FBC */ void decAlphaBlur();
    /* 8049A04C */ void setBlur();
    /* 8049A110 */ void setLightArrowHitMark(cXyz const*);
    /* 8049A1EC */ void setLightChargeEffect(int);
    /* 8049A370 */ int setArrowWaterNextPos(cXyz*, cXyz*);
    /* 8049A76C */ void setArrowAt(f32);
    /* 8049A9CC */ void arrowShooting();
    /* 8049AC84 */ void setBombArrowExplode(cXyz*);
    /* 8049ACE0 */ void setRoomInfo();
    /* 8049AD74 */ void clearNearActorData();
    /* 8049AD94 */ s16 getVibAngle();
    /* 8049AE64 */ void setNormalMatrix();
    /* 8049AF18 */ void setSmokePos();
    /* 8049AFEC */ void setKeepMatrix();
    /* 8049B120 */ fopAc_ac_c* setStopActorMatrix();
    /* 8049B214 */ void setBombMoveEffect();
    /* 8049B370 */ bool checkReget();
    /* 8049B45C */ int procWait();
    /* 8049B764 */ int procMove();
    /* 8049C700 */ int procReturnInit(int);
    /* 8049C874 */ int procReturn();
    /* 8049CB70 */ int procBGStop();
    /* 8049CC60 */ int procActorStop();
    /* 8049CCCC */ int procActorControllStop();
    /* 8049CCF0 */ int procSlingHitInit(cXyz*, dCcD_GObjInf*);
    /* 8049CE50 */ int procSlingHit();
    /* 8049CEA0 */ int execute();
    /* 8049D0CC */ int draw();
    /* 8049D440 */ cPhs__Step create();
    /* 8049D98C */ ~daArrow_c();

    void setBombArrowExplode() { field_0x950 = 0; }
    bool checkBombArrow() const { return mArrowType == true; }
    u32 getHitAcID() { return mHitAcID; }

    BOOL checkWait() { return fopAcM_GetParam(this) == 0; }

    f32 getOutLengthRate() { return mOutLengthRate; }
    f32 getArrowOutLength() { return 95.0f; }
    f32 getArrowAtR() { return 5.0f; }
    f32 getFlyMax() { return mFlyMax; }
    cXyz getStartPos() { return mStartPos; }

    void setShoot() { fopAcM_SetParam(this, 1); }
    void setChargeShoot() { fopAcM_SetParam(this, 2); }

    void deleteArrow() { field_0x93f = 1; }

    static fopAc_ac_c* makeArrow(fopAc_ac_c* i_actor, u16 param_1) {
        return (fopAc_ac_c*)fopAcM_fastCreate(PROC_ARROW,
                                              param_1 << 8,
                                              &i_actor->current.pos,
                                              fopAcM_GetRoomNo(i_actor),
                                              NULL,
                                              NULL,
                                              -1,
                                              NULL,
                                              NULL);
    }

    static fopAc_ac_c* makeSlingStone(fopAc_ac_c* i_actor, cXyz* i_pos) {
        return (fopAc_ac_c*)fopAcM_fastCreate(PROC_ARROW,
                                              0x401,
                                              i_pos,
                                              fopAcM_GetRoomNo(i_actor),
                                              NULL,
                                              NULL,
                                              -1,
                                              NULL,
                                              NULL);
    }

private:
    /* 0x568 */ J3DModel* mpModel;
    /* 0x56C */ dBgS_ArrowLinChk field_0x56c;
    /* 0x5DC */ dBgS_ObjLinChk field_0x5dc;
    /* 0x64C */ dCcD_Stts field_0x64c;
    /* 0x688 */ dCcD_Cps field_0x688;
    /* 0x7CC */ dCcD_Sph field_0x7cc;
    /* 0x904 */ Z2SoundObjArrow mSoundObjArrow;
    /* 0x93C */ u8 mArrowType;
    /* 0x93D */ u8 field_0x93d;
    /* 0x93E */ s8 field_0x93e;
    /* 0x93F */ u8 field_0x93f;
    /* 0x940 */ u8 field_0x940;
    /* 0x941 */ u8 field_0x941;
    /* 0x942 */ u8 field_0x942;
    /* 0x943 */ u8 field_0x943;
    /* 0x944 */ u8 field_0x944;
    /* 0x945 */ u8 field_0x945;
    /* 0x946 */ u8 field_0x946;
    /* 0x947 */ u8 field_0x947;
    /* 0x948 */ s8 mReverb;
    /* 0x949 */ u8 field_0x949[0x94C - 0x949];
    /* 0x94C */ u16 field_0x94c;
    /* 0x94E */ s16 field_0x94e;
    /* 0x950 */ s16 field_0x950;
    /* 0x952 */ s16 field_0x952;
    /* 0x954 */ s16 field_0x954;
    /* 0x956 */ s16 field_0x956;
    /* 0x958 */ csXyz field_0x958;
    /* 0x960 */ int field_0x960;
    /* 0x964 */ u32 field_0x964;
    /* 0x968 */ u32 field_0x968;
    /* 0x96C */ u32 field_0x96c[4];
    /* 0x97C */ u32 field_0x97c[6];
    /* 0x994 */ u32 mHitAcID;
    /* 0x998 */ f32 field_0x998;
    /* 0x99C */ f32 field_0x99c;
    /* 0x9A0 */ f32 mOutLengthRate;
    /* 0x9A4 */ f32 mFlyMax;
    /* 0x9A8 */ cXyz field_0x9a8;
    /* 0x9B4 */ cXyz field_0x9b4;
    /* 0x9C0 */ cXyz mStartPos;
    /* 0x9CC */ cXyz field_0x9cc;
    /* 0x9D8 */ cXyz field_0x9d8;
    /* 0x9E4 */ cXyz field_0x9e4;
    /* 0x9F0 */ cXyz mSmokePos;
    /* 0x9FC */ cXyz field_0x9fc;
    /* 0xA08 */ daPy_actorKeep_c field_0xa08;
    /* 0xA10 */ dPa_hermiteEcallBack_c field_0xa10;
    /* 0xA28 */ int (daArrow_c::*mProcFunc)();

    static s16 m_count;
};

STATIC_ASSERT(sizeof(daArrow_c) == 0xA34);

#endif /* D_A_ARROW_H */
