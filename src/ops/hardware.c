#include "ops/hardware.h"

#include "helpers.h"
#include "ii.h"
#include "teletype.h"
#include "teletype_io.h"

static void op_CV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_CV_set(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_CV_SLEW_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_CV_SLEW_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_CV_OFF_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CV_OFF_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_TR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_TR_set(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_TR_POL_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_TR_POL_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_TR_TIME_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_TR_TIME_set(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_TR_TOG_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_TR_PULSE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_II_get(const void *data, scene_state_t *ss, exec_state_t *es,
                      command_state_t *cs);
static void op_CV_SET_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_MUTE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                        command_state_t *cs);
static void op_UNMUTE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_STATE_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

static void op_JF_TR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);
static void op_JF_RMODE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_JF_RUN_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_SHIFT_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_JF_VTR_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_MODE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_TICK_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_VOX_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_NOTE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_GOD_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_JF_TUNE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_JF_QT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs);

static void op_KR_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_KR_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_KR_PATTERN_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_KR_PATTERN_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_KR_SCALE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_KR_SCALE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_KR_PERIOD_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_KR_PERIOD_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_KR_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_KR_LOOP_ST_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_KR_LOOP_ST_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_KR_LOOP_LEN_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_KR_LOOP_LEN_set(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_KR_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);

static void op_MP_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_MP_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_MP_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_MP_STOP_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs);
static void op_MP_SCALE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_MP_SCALE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_MP_PERIOD_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_MP_PERIOD_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);

static void op_LV_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_LV_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_LV_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_LV_PATTERN_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_LV_PATTERN_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_LV_LOOP_ST_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_LV_LOOP_ST_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs);
static void op_LV_LOOP_LEN_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_LV_LOOP_LEN_set(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs);
static void op_LV_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_LV_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);

static void op_CY_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_CY_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs);
static void op_CY_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_CY_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_REV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs);
static void op_CY_BRAKE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_CY_BRAKE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_CY_ACCEL_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_CY_ACCEL_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_CY_RANGE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);
static void op_CY_RANGE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs);


// clang-format off
const tele_op_t op_CV       = MAKE_GET_SET_OP(CV     , op_CV_get     , op_CV_set     , 1, true);
const tele_op_t op_CV_OFF   = MAKE_GET_SET_OP(CV.OFF , op_CV_OFF_get , op_CV_OFF_set , 1, true);
const tele_op_t op_CV_SLEW  = MAKE_GET_SET_OP(CV.SLEW, op_CV_SLEW_get, op_CV_SLEW_set, 1, true);
const tele_op_t op_TR       = MAKE_GET_SET_OP(TR     , op_TR_get     , op_TR_set     , 1, true);
const tele_op_t op_TR_POL   = MAKE_GET_SET_OP(TR.POL , op_TR_POL_get , op_TR_POL_set , 1, true);
const tele_op_t op_TR_TIME  = MAKE_GET_SET_OP(TR.TIME, op_TR_TIME_get, op_TR_TIME_set, 1, true);
const tele_op_t op_TR_TOG   = MAKE_GET_OP(TR.TOG  , op_TR_TOG_get  , 1, false);
const tele_op_t op_TR_PULSE = MAKE_GET_OP(TR.PULSE, op_TR_PULSE_get, 1, false);
const tele_op_t op_II       = MAKE_GET_OP(II      , op_II_get      , 2, false);
const tele_op_t op_CV_SET   = MAKE_GET_OP(CV.SET  , op_CV_SET_get  , 2, false);
const tele_op_t op_MUTE     = MAKE_GET_OP(MUTE    , op_MUTE_get    , 1, false);
const tele_op_t op_UNMUTE   = MAKE_GET_OP(UNMUTE  , op_UNMUTE_get  , 1, false);
const tele_op_t op_STATE    = MAKE_GET_OP(STATE   , op_STATE_get   , 1, true );

