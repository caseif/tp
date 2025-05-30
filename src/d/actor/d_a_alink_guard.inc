/**
 * d_a_alink_guard.inc
 * Player Shield action handling
 */

#include "d/actor/d_a_alink.h"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_obj_gra2.h"

/* 800DC548-800DC5A4 0D6E88 005C+00 13/13 0/0 0/0 .text checkEnemyGroup__9daAlink_cFP10fopAc_ac_c
 */
BOOL daAlink_c::checkEnemyGroup(fopAc_ac_c* i_actor) {
    return i_actor != NULL && (fopAcM_GetGroup(i_actor) == fopAc_ENEMY_e ||
                               fopAcM_GetName(i_actor) == PROC_E_ZH || checkSpecialNpc(i_actor));
}

// properly setup later
class daNpc_Kn_c : public fopAc_ac_c {
public:
    bool checkBattleMode() { return true; }
};

/* 800DC5A4-800DC5DC 0D6EE4 0038+00 4/4 0/0 0/0 .text checkSpecialNpc__9daAlink_cFP10fopAc_ac_c */
BOOL daAlink_c::checkSpecialNpc(fopAc_ac_c* i_actor) {
    s16 name = fopAcM_GetName(i_actor);

    return ((name == PROC_NPC_KAKASHI || name == PROC_NPC_KN) &&
            static_cast<daNpc_Kn_c*>(i_actor)->checkBattleMode()) ||
           (name == PROC_OBJ_GRA && !static_cast<daObj_GrA_c*>(i_actor)->checkNpcObjGra());
}

/* 800DC5DC-800DC678 0D6F1C 009C+00 1/1 0/0 0/0 .text checkShieldAttackEmphasys__9daAlink_cFv */
BOOL daAlink_c::checkShieldAttackEmphasys() {
    return mTargetedActor != NULL && (mTargetedActor->attention_info.flags & 0x200000) &&
           !checkCutHeadState() &&
           (fopAcM_GetGroup(mTargetedActor) != fopAc_ENEMY_e ||
            !static_cast<fopEn_enemy_c*>(mTargetedActor)->checkDownFlg());
}

/* 800DC678-800DC79C 0D6FB8 0124+00 5/5 0/0 0/0 .text checkGuardActionChange__9daAlink_cFv */
BOOL daAlink_c::checkGuardActionChange() {
    return checkAttentionLock() && !checkIronBallWaitAnime() && !checkGrabAnime() &&
           !checkCopyRodControllAnime() && !((mTargetedActor != NULL && !checkSpecialNpc(mTargetedActor)) &&
           (fopAcM_GetGroup(mTargetedActor) == 3 || fopAcM_GetGroup(mTargetedActor) == fopAc_NPC_e));
}

/* 800DC79C-800DC8C4 0D70DC 0128+00 3/3 0/0 0/0 .text            stickArrowIncrement__9daAlink_cFi
 */
void daAlink_c::stickArrowIncrement(int param_0) {
    if (!checkNoResetFlg2(FLG2_STATUS_WINDOW_DRAW)) {
        s16 arrow_num = 0;

        for (int i = 0; i < 16; i++) {
            if (mShieldArrowIDs[i] != -1) {
                if (field_0x2f7c[i] == 0 || param_0) {
                    arrow_num++;
                    mShieldArrowIDs[i] = -1;
                    field_0x2f7c[i] = 0;
                } else {
                    field_0x2f7c[i]--;
                }
            }
        }

        if (arrow_num != 0) {
            if (checkShieldGet() && checkItemGet(fpcNm_ITEM_BOW, true)) {
                int arrow_type = arrow_num == 1 ? fpcNm_ITEM_ARROW_1 : fpcNm_ITEM_ARROW_10;

                dComIfGp_setItemArrowNumCount(arrow_num);
                fopAcM_createItemForSimpleDemo(&current.pos, arrow_type, -1, NULL, NULL, 0.0f,
                                               0.0f);
            }

            onResetFlg0(RFLG0_UNK_1);
        }
    }
}