const tele_op_t op_JF_TR    = MAKE_GET_OP(JF.TR    , op_JF_TR_get    , 2, false);
const tele_op_t op_JF_RMODE = MAKE_GET_OP(JF.RMODE , op_JF_RMODE_get , 1, false);
const tele_op_t op_JF_RUN   = MAKE_GET_OP(JF.RUN   , op_JF_RUN_get   , 1, false);
const tele_op_t op_JF_SHIFT = MAKE_GET_OP(JF.SHIFT , op_JF_SHIFT_get , 1, false);
const tele_op_t op_JF_VTR   = MAKE_GET_OP(JF.VTR   , op_JF_VTR_get   , 2, false);
const tele_op_t op_JF_MODE  = MAKE_GET_OP(JF.MODE  , op_JF_MODE_get  , 1, false);
const tele_op_t op_JF_TICK  = MAKE_GET_OP(JF.TICK  , op_JF_TICK_get  , 1, false);
const tele_op_t op_JF_VOX   = MAKE_GET_OP(JF.VOX   , op_JF_VOX_get   , 3, false);
const tele_op_t op_JF_NOTE  = MAKE_GET_OP(JF.NOTE  , op_JF_NOTE_get  , 2, false);
const tele_op_t op_JF_GOD   = MAKE_GET_OP(JF.GOD   , op_JF_GOD_get   , 1, false);
const tele_op_t op_JF_TUNE  = MAKE_GET_OP(JF.TUNE  , op_JF_TUNE_get  , 3, false);
const tele_op_t op_JF_QT    = MAKE_GET_OP(JF.QT    , op_JF_QT_get    , 1, false);

const tele_op_t op_KR_PRESET   = MAKE_GET_SET_OP(KR.PRE     , op_KR_PRESET_get   , op_KR_PRESET_set   , 0, true);
const tele_op_t op_KR_PATTERN  = MAKE_GET_SET_OP(KR.PAT     , op_KR_PATTERN_get  , op_KR_PATTERN_set  , 0, true);
const tele_op_t op_KR_SCALE    = MAKE_GET_SET_OP(KR.SCALE   , op_KR_SCALE_get    , op_KR_SCALE_set    , 0, true);
const tele_op_t op_KR_PERIOD   = MAKE_GET_SET_OP(KR.PERIOD  , op_KR_PERIOD_get   , op_KR_PERIOD_set   , 0, true);
const tele_op_t op_KR_POS      = MAKE_GET_SET_OP(KR.POS     , op_KR_POS_get      , op_KR_POS_set      , 2, true);
const tele_op_t op_KR_LOOP_ST  = MAKE_GET_SET_OP(KR.L.ST    , op_KR_LOOP_ST_get  , op_KR_LOOP_ST_set  , 2, true);
const tele_op_t op_KR_LOOP_LEN = MAKE_GET_SET_OP(KR.L.LEN   , op_KR_LOOP_LEN_get , op_KR_LOOP_LEN_set , 2, true);
const tele_op_t op_KR_RESET    = MAKE_GET_OP(KR.RES         , op_KR_RESET_get    , 2, false);

const tele_op_t op_MP_PRESET1  = MAKE_GET_SET_OP(MP.PRE     , op_MP_PRESET_get   , op_MP_PRESET_set   , 0, true);
const tele_op_t op_MP_RESET1   = MAKE_GET_OP(MP.RES         , op_MP_RESET_get    , 1, false);
const tele_op_t op_MP_STOP1    = MAKE_GET_OP(MP.OFF         , op_MP_STOP_get     , 1, false);
const tele_op_t op_MP_SCALE    = MAKE_GET_SET_OP(MP.SCALE   , op_MP_SCALE_get    , op_MP_SCALE_set    , 0, true);
const tele_op_t op_MP_PERIOD   = MAKE_GET_SET_OP(MP.PERIOD  , op_MP_PERIOD_get   , op_MP_PERIOD_set   , 0, true);