/* 800DC8C4-800DCA2C 0D7204 0168+00 0/0 0/0 1/1 .text
 * setArrowShieldActor__9daAlink_cFP10fopAc_ac_ci               */
void daAlink_c::setArrowShieldActor(fopAc_ac_c* i_actor, int param_1) {
    fpc_ProcID new_arrow_id = fopAcM_GetID(i_actor);

    int arrow_num = 0;
    for (; arrow_num < 16; arrow_num++) {
        if (mShieldArrowIDs[arrow_num] == -1) {
            mShieldArrowIDs[arrow_num] = new_arrow_id;
            field_0x2f7c[arrow_num] = 10;
            break;
        }
    }

    if (arrow_num == 16) {
        fopAc_ac_c* first_arrow = fopAcM_SearchByID(mShieldArrowIDs[0]);
        if (first_arrow != NULL) {
            fopAcM_delete(first_arrow);
        }

        int i = 0;
        for (; i < 15; i++) {
            mShieldArrowIDs[i] = mShieldArrowIDs[i + 1];
            field_0x2f7c[i] = field_0x2f7c[i + 1];
        }

        mShieldArrowIDs[i] = new_arrow_id;
        field_0x2f7c[i] = 10;

        dComIfGp_setItemArrowNumCount(1);
        fopAcM_createItemForSimpleDemo(&current.pos, fpcNm_ITEM_ARROW_10, -1, NULL, NULL, 0.0f, 0.0f);
    }

    if (param_1 && field_0x2fcb == 0 && !checkMagicArmorNoDamage()) {
        field_0x2fcb = 120;
    }
}


/* 800DCA2C-800DCA80 0D736C 0054+00 1/1 0/0 2/2 .text
 * checkWoodShieldEquipNotIronBall__9daAlink_cCFv               */
BOOL daAlink_c::checkWoodShieldEquipNotIronBall() const {
    return checkWoodShieldEquip() && !checkIronBallEquip();
}

/* 800DCA80-800DCD74 0D73C0 02F4+00 0/0 0/0 1/1 .text
 * getArrowShieldOffset__9daAlink_cCFPC4cXyzPC5csXyzP4cXyzP4cXyz */
void daAlink_c::getArrowShieldOffset(cXyz const* param_0, csXyz const* param_1, cXyz* param_2,
                                     cXyz* param_3) const {
    cXyz sp54;
    mDoMtx_stack_c::ZXYrotS(param_1->x + cM_rndFX(0x1000), param_1->y + cM_rndFX(0x1000), 0);
    mDoMtx_stack_c::multVec(&cXyz::BaseZ, &sp54);
    mDoMtx_stack_c::copy(getShieldMtx());
    mDoMtx_stack_c::inverse();
    mDoMtx_stack_c::multVecSR(&sp54, param_3);

    static Vec const localCenter = {-15.0f, 0.0f, 18.0f};
    cXyz sp60;
    mDoMtx_multVec(getShieldMtx(), &localCenter, &sp60);

    cM3dGPla plane(&field_0x351c, -field_0x351c.inprod(sp60));

    cXyz sp6C;
    cXyz sp78 = *param_0 + sp54;
    plane.crossInfLin(*param_0, sp78, sp6C);

    if (sp6C.abs(sp60) > 20.0f) {
        sp6C = sp6C - sp60;
        sp6C.normalize();
        sp6C *= 20.0f;
        sp6C += sp60;
    }

    mDoMtx_stack_c::multVec(&sp6C, param_2);
}


/* 800DCD74-800DCDF0 0D76B4 007C+00 0/0 0/0 2/2 .text
 * setArrowShieldPos__9daAlink_cCFP4cXyzP5csXyzPC4cXyzPC4cXyz   */
void daAlink_c::setArrowShieldPos(cXyz* param_0, csXyz* param_1, cXyz const* param_2,
                                  cXyz const* param_3) const {
    cXyz sp1C;

    mDoMtx_multVecSR(getShieldMtx(), param_3, &sp1C);
    param_1->x = sp1C.atan2sY_XZ();
    param_1->y = sp1C.atan2sX_Z();

    mDoMtx_multVec(getShieldMtx(), param_2, param_0);
}