const tele_op_t op_LV_PRESET   = MAKE_GET_SET_OP(LV.PRE     , op_LV_PRESET_get   , op_LV_PRESET_set   , 0, true);
const tele_op_t op_LV_RESET    = MAKE_GET_OP(LV.RES         , op_LV_RESET_get    , 1, false);
const tele_op_t op_LV_PATTERN  = MAKE_GET_SET_OP(LV.PAT     , op_LV_PATTERN_get  , op_LV_PATTERN_set  , 0, true);
const tele_op_t op_LV_LOOP_ST  = MAKE_GET_SET_OP(LV.L.ST    , op_LV_LOOP_ST_get  , op_LV_LOOP_ST_set  , 0, true);
const tele_op_t op_LV_LOOP_LEN = MAKE_GET_SET_OP(LV.L.LEN   , op_LV_LOOP_LEN_get , op_LV_LOOP_LEN_set , 0, true);
const tele_op_t op_LV_POS      = MAKE_GET_SET_OP(LV.POS     , op_LV_POS_get      , op_LV_POS_set      , 0, true);

const tele_op_t op_CY_PRESET   = MAKE_GET_SET_OP(CY.PRE     , op_CY_PRESET_get   , op_CY_PRESET_set   , 0, true);
const tele_op_t op_CY_RESET    = MAKE_GET_OP(CY.RES         , op_CY_RESET_get    , 1, false);
const tele_op_t op_CY_POS      = MAKE_GET_SET_OP(CY.POS     , op_CY_POS_get      , op_CY_POS_set      , 1, true);
const tele_op_t op_CY_REV      = MAKE_GET_OP(CY.REV         , op_CY_REV_get      , 1, false);
const tele_op_t op_CY_BRAKE    = MAKE_GET_SET_OP(CY.BRAKE   , op_CY_BRAKE_get    , op_CY_BRAKE_set    , 1, true);
const tele_op_t op_CY_ACCEL    = MAKE_GET_SET_OP(CY.ACCEL   , op_CY_ACCEL_get    , op_CY_ACCEL_set    , 1, true);
const tele_op_t op_CY_RANGE    = MAKE_GET_SET_OP(CY.RANGE   , op_CY_RANGE_get    , op_CY_RANGE_set    , 1, true);


// clang-format on

static void op_CV_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    // cs_push(cs, ss->variables.cv[a]);

    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.cv[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx_now(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_CV_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    // a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    b = normalise_value(0, 16383, 0, b);
    // ss->variables.cv[a] = b;
    // tele_cv(a, b, 1);

    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv[a] = b;
        tele_cv(a, b, 1);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);

        tele_ii_tx(addr, d, 4);
    }
}

static void op_CV_SLEW_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    // cs_push(cs, ss->variables.cv_slew[a]);

    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.cv_slew[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_SLEW | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx_now(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_CV_SLEW_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    // a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    b = normalise_value(1, 32767, 0, b);  // min slew = 1
    // ss->variables.cv_slew[a] = b;
    // tele_cv_slew(a, b);

    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv_slew[a] = b;
        tele_cv_slew(a, b);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_SLEW, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_CV_OFF_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    // cs_push(cs, ss->variables.cv_off[a]);

    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.cv_off[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_OFF | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx_now(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_CV_OFF_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    // a = normalise_value(0, CV_COUNT - 1, 0, a - 1);
    ss->variables.cv_off[a] = b;
    // tele_cv_off(a, b);
    // tele_cv(a, ss->variables.cv[a], 1);

    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv_off[a] = b;
        tele_cv_off(a, b);
        tele_cv(a, ss->variables.cv[a], 1);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_OFF, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_TR_get(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.tr[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx_now(addr, d, 2);
        d[0] = 0;
        tele_ii_rx(addr, d, 1);
        cs_push(cs, d[0]);
    }
    else
        cs_push(cs, 0);
}

static void op_TR_set(const void *NOTUSED(data), scene_state_t *ss,
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    // a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.tr[a] = b != 0;
        tele_tr(a, b);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR, a & 0x3, b };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 3);
    }
}

static void op_TR_POL_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    // cs_push(cs, ss->variables.tr_pol[a]);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.tr_pol[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_POL | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx_now(addr, d, 2);
        d[0] = 0;
        tele_ii_rx(addr, d, 1);
        cs_push(cs, d[0]);
    }
    else
        cs_push(cs, 0);
}

static void op_TR_POL_set(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    // a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    // ss->variables.tr_pol[a] = b > 0;
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.tr_pol[a] = b > 0;
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_POL, a & 0x3, b > 0 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 3);
    }
}

static void op_TR_TIME_get(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    // cs_push(cs, ss->variables.tr_time[a]);
    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 4)
        cs_push(cs, ss->variables.tr_time[a]);
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_TIME | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx_now(addr, d, 2);
        d[0] = 0;
        d[1] = 0;
        tele_ii_rx(addr, d, 2);
        cs_push(cs, (d[0] << 8) + d[1]);
    }
    else
        cs_push(cs, 0);
}

static void op_TR_TIME_set(const void *NOTUSED(data), scene_state_t *ss,
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    // a = normalise_value(0, TR_COUNT - 1, 0, a - 1);
    if (b < 0) b = 0;
    // ss->variables.tr_time[a] = b;
    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.tr_time[a] = b;
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_TIME, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_TR_TOG_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // saturate and shift
    // a--;
    // if (ss->variables.tr[a])
    //     ss->variables.tr[a] = 0;
    // else
    //     ss->variables.tr[a] = 1;
    // tele_tr(a, ss->variables.tr[a]);

    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        if (ss->variables.tr[a])
            ss->variables.tr[a] = 0;
        else
            ss->variables.tr[a] = 1;
        tele_tr(a, ss->variables.tr[a]);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_TOG, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
    }
}

static void op_TR_PULSE_get(const void *NOTUSED(data), scene_state_t *ss,
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // saturate and shift
    // if (a < 1)
    //     a = 1;
    // else if (a > 4)
    //     a = 4;
    // a--;
    // int16_t time = ss->variables.tr_time[a];  // pulse time
    // if (time <= 0) return;                    // if time <= 0 don't do
    // anything
    // ss->variables.tr[a] = ss->variables.tr_pol[a];
    // ss->tr_pulse_timer[a] = time;  // set time
    // tele_tr(a, ss->variables.tr[a]);

    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        int16_t time = ss->variables.tr_time[a];  // pulse time
        if (time <= 0) return;  // if time <= 0 don't do anything
        ss->variables.tr[a] = ss->variables.tr_pol[a];
        ss->tr_pulse_timer[a] = time;  // set time
        tele_tr(a, ss->variables.tr[a]);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_TR_PULSE, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 2);
    }
}

static void op_II_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                      exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    tele_ii(a, b);
}

static void op_CV_SET_get(const void *NOTUSED(data), scene_state_t *ss,
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    // saturate and shift
    // if (a < 1)
    //     a = 1;
    // else if (a > 4)
    //     a = 4;
    // a--;
    if (b < 0)
        b = 0;
    else if (b > 16383)
        b = 16383;
    // ss->variables.cv[a] = b;
    // tele_cv(a, b, 0);

    a--;
    if (a < 0)
        return;
    else if (a < 4) {
        ss->variables.cv[a] = b;
        tele_cv(a, b, 0);
    }
    else if (a < 20) {
        uint8_t d[] = { II_ANSIBLE_CV_SET, a & 0x3, b >> 8, b & 0xff };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 4) >> 2) << 1);
        tele_ii_tx(addr, d, 4);
    }
}

static void op_MUTE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                        exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a;
    a = cs_pop(cs);
    if (a > 0 && a < 9) { tele_mute(a - 1, 0); }
}

static void op_UNMUTE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a;
    a = cs_pop(cs);
    if (a > 0 && a < 9) { tele_mute(a - 1, 1); }
}