/* 800DCDF0-800DCE6C 0D7730 007C+00 17/17 0/0 0/0 .text checkUpperGuardAnime__9daAlink_cCFv */
BOOL daAlink_c::checkUpperGuardAnime() const {
    return checkNoResetFlg2(FLG2_UNK_8000000) && !checkEventRun() &&
           !checkNoResetFlg0(FLG0_UNK_2) && mProcID != PROC_GUARD_BREAK &&
           mProcID != PROC_CUT_REVERSE && mProcID != PROC_GUARD_ATTACK;
}

/* 800DCE6C-800DCF04 0D77AC 0098+00 1/0 0/0 0/0 .text            checkPlayerGuard__9daAlink_cCFv */
BOOL daAlink_c::checkPlayerGuard() const {
    return (checkSmallUpperGuardAnime() || (checkShieldGet() && checkHorseLieAnime())) ||
           checkUpperGuardAnime();
}

/* 800DCF04-800DCF64 0D7844 0060+00 5/5 0/0 0/0 .text checkPlayerGuardAndAttack__9daAlink_cCFv */
BOOL daAlink_c::checkPlayerGuardAndAttack() const {
    return checkPlayerGuard() || mProcID == PROC_GUARD_ATTACK;
}

/* 800DCF64-800DD008 0D78A4 00A4+00 2/2 0/0 0/0 .text            checkGuardAccept__9daAlink_cFv */
BOOL daAlink_c::checkGuardAccept() {
    return checkModeFlg(0x80) && checkShieldGet() && !checkFmChainGrabAnime() &&
           !checkNotBattleStage();
}

/* 800DD008-800DD018 0D7948 0010+00 11/11 0/0 0/0 .text            setUpperGuardAnime__9daAlink_cFf
 */
void daAlink_c::setUpperGuardAnime(f32) {
    onNoResetFlg2(FLG2_UNK_8000000);
}

/* 800DD018-800DD114 0D7958 00FC+00 1/1 0/0 0/0 .text            setShieldGuard__9daAlink_cFv */
void daAlink_c::setShieldGuard() {
    if ((mProcID == PROC_GUARD_SLIP && mEquipItem != fpcNm_ITEM_IRONBALL) || checkSmallUpperGuardAnime() ||
        (checkGuardAccept() && !checkGrabAnime() && !checkUpperReadyThrowAnime() &&
         !checkDkCaught2Anime() && !checkKandelaarSwingAnime() && !checkCutDashAnime() &&
         !checkCutDashChargeAnime() && (!checkEquipAnime() || checkUpperGuardAnime()) &&
         !checkRideOn() && checkGuardActionChange()))
    {
        onNoResetFlg2(FLG2_UNK_8000000);
    } else {
        offNoResetFlg2(FLG2_UNK_8000000);
    }
}

/* 800DD114-800DD1D4 0D7A54 00C0+00 1/1 0/0 0/0 .text setGuardSe__9daAlink_cFP12dCcD_GObjInf */
void daAlink_c::setGuardSe(dCcD_GObjInf* i_objinf) {
    int var_r31;
    int var_r30;

    if (checkWoodShieldEquipNotIronBall() && !checkMagicArmorNoDamage()) {
        var_r31 = 0x29;
        var_r30 = 0;
    } else {
        var_r31 = 0x28;
        var_r30 = 1;
    }

    u32 hit_se;
    if (i_objinf->GetTgHitGObj() != NULL && i_objinf->GetTgHitGObj()->GetAtType() == AT_TYPE_BOMB) {
        hit_se = Z2SE_HIT_METAL_WEAPON;
    } else {
        hit_se = i_objinf->GetTgHitObjHitSeID(var_r30);
    }

    mZ2Link.startCollisionSE(hit_se, var_r31);
}

/* 800DD1D4-800DD3E4 0D7B14 0210+00 1/1 0/0 0/0 .text setSmallGuard__9daAlink_cFP12dCcD_GObjInf */
void daAlink_c::setSmallGuard(dCcD_GObjInf* i_objinf) {
    setUpperAnimeBase(0x16);
    cXyz* dmg_vec = getDamageVec(i_objinf);

    mBodyAngle.y = (dmg_vec->atan2sX_Z() + 0x8000) - shape_angle.y;
    if (abs(mBodyAngle.y) > 0x7000) {
        mBodyAngle.y = 0;
    } else {
        mBodyAngle.y =
            cLib_minMaxLimit<s16>((s16)mBodyAngle.y, -daAlinkHIO_guard_c0::m.mSmallGuardLRAngleMax,
                                  daAlinkHIO_guard_c0::m.mSmallGuardLRAngleMax);
    }

    mBodyAngle.x = cLib_minMaxLimit<s16>(cM_atan2s(dmg_vec->y, dmg_vec->absXZ()), -daAlinkHIO_guard_c0::m.mSmallGuardFBAngleMax, daAlinkHIO_guard_c0::m.mSmallGuardFBAngleMax);
}

/* 800DD3E4-800DD894 0D7D24 04B0+00 1/1 0/0 0/0 .text
 * procGuardSlipInit__9daAlink_cFiP12dCcD_GObjInf               */
// NONMATCHING - extra useless mr
int daAlink_c::procGuardSlipInit(int param_0, dCcD_GObjInf* param_1) {
    if (mProcID == PROC_GUARD_SLIP) {
        return 0;
    }

    field_0x2fd5++;
    if (field_0x2fd5 == 4) {
        field_0x2fd5 = 0;
        return procGuardBreakInit();
    }

    commonProcInit(PROC_GUARD_SLIP);
    field_0x2fd4 = 3;
    
    cXyz* temp_r3 = getDamageVec(param_1);
    current.angle.y = temp_r3->atan2sX_Z();

    if (mEquipItem == fpcNm_ITEM_IRONBALL) {
        setSingleAnimeBase(ANM_IRONBALL_DEF);
        setIronBallWaitUpperAnime(1);
        mNowAnmPackUpper[2].setRatio(0.0f);

        mProcVar1.field_0x300a = 0;
        mProcVar2.field_0x300c = 0;
        mProcVar3.field_0x300e = 0;
    } else {
        const daAlinkHIO_guard_c1* var_r27 = &daAlinkHIO_guard_c0::m;
        daAlink_ANM var_r28;
        if (field_0x2f98 == 2) {
            var_r28 = ANM_GUARD_LEFT;
        } else {
            var_r28 = ANM_GUARD_RIGHT;
        }

        setSingleAnimeParam(var_r28, &var_r27->mGuardHitAnm);
        setUpperGuardAnime(-1.0f);

        mProcVar2.field_0x300c = (current.angle.y + 0x8000) - shape_angle.y;
        if (abs(mProcVar2.field_0x300c) > 0x7000) {
            mProcVar2.field_0x300c = 0;
        } else {
            mProcVar2.field_0x300c = cLib_minMaxLimit<s16>((s16)mProcVar2.field_0x300c, -daAlinkHIO_guard_c0::m.mGuardLRAngleMax, daAlinkHIO_guard_c0::m.mGuardLRAngleMax);
        }

        mProcVar3.field_0x300e = cLib_minMaxLimit<s16>(cM_atan2s(temp_r3->y, temp_r3->absXZ()), -daAlinkHIO_guard_c0::m.mGuardFBAngleMax, daAlinkHIO_guard_c0::m.mGuardFBAngleMax);
        mProcVar1.field_0x300a = daAlinkHIO_guard_c0::m.mGuardBodyInterpolation + 1;
        mProcVar2.field_0x300c = (mProcVar2.field_0x300c - mBodyAngle.y) / mProcVar1.field_0x300a;
        mProcVar3.field_0x300e = (mProcVar3.field_0x300e - mBodyAngle.x) / mProcVar1.field_0x300a;
        mProcVar1.field_0x300a--;

        mBodyAngle.y += mProcVar2.field_0x300c;
        mBodyAngle.x += mProcVar3.field_0x300e;
    }

    mProcVar4.field_0x3010 = 0;
    mProcVar5.field_0x3012 = 0;

    if (param_1 != NULL && param_1->GetTgHitAc() != NULL && fopAcM_GetName(param_1->GetTgHitAc()) == PROC_E_MM_MT && param_1->GetTgHitAc()->subtype == 1) {
        if (checkBootsOrArmorHeavy()) {
            mNormalSpeed = daAlinkHIO_guard_c0::m.mMagneHvyGuardSpeed;
            mProcVar5.field_0x3012 = 1;
        } else {
            mNormalSpeed = daAlinkHIO_guard_c0::m.mMagneGuardSpeed;
            mProcVar5.field_0x3012 = 0;
        }

        mProcVar4.field_0x3010 = 1;
    } else if (checkHugeAttack(param_0)) {
        if ((!checkBootsOrArmorHeavy() && checkMiddleBossGoronRoom()) || checkStageName("D_MN07B")) {
            mNormalSpeed = 40.0f;
        } else {
            mNormalSpeed = daAlinkHIO_guard_c0::m.mGuardSpeedHuge;
        }
    } else if (checkLargeAttack(param_0)) {
        mNormalSpeed = daAlinkHIO_guard_c0::m.mGuardSpeedLarge;
    } else {
        mNormalSpeed = daAlinkHIO_guard_c0::m.mGuardSpeedNormal;
    }

    field_0x2f9d = 0x60;
    setFootEffectProcType(1);
    return 1;
}