static void op_STATE_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    // a--;
    // if (a < 0)
    //     a = 0;
    // else if (a > 7)
    //     a = 7;

    // cs_push(cs, tele_get_input_state(a));

    a--;
    if (a < 0)
        cs_push(cs, 0);
    else if (a < 8)
        cs_push(cs, tele_get_input_state(a));
    else if (a < 24) {
        uint8_t d[] = { II_ANSIBLE_INPUT | II_GET, a & 0x3 };
        uint8_t addr = II_ANSIBLE_ADDR + (((a - 8) >> 2) << 1);
        tele_ii_tx_now(addr, d, 2);
        d[0] = 0;
        tele_ii_rx(addr, d, 1);
        cs_push(cs, d[0]);
    }
    else
        cs_push(cs, 0);
}


static void op_JF_TR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                         exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { JF_TR, a, b };
    tele_ii_tx(JF_ADDR, d, 3);
}

static void op_JF_RMODE_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_RMODE, a };
    tele_ii_tx(JF_ADDR, d, 2);
}

static void op_JF_RUN_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_RUN, a >> 8, a & 0xff };
    tele_ii_tx(JF_ADDR, d, 3);
}

static void op_JF_SHIFT_get(const void *NOTUSED(data),
                            scene_state_t *NOTUSED(ss),
                            exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_SHIFT, a >> 8, a & 0xff };
    tele_ii_tx(JF_ADDR, d, 3);
}

static void op_JF_VTR_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { JF_VTR, a, b >> 8, b & 0xff };
    tele_ii_tx(JF_ADDR, d, 4);
}

static void op_JF_MODE_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_MODE, a };
    tele_ii_tx(JF_ADDR, d, 2);
}

static void op_JF_TICK_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_TICK, a };
    tele_ii_tx(JF_ADDR, d, 2);
}

static void op_JF_VOX_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { JF_VOX, a, b >> 8, b & 0xff, c >> 8, c & 0xff };
    tele_ii_tx(JF_ADDR, d, 6);
}

static void op_JF_NOTE_get(const void *NOTUSED(data),
                           scene_state_t *NOTUSED(ss),
                           exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { JF_NOTE, a >> 8, a & 0xff, b >> 8, b & 0xff };
    tele_ii_tx(JF_ADDR, d, 5);
}

static void op_JF_GOD_get(const void *NOTUSED(data), scene_state_t *NOTUSED(ss),
                          exec_state_t *NOTUSED(es), command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_GOD, a };
    tele_ii_tx(JF_ADDR, d, 2);
}

static void op_JF_TUNE_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { JF_TUNE, a, b, c };
    tele_ii_tx(JF_ADDR, d, 4);
}

static void op_JF_QT_get(const void *data, scene_state_t *ss, exec_state_t *es,
                         command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { JF_QT, a };
    tele_ii_tx(JF_ADDR, d, 2);
}


static void op_KR_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_PRESET, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_KR_PRESET | II_GET };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_PATTERN_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_PATTERN, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_PATTERN_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_KR_PATTERN | II_GET };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_SCALE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_SCALE, a };
    tele_ii_tx(II_KR_ADDR, d, 2);
}

static void op_KR_SCALE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_KR_SCALE | II_GET };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_PERIOD_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_KR_PERIOD, a >> 8, a & 0xff };
    tele_ii_tx(II_KR_ADDR, d, 3);
}

static void op_KR_PERIOD_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_KR_PERIOD | II_GET, 0 };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_KR_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { II_KR_POS, a, b, c };
    tele_ii_tx(II_KR_ADDR, d, 4);
}

static void op_KR_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_POS | II_GET, a, b };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx_now(addr, d, 3);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_LOOP_ST_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_ST, a, b, c };
    tele_ii_tx(II_KR_ADDR, d, 4);
}

static void op_KR_LOOP_ST_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_ST | II_GET, a, b };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx_now(addr, d, 3);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_LOOP_LEN_set(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    int16_t c = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_LEN, a, b, c };
    tele_ii_tx(II_KR_ADDR, d, 4);
}