/* 800DD894-800DDA10 0D81D4 017C+00 1/0 0/0 0/0 .text            procGuardSlip__9daAlink_cFv */
int daAlink_c::procGuardSlip() {
    if (mEquipItem != fpcNm_ITEM_IRONBALL) {
        setUpperGuardAnime(-1.0f);
    }

    setShapeAngleToAtnActor(0);

    if (mProcVar1.field_0x300a != 0) {
        mProcVar1.field_0x300a -= 1;
        mBodyAngle.y += mProcVar2.field_0x300c;
        mBodyAngle.x += mProcVar3.field_0x300e;
    }

    if (mNormalSpeed > 2.5f) {
        field_0x2f9d = 0x60;
    }

    f32 var_f31;
    if (checkBootsOrArmorHeavy() && mProcVar4.field_0x3010 == 0) {
        var_f31 = 2.5f;
    } else {
        var_f31 = 1.25f;
    }

    if (mProcVar4.field_0x3010 != 0 && mProcVar5.field_0x3012 != checkBootsOrArmorHeavy()) {
        mNormalSpeed = 0.0f;
    }

    if (cLib_chaseF(&mNormalSpeed, 0.0f, var_f31) != 0 && checkAnmEnd(&mUnderFrameCtrl[0])) {
        field_0x814.SetTgApid(0);

        if (!checkGuardActionChange()) {
            onEndResetFlg1(ERFLG1_UNK_20);
        }

        if (mEquipItem == fpcNm_ITEM_IRONBALL) {
            mNowAnmPackUpper[2].setRatio(1.0f);
        }

        checkNextAction(0);
    }

    return 1;
}

/* 800DDA10-800DDBA8 0D8350 0198+00 2/1 0/0 0/0 .text            procGuardAttackInit__9daAlink_cFv
 */
int daAlink_c::procGuardAttackInit() {
    if (mDemo.getDemoMode() == 0x4B && mProcID == PROC_GUARD_ATTACK) {
        return 1;
    }

    commonProcInit(PROC_GUARD_ATTACK);
    const daAlinkHIO_guardAttack_c1* temp_r30 = &daAlinkHIO_gAtPush_c0::m;
    
    field_0x2f98 = 3;
    field_0x34d4 = l_rWaitBaseAnime;
    field_0x3588 = l_rWaitBaseAnime;

    setCutType(0x29);
    setSingleAnimeParam(ANM_SHIELD_ATTACK, &temp_r30->mAttackAnm);

    field_0x3478 = temp_r30->mAttackStartFrame;
    field_0x347c = temp_r30->mAttackEndFrame;
    field_0x3480 = temp_r30->mSpeed;

    if (checkHeavyStateOn(1, 1) != 0) {
        field_0x3480 *= mHeavySpeedMultiplier;
    }

    if (mDemo.getDemoMode() == 0x4B) {
        field_0x3484 = 1000.0f;
        field_0x3488 = 1000.0f;
    } else {
        field_0x3484 = temp_r30->mAttackAnm.mCancelFrame;
        field_0x3488 = temp_r30->mSlashCheckFrame;
    }

    mNormalSpeed = 0.0f;
    setSwordVoiceSe(Z2SE_AL_V_TATE_OSHI);

    mGuardAtCps.ResetAtHit();
    mGuardAtCps.SetAtSe(2);
    mGuardAtCps.SetR(daAlinkHIO_guard_c0::m.mAttackRadius);

    current.angle.y = shape_angle.y;

    setUpperGuardAnime(-1.0f);
    mProcVar2.field_0x300c = 0;
    mProcVar3.field_0x300e = 0;
    mProcVar5.field_0x3012 = 0;
    mProcVar0.field_0x3008 = 0;
    return 1;
}

/* 800DDBA8-800DDE58 0D84E8 02B0+00 1/0 0/0 0/0 .text            procGuardAttack__9daAlink_cFv */
int daAlink_c::procGuardAttack() {
    daPy_frameCtrl_c* temp_r30 = &mUnderFrameCtrl[0];

    setUpperGuardAnime(-1.0f);
    field_0x2f99 = 4;
    cLib_chaseF(&mNormalSpeed, 0.0f, daAlinkHIO_move_c0::m.mDeceleration);
    setShapeAngleToAtnActor(1);
    current.angle.y = shape_angle.y;

    if (checkForceSwordSwing()) {
        mProcVar2.field_0x300c = 1;
    }

    BOOL var_r29;
    if (mEquipItem == 0x103 && !checkMagneBootsOn()) {
        var_r29 = 1;
        if (doTrigger()) {
            mProcVar3.field_0x300e = 1;
        }
    } else {
        var_r29 = 0;
    }

    if (mProcVar0.field_0x3008 == 0 && mGuardAtCps.ChkAtHit()) {
        mProcVar0.field_0x3008 = 1;
        dComIfGp_getVibration().StartShock(3, 1, cXyz(0.0f, 1.0f, 0.0f));
    }

    if (mProcVar5.field_0x3012 != 0 && commonLineCheck(&field_0x3834, &mRightHandPos) && dComIfG_Bgsp().GetWallCode(mLinkLinChk) != 7) {
        mZ2Link.startHitItemSE(Z2SE_HIT_SHIELD_ATTACK, dKy_pol_sound_get(&mLinkLinChk), NULL, -1.0f);
        return procCutReverseInit(ANM_SHIELD_ATTACK_RECOIL);
    }

    if (var_r29) {
        if (checkCutHeadState()) {
            setDoStatusEmphasys(0x77);
        } else {
            setDoStatus(0x86);
        }
    }

    if (checkAnmEnd(temp_r30)) {
        if (mDemo.getDemoMode() == 0x4B) {
            dComIfGp_evmng_cutEnd(field_0x3184);
        } else {
            checkNextAction(0);
        }
    } else {
        if (temp_r30->getFrame() > field_0x3488) {
            if (mProcVar3.field_0x300e != 0) {
                return checkDoCutAction();
            }

            if (mProcVar2.field_0x300c != 0) {
                return checkCutAction();
            }
        }

        if (temp_r30->getFrame() > field_0x3484) {
            onModeFlg(4);
            checkNextAction(1);
        } else if (temp_r30->getFrame() >= field_0x3478 && temp_r30->getFrame() < field_0x347c) {
            if (mProcVar5.field_0x3012 == 0) {
                mNormalSpeed = field_0x3480;
            }
            mProcVar5.field_0x3012 = 1;
        } else {
            mProcVar5.field_0x3012 = 0;
        }
    }

    return 1;
}