static void op_KR_LOOP_LEN_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_LOOP_LEN | II_GET, a, b };
    uint8_t addr = II_KR_ADDR;
    tele_ii_tx_now(addr, d, 3);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_KR_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_KR_RESET, a, b };
    tele_ii_tx(II_KR_ADDR, d, 3);
}

static void op_MP_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_PRESET, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_MP_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_MP_PRESET | II_GET };
    uint8_t addr = II_MP_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_MP_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_RESET, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_MP_STOP_get(const void *data, scene_state_t *ss,
                           exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_STOP, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_MP_SCALE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_SCALE, a };
    tele_ii_tx(II_MP_ADDR, d, 2);
}

static void op_MP_SCALE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_MP_SCALE | II_GET };
    uint8_t addr = II_MP_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_MP_PERIOD_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_MP_PERIOD, a >> 8, a & 0xff };
    tele_ii_tx(II_MP_ADDR, d, 3);
}

static void op_MP_PERIOD_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_MP_PERIOD | II_GET, 0 };
    uint8_t addr = II_MP_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}

static void op_LV_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_PRESET, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_LV_PRESET | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_RESET, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_PATTERN_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_PATTERN, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_PATTERN_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_LV_PATTERN | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_LOOP_ST_set(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_LOOP_ST, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_LOOP_ST_get(const void *data, scene_state_t *ss,
                              exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_LV_LOOP_ST | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_LOOP_LEN_set(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_LOOP_LEN, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_LOOP_LEN_get(const void *data, scene_state_t *ss,
                               exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_LV_LOOP_LEN | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_LV_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_LV_POS, a };
    tele_ii_tx(II_LV_ADDR, d, 2);
}

static void op_LV_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    uint8_t d[] = { II_LV_POS | II_GET };
    uint8_t addr = II_LV_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_CY_PRESET_set(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_PRESET, a };
    tele_ii_tx(II_CY_ADDR, d, 2);
}

static void op_CY_PRESET_get(const void *data, scene_state_t *ss,
                             exec_state_t *es, command_state_t *cs) {
    uint8_t d[] = { II_CY_PRESET | II_GET };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx_now(addr, d, 1);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_CY_RESET_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_RESET, a };
    tele_ii_tx(II_CY_ADDR, d, 2);
}

static void op_CY_POS_set(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_CY_POS, a, b };
    tele_ii_tx(II_CY_ADDR, d, 3);
}

static void op_CY_POS_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_POS | II_GET, a };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx_now(addr, d, 2);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_CY_REV_get(const void *data, scene_state_t *ss, exec_state_t *es,
                          command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_REV, a };
    tele_ii_tx(II_CY_ADDR, d, 2);
}

static void op_CY_BRAKE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_CY_BRAKE, a, b };
    tele_ii_tx(II_CY_ADDR, d, 3);
}

static void op_CY_BRAKE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_BRAKE | II_GET, a };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx_now(addr, d, 2);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_CY_ACCEL_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_CY_ACCEL, a, b };
    tele_ii_tx(II_CY_ADDR, d, 3);
}

static void op_CY_ACCEL_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_ACCEL | II_GET, a };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx_now(addr, d, 2);
    d[0] = 0;
    tele_ii_rx(addr, d, 1);
    cs_push(cs, d[0]);
}

static void op_CY_RANGE_set(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    int16_t b = cs_pop(cs);
    uint8_t d[] = { II_CY_RANGE, a, b >> 8, b & 0xff };
    tele_ii_tx(II_CY_ADDR, d, 4);
}

static void op_CY_RANGE_get(const void *data, scene_state_t *ss,
                            exec_state_t *es, command_state_t *cs) {
    int16_t a = cs_pop(cs);
    uint8_t d[] = { II_CY_RANGE | II_GET, a };
    uint8_t addr = II_CY_ADDR;
    tele_ii_tx_now(addr, d, 2);
    d[0] = 0;
    d[1] = 0;
    tele_ii_rx(addr, d, 2);
    cs_push(cs, (d[0] << 8) + d[1]);
}