/* 800DDE58-800DDF2C 0D8798 00D4+00 2/2 0/0 0/0 .text            procGuardBreakInit__9daAlink_cFv */
int daAlink_c::procGuardBreakInit() {
    commonProcInit(PROC_GUARD_BREAK);
    setSingleAnimeParam(ANM_GUARD_BREAK, &daAlinkHIO_guard_c0::m.mGuardBreakAnm);

    if (mEquipItem == fpcNm_ITEM_IRONBALL) {
        deleteEquipItem(0, 0);
    }

    mNormalSpeed = 0.0f;
    field_0x3588 = l_halfAtnWaitBaseAnime;
    dComIfGp_getVibration().StartShock(5, 0x1F, cXyz(0.0f, 1.0f, 0.0f));
    voiceStart(Z2SE_AL_V_GUARD_BROKEN);
    return 1;
}

/* 800DDF2C-800DE008 0D886C 00DC+00 1/0 0/0 0/0 .text            procGuardBreak__9daAlink_cFv */
int daAlink_c::procGuardBreak() {
    daPy_frameCtrl_c* temp_r30 = &mUnderFrameCtrl[0];

    setUpperGuardAnime(-1.0f);
    onEndResetFlg0(ERFLG0_UNK_8000000);
    field_0x2f99 = 4;

    if (checkAnmEnd(temp_r30)) {
        if (checkGuardActionChange() == 0) {
            onEndResetFlg1(ERFLG1_UNK_20);
        }
        checkNextAction(0);
    } else if (temp_r30->getFrame() > daAlinkHIO_guard_c0::m.mGuardBreakAnm.mCancelFrame) {
        if (!checkGuardActionChange()) {
            onEndResetFlg1(ERFLG1_UNK_20);
        }

        onModeFlg(4);
        checkNextAction(1);
    }

    return 1;
}

/* 800DE008-800DE380 0D8948 0378+00 2/2 0/0 0/0 .text            procTurnMoveInit__9daAlink_cFi */
int daAlink_c::procTurnMoveInit(int param_0) {
    BOOL temp_r27 = checkUpperGuardAnime();
    fopAc_ac_c* temp_r28 = mTargetedActor;

    commonProcInit(PROC_TURN_MOVE);
    mNormalSpeed = 0.0f;

    daAlink_ANM var_r29;
    if (param_0 == 2) {
        var_r29 = ANM_SIDE_ROLL_LEFT;
    } else if (param_0 == 3) {
        var_r29 = ANM_SIDE_ROLL_RIGHT;
    } else if (checkInputOnR()) {
        if ((s16)(field_0x2fe2 - shape_angle.y) >= 0) {
            var_r29 = ANM_SIDE_ROLL_LEFT;
        } else {
            var_r29 = ANM_SIDE_ROLL_RIGHT;
        }
    } else {
        var_r29 = ANM_SIDE_ROLL_LEFT;
    }

    f32 temp_f31 = daAlinkHIO_turnMove_c0::m.mTurnAnm.mSpeed;
    mProcVar1.field_0x300a = daAlinkHIO_turnMove_c0::m.mMaxTurnSpeed;

    if (checkNoResetFlg0(FLG0_UNDERWATER)) {
        mProcVar1.field_0x300a = mProcVar1.field_0x300a * daAlinkHIO_magneBoots_c0::m.mWaterVelocityX;
    }

    setSingleAnime(var_r29, temp_f31, daAlinkHIO_turnMove_c0::m.mTurnAnm.mStartFrame, daAlinkHIO_turnMove_c0::m.mTurnAnm.mEndFrame, daAlinkHIO_turnMove_c0::m.mTurnAnm.mInterpolation);

    if (var_r29 == ANM_SIDE_ROLL_LEFT) {
        mProcVar2.field_0x300c = -0x7FF0;
        field_0x2f98 = 2;
    } else {
        mProcVar2.field_0x300c = 0x7FF0;
        field_0x2f98 = 3;
    }

    if (temp_r28 != NULL) {
        cXyz sp8 = current.pos - temp_r28->current.pos;
        shape_angle.y = fopAcM_searchActorAngleY(this, temp_r28);
        field_0x3478 = sp8.absXZ();
        mProcVar3.field_0x300e = sp8.atan2sX_Z();
    } else {
        field_0x3478 = 200.0f;
        mProcVar3.field_0x300e = shape_angle.y + 0x8000;
    }

    field_0x347c = (field_0x3478 - 150.0f) / mProcVar2.field_0x300c;
    voiceStart(Z2SE_AL_V_SOTOMO_ROLL);

    if (mDemo.getDemoMode() == 0x52) {
        mProcVar0.field_0x3008 = 1;
    } else {
        mProcVar0.field_0x3008 = 0;
    }

    setFootEffectProcType(0);
    field_0x2f9d = 4;
    dComIfGp_setPlayerStatus0(0, 4);

    if (temp_r27 != 0) {
        onNoResetFlg0(FLG0_UNK_2);
    }

    return 1;
}

/* 800DE380-800DE64C 0D8CC0 02CC+00 1/0 0/0 0/0 .text            procTurnMove__9daAlink_cFv */
int daAlink_c::procTurnMove() {
    onEndResetFlg0(ERFLG0_UNK_8000000);
    s16 temp_r28 = mProcVar2.field_0x300c;

    cLib_addCalcAngleS(&mProcVar2.field_0x300c, 0, daAlinkHIO_turnMove_c0::m.mTurnSpeedRate, mProcVar1.field_0x300a, daAlinkHIO_turnMove_c0::m.mMinTurnSpeed);

    s16 temp_r29 = temp_r28 - mProcVar2.field_0x300c;
    if (temp_r28 > 0) {
        current.angle.y = mProcVar3.field_0x300e + 0x4000;
    } else {
        current.angle.y = mProcVar3.field_0x300e - 0x4000;
    }

    f32 temp_f31 = current.pos.x - (field_0x3478 * cM_ssin(mProcVar3.field_0x300e));
    f32 temp_f30 = current.pos.z - (field_0x3478 * cM_scos(mProcVar3.field_0x300e));

    mProcVar3.field_0x300e += temp_r29;
    field_0x3478 -= field_0x347c * temp_r29;

    current.pos.x = temp_f31 + (field_0x3478 * cM_ssin(mProcVar3.field_0x300e));
    current.pos.z = temp_f30 + (field_0x3478 * cM_scos(mProcVar3.field_0x300e));

    shape_angle.y = mProcVar3.field_0x300e + 0x8000;
    mProcVar0.field_0x3008 |= checkCutFinishJumpUp();

    daPy_frameCtrl_c* temp_r30 = &mUnderFrameCtrl[0];

    if (checkNoResetFlg0(FLG0_UNK_2)) {
        setUpperGuardAnime(-1.0f);
    }

    if (checkAnmEnd(temp_r30)) {
        if (!checkAttentionLock()) {
            offNoResetFlg2(FLG2_UNK_8000000);
        }
        checkNextAction(0);
    } else if (mProcVar0.field_0x3008 != 0 && !checkNotJumpSinkLimit() && temp_r30->getFrame() > daAlinkHIO_turnMove_c0::m.mTwirlCutDelayF) {
        procCutFinishJumpUpInit();
        dComIfGp_setPlayerStatus0(0, 4);
    } else {
        if (temp_r30->getFrame() > daAlinkHIO_turnMove_c0::m.mTurnAnm.mCancelFrame) {
            onModeFlg(4);

            if (checkNextAction(1)) {
                return 1;
            }
        }

        if (temp_r30->checkPass(19.0f)) {
            dComIfGp_getVibration().StartShock(1, 1, cXyz(0.0f, 1.0f, 0.0f));
        }

        if (abs(temp_r29) > 0x100) {
            field_0x2f9d = 4;
        }
    }

    return 1;
}
