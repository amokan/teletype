#include <ctype.h>   // isdigit
#include <stdint.h>  // types
#include <stdio.h>   // printf
#include <stdlib.h>  // rand, strtol
#include <string.h>

#include "euclidean/euclidean.h"
#include "ii.h"
#include "table.h"
#include "teletype.h"
#include "util.h"

#ifdef SIM
#define DBG printf("%s", dbg);
#else
#include "print_funcs.h"
#define DBG print_dbg(dbg);
#endif


static const char *errordesc[] = { "OK",
                                   WELCOME,
                                   "UNKNOWN WORD",
                                   "COMMAND TOO LONG",
                                   "NOT ENOUGH PARAMS",
                                   "TOO MANY PARAMS",
                                   "MOD NOT ALLOWED HERE",
                                   "EXTRA SEPARATOR",
                                   "NEED SEPARATOR",
                                   "BAD SEPARATOR",
                                   "MOVE LEFT" };

const char *tele_error(error_t e) {
    return errordesc[e];
}

// static char dbg[32];
static char pcmd[32];


int16_t output, output_new;

char error_detail[16];

uint8_t mutes[8];

tele_pattern_t tele_patterns[4];

static uint8_t pn;

static char condition;

static tele_command_t tele_stack[TELE_STACK_SIZE];
static uint8_t tele_stack_top;
static uint8_t left;


volatile update_metro_t update_metro;
volatile update_tr_t update_tr;
volatile update_cv_t update_cv;
volatile update_cv_slew_t update_cv_slew;
volatile update_delay_t update_delay;
volatile update_s_t update_s;
volatile update_cv_off_t update_cv_off;
volatile update_ii_t update_ii;
volatile update_scene_t update_scene;
volatile update_pi_t update_pi;
volatile update_kill_t update_kill;
volatile update_mute_t update_mute;
volatile update_input_t update_input;

volatile run_script_t run_script;

volatile uint8_t input_states[8];

const char *to_v(int16_t);


/////////////////////////////////////////////////////////////////
// STACK ////////////////////////////////////////////////////////

static int16_t pop(void);
static void push(int16_t);

static int16_t top;
static int16_t stack[STACK_SIZE];

int16_t pop() {
    top--;
    // sprintf(dbg,"\r\npop %d", stack[top]);
    return stack[top];
}

void push(int16_t data) {
    stack[top] = data;
    // sprintf(dbg,"\r\npush %d", stack[top]);
    top++;
}


/////////////////////////////////////////////////////////////////
// VARS ARRAYS KEYS /////////////////////////////////////////////


#define KEYS 47
static tele_key_t tele_keys[KEYS] = {
    { "WW.PRESET", WW_PRESET },     { "WW.POS", WW_POS },
    { "WW.SYNC", WW_SYNC },         { "WW.START", WW_START },
    { "WW.END", WW_END },           { "WW.PMODE", WW_PMODE },
    { "WW.PATTERN", WW_PATTERN },   { "WW.QPATTERN", WW_QPATTERN },
    { "WW.MUTE1", WW_MUTE1 },       { "WW.MUTE2", WW_MUTE2 },
    { "WW.MUTE3", WW_MUTE3 },       { "WW.MUTE4", WW_MUTE4 },
    { "WW.MUTEA", WW_MUTEA },       { "WW.MUTEB", WW_MUTEB },
    { "MP.PRESET", MP_PRESET },     { "MP.RESET", MP_RESET },
    { "MP.SYNC", MP_SYNC },         { "MP.MUTE", MP_MUTE },
    { "MP.UNMUTE", MP_UNMUTE },     { "MP.FREEZE", MP_FREEZE },
    { "MP.UNFREEZE", MP_UNFREEZE }, { "MP.STOP", MP_STOP },
    { "ES.PRESET", ES_PRESET },     { "ES.MODE", ES_MODE },
    { "ES.CLOCK", ES_CLOCK },       { "ES.RESET", ES_RESET },
    { "ES.PATTERN", ES_PATTERN },   { "ES.TRANS", ES_TRANS },
    { "ES.STOP", ES_STOP },         { "ES.TRIPLE", ES_TRIPLE },
    { "ES.MAGIC", ES_MAGIC },       { "OR.TRK", ORCA_TRACK },
    { "OR.CLK", ORCA_CLOCK },       { "OR.DIV", ORCA_DIVISOR },
    { "OR.PHASE", ORCA_PHASE },     { "OR.RST", ORCA_RESET },
    { "OR.WGT", ORCA_WEIGHT },      { "OR.MUTE", ORCA_MUTE },
    { "OR.SCALE", ORCA_SCALE },     { "OR.BANK", ORCA_BANK },
    { "OR.PRESET", ORCA_PRESET },   { "OR.RELOAD", ORCA_RELOAD },
    { "OR.ROTS", ORCA_ROTATES },    { "OR.ROTW", ORCA_ROTATEW },
    { "OR.GRST", ORCA_GRESET },     { "OR.CVA", ORCA_CVA },
    { "OR.CVB", ORCA_CVB }
};


// {NAME,VAL}

// ENUM IN HEADER

static void v_P_N(void);
static void v_M(void);
static void v_M_ACT(void);
static void v_P_L(void);
static void v_P_I(void);
static void v_P_HERE(void);
static void v_P_NEXT(void);
static void v_P_PREV(void);
static void v_P_WRAP(void);
static void v_P_START(void);
static void v_P_END(void);
static void v_O(void);
static void v_DRUNK(void);
static void v_Q(void);
static void v_Q_N(void);
static void v_Q_AVG(void);
static void v_SCENE(void);
static void v_FLIP(void);

static int16_t tele_q[16];

#define VARS 39
static tele_var_t tele_vars[VARS] = {
    { "I", NULL, 0 },  // gets overwritten by ITER
    { "TIME", NULL, 0 },
    { "TIME.ACT", NULL, 1 },
    { "IN", NULL, 0 },
    { "PARAM", NULL, 0 },
    { "PRESET", NULL, 0 },
    { "M", v_M, 1000 },
    { "M.ACT", v_M_ACT, 1 },
    { "X", NULL, 0 },
    { "Y", NULL, 0 },
    { "Z", NULL, 0 },
    { "T", NULL, 0 },
    { "A", NULL, 1 },
    { "B", NULL, 2 },
    { "C", NULL, 3 },
    { "D", NULL, 4 },
    { "O", v_O, 0 },
    { "DRUNK", v_DRUNK, 0 },
    { "Q", v_Q, 0 },
    { "Q.N", v_Q_N, 1 },
    { "Q.AVG", v_Q_AVG, 0 },
    { "SCENE", v_SCENE, 0 },
    { "P.N", v_P_N, 0 },
    { "P.L", v_P_L, 0 },
    { "P.I", v_P_I, 0 },
    { "P.HERE", v_P_HERE, 0 },
    { "P.NEXT", v_P_NEXT, 0 },
    { "P.PREV", v_P_PREV, 0 },
    { "P.WRAP", v_P_WRAP, 0 },
    { "P.START", v_P_START, 0 },
    { "P.END", v_P_END, 0 },
    { "FLIP", v_FLIP, 0 },
    { "O.MIN", NULL, 0 },
    { "O.MAX", NULL, 63 },
    { "O.WRAP", NULL, 1 },
    { "O.DIR", NULL, 1 },
    { "DRUNK.MIN", NULL, 0 },
    { "DRUNK.MAX", NULL, 255 },
    { "DRUNK.WRAP", NULL, 0 }
};

static void v_M() {
    if (left || top == 0)
        push(tele_vars[V_M].v);
    else {
        tele_vars[V_M].v = pop();
        if (tele_vars[V_M].v < 10) tele_vars[V_M].v = 10;
        (*update_metro)(tele_vars[V_M].v, tele_vars[V_M_ACT].v, 0);
    }
}

static void v_M_ACT() {
    if (left || top == 0)
        push(tele_vars[V_M_ACT].v);
    else {
        tele_vars[V_M_ACT].v = pop();
        if (tele_vars[V_M_ACT].v != 0) tele_vars[V_M_ACT].v = 1;
        (*update_metro)(tele_vars[V_M].v, tele_vars[V_M_ACT].v, 0);
    }
}

static void v_P_N() {
    int16_t a;
    if (left || top == 0) { push(pn); }
    else {
        a = pop();
        if (a < 0)
            pn = 0;
        else if (a > 3)
            pn = 3;
        else
            pn = a;
    }
}

static void v_P_L() {
    int16_t a;
    if (left || top == 0) { push(tele_patterns[pn].l); }
    else {
        a = pop();
        if (a < 0)
            tele_patterns[pn].l = 0;
        else if (a > 63)
            tele_patterns[pn].l = 63;
        else
            tele_patterns[pn].l = a;
    }
}

static void v_P_I() {
    int16_t a;
    if (left || top == 0) { push(tele_patterns[pn].i); }
    else {
        a = pop();
        if (a < 0)
            tele_patterns[pn].i = 0;
        else if (a > tele_patterns[pn].l)
            tele_patterns[pn].i = tele_patterns[pn].l;
        else
            tele_patterns[pn].i = a;
        (*update_pi)();
    }
}

static void v_P_HERE() {
    int16_t a;
    if (left || top == 0) { push(tele_patterns[pn].v[tele_patterns[pn].i]); }
    else {
        a = pop();
        tele_patterns[pn].v[tele_patterns[pn].i] = a;
    }
}

static void v_P_NEXT() {
    int16_t a;
    if ((tele_patterns[pn].i == (tele_patterns[pn].l - 1)) ||
        (tele_patterns[pn].i == tele_patterns[pn].end)) {
        if (tele_patterns[pn].wrap)
            tele_patterns[pn].i = tele_patterns[pn].start;
    }
    else
        tele_patterns[pn].i++;

    if (tele_patterns[pn].i > tele_patterns[pn].l) tele_patterns[pn].i = 0;

    if (left || top == 0) { push(tele_patterns[pn].v[tele_patterns[pn].i]); }
    else {
        a = pop();
        tele_patterns[pn].v[tele_patterns[pn].i] = a;
    }

    (*update_pi)();
}

static void v_P_PREV() {
    int16_t a;
    if ((tele_patterns[pn].i == 0) ||
        (tele_patterns[pn].i == tele_patterns[pn].start)) {
        if (tele_patterns[pn].wrap) {
            if (tele_patterns[pn].end < tele_patterns[pn].l)
                tele_patterns[pn].i = tele_patterns[pn].end;
            else
                tele_patterns[pn].i = tele_patterns[pn].l - 1;
        }
    }
    else
        tele_patterns[pn].i--;

    if (left || top == 0) { push(tele_patterns[pn].v[tele_patterns[pn].i]); }
    else {
        a = pop();
        tele_patterns[pn].v[tele_patterns[pn].i] = a;
    }

    (*update_pi)();
}

static void v_P_WRAP() {
    int16_t a;
    if (left || top == 0) { push(tele_patterns[pn].wrap); }
    else {
        a = pop();
        if (a < 0)
            tele_patterns[pn].wrap = 0;
        else if (a > 1)
            tele_patterns[pn].wrap = 1;
        else
            tele_patterns[pn].wrap = a;
    }
}

static void v_P_START() {
    int16_t a;
    if (left || top == 0) { push(tele_patterns[pn].start); }
    else {
        a = pop();
        if (a < 0)
            tele_patterns[pn].start = 0;
        else if (a > 63)
            tele_patterns[pn].start = 1;
        else
            tele_patterns[pn].start = a;
    }
}

static void v_P_END() {
    int16_t a;
    if (left || top == 0) { push(tele_patterns[pn].end); }
    else {
        a = pop();
        if (a < 0)
            tele_patterns[pn].end = 0;
        else if (a > 63)
            tele_patterns[pn].end = 1;
        else
            tele_patterns[pn].end = a;
    }
}

static void v_O() {
    if (left || top == 0) {
        tele_vars[V_O].v += tele_vars[V_O_DIR].v;
        if (tele_vars[V_O].v > tele_vars[V_O_MAX].v) {
            if (tele_vars[V_O_WRAP].v)
                tele_vars[V_O].v = tele_vars[V_O_MIN].v;
            else
                tele_vars[V_O].v = tele_vars[V_O_MAX].v;
        }
        else if (tele_vars[V_O].v < tele_vars[V_O_MIN].v) {
            if (tele_vars[V_O_WRAP].v)
                tele_vars[V_O].v = tele_vars[V_O_MAX].v;
            else
                tele_vars[V_O].v = tele_vars[V_O_MIN].v;
        }

        push(tele_vars[V_O].v);
    }
    else {
        tele_vars[V_O].v = pop();
    }
}

static void v_DRUNK() {
    if (left || top == 0) {
        tele_vars[V_DRUNK].v += (rand() % 3) - 1;
        if (tele_vars[V_DRUNK].v < tele_vars[V_DRUNK_MIN].v) {
            if (tele_vars[V_DRUNK_WRAP].v)
                tele_vars[V_DRUNK].v = tele_vars[V_DRUNK_MAX].v;
            else
                tele_vars[V_DRUNK].v = tele_vars[V_DRUNK_MIN].v;
        }
        else if (tele_vars[V_DRUNK].v > tele_vars[V_DRUNK_MAX].v) {
            if (tele_vars[V_DRUNK_WRAP].v)
                tele_vars[V_DRUNK].v = tele_vars[V_DRUNK_MIN].v;
            else
                tele_vars[V_DRUNK].v = tele_vars[V_DRUNK_MAX].v;
        }
        push(tele_vars[V_DRUNK].v);
    }
    else {
        tele_vars[V_DRUNK].v = pop();
    }
}

static void v_Q() {
    if (left || top == 0) { push(tele_q[tele_vars[V_Q_N].v - 1]); }
    else {
        for (int16_t i = 15; i > 0; i--) tele_q[i] = tele_q[i - 1];
        tele_q[0] = pop();
    }
}
static void v_Q_N() {
    if (left || top == 0) { push(tele_vars[V_Q_N].v); }
    else {
        int16_t a = pop();
        if (a < 1)
            a = 1;
        else if (a > 16)
            a = 16;
        tele_vars[V_Q_N].v = a;
    }
}

static void v_Q_AVG() {
    if (left || top == 0) {
        int32_t avg = 0;
        for (int16_t i = 0; i < tele_vars[V_Q_N].v; i++) avg += tele_q[i];
        avg /= tele_vars[V_Q_N].v;

        push(avg);
    }
    else {
        int16_t a = pop();
        for (int16_t i = 0; i < 16; i++) tele_q[i] = a;
    }
}
static void v_SCENE() {
    if (left || top == 0) { push(tele_vars[V_SCENE].v); }
    else {
        tele_vars[V_SCENE].v = pop();
        (*update_scene)(tele_vars[V_SCENE].v);
    }
}
static void v_FLIP() {
    if (left || top == 0) {
        push(tele_vars[V_O].v);
        tele_vars[V_O].v = (tele_vars[V_O].v == 0);
    }
    else {
        tele_vars[V_O].v = (pop() != 0);
    }
}


static void a_TR(uint8_t);
static void a_CV(uint8_t);
static void a_CV_SLEW(uint8_t);
static void a_CV_OFF(uint8_t);
static void a_TR_TIME(uint8_t);
static void a_TR_POL(uint8_t);

#define MAKEARRAY(name, func) \
    { #name, { 0, 0, 0, 0 }, func }
#define ARRAYS 6
static tele_array_t tele_arrays[ARRAYS] = {
    MAKEARRAY(TR, a_TR),
    MAKEARRAY(CV, a_CV),
    MAKEARRAY(CV.SLEW, a_CV_SLEW),
    MAKEARRAY(CV.OFF, a_CV_OFF),
    { "TR.TIME", { 100, 100, 100, 100 }, a_TR_TIME },
    { "TR.POL", { 1, 1, 1, 1 }, a_TR_POL }
};

static void a_TR(uint8_t i) {
    int16_t a = pop();
    a = (a != 0);

    tele_arrays[0].v[i] = a;
    (*update_tr)(i, a);
}
static void a_CV(uint8_t i) {
    int16_t a = pop();
    if (a < 0)
        a = 0;
    else if (a > 16383)
        a = 16383;
    tele_arrays[1].v[i] = a;
    (*update_cv)(i, a, 1);
}
static void a_CV_SLEW(uint8_t i) {
    int16_t a = pop();
    if (a < 1)
        a = 1;  // min slew = 1
    else if (a > 32767)
        a = 32767;
    tele_arrays[2].v[i] = a;
    (*update_cv_slew)(i, a);
}
static void a_CV_OFF(uint8_t i) {
    int16_t a = pop();
    tele_arrays[3].v[i] = a;
    (*update_cv_off)(i, a);
    (*update_cv)(i, tele_arrays[1].v[i], 1);
}
static void a_TR_TIME(uint8_t i) {
    int16_t a = pop();
    if (a < 0) a = 0;  // 0 = no pulse
    tele_arrays[4].v[i] = a;
}
static void a_TR_POL(uint8_t i) {
    int16_t a = pop();
    if (a > 1)
        a = 1;
    else if (a < 0)
        a = 0;
    tele_arrays[5].v[i] = a;
}


/////////////////////////////////////////////////////////////////
// DELAY ////////////////////////////////////////////////////////

static tele_command_t delay_c[TELE_D_SIZE];
static int16_t delay_t[TELE_D_SIZE];
static uint8_t delay_count;
static int16_t tr_pulse[4];

static void process_delays(uint8_t);
void clear_delays(void);

static void process_delays(uint8_t v) {
    for (int16_t i = 0; i < TELE_D_SIZE; i++) {
        if (delay_t[i]) {
            delay_t[i] -= v;
            if (delay_t[i] <= 0) {
                // sprintf(dbg,"\r\ndelay %d", i);
                // DBG
                process(&delay_c[i]);
                delay_t[i] = 0;
                delay_count--;
                if (delay_count == 0) (*update_delay)(0);
            }
        }
    }

    for (int16_t i = 0; i < 4; i++) {
        if (tr_pulse[i]) {
            tr_pulse[i] -= v;
            if (tr_pulse[i] <= 0) {
                tr_pulse[i] = 0;
                // if(tele_arrays[0].v[i]) tele_arrays[0].v[i] = 0;
                tele_arrays[0].v[i] = (tele_arrays[5].v[i] == 0);
                (*update_tr)(i, tele_arrays[0].v[i]);
            }
        }
    }
}

void clear_delays(void) {
    for (int16_t i = 0; i < 4; i++) tr_pulse[i] = 0;

    for (int16_t i = 0; i < TELE_D_SIZE; i++) { delay_t[i] = 0; }

    delay_count = 0;

    tele_stack_top = 0;

    (*update_delay)(0);
    (*update_s)(0);
}


/////////////////////////////////////////////////////////////////
// MOD //////////////////////////////////////////////////////////

static void mod_PROB(tele_command_t *);
static void mod_DEL(tele_command_t *);
static void mod_S(tele_command_t *);
static void mod_IF(tele_command_t *);
static void mod_ELIF(tele_command_t *);
static void mod_ELSE(tele_command_t *);
static void mod_L(tele_command_t *);

#define MAKEMOD(name, params, doc) \
    { #name, mod_##name, params, doc }
#define MODS 7
static const tele_mod_t tele_mods[MODS] = {
    MAKEMOD(PROB, 1, "PROBABILITY TO CONTINUE EXECUTING LINE"),
    MAKEMOD(DEL, 1, "DELAY THIS COMMAND"),
    MAKEMOD(S, 0, "ADD COMMAND TO STACK"),
    MAKEMOD(IF, 1, "IF CONDITION FOR COMMAND"),
    MAKEMOD(ELIF, 1, "ELSE IF"),
    MAKEMOD(ELSE, 0, "ELSE"),
    MAKEMOD(L, 2, "LOOPED COMMAND WITH ITERATION")
};


void mod_PROB(tele_command_t *c) {
    int16_t a = pop();

    tele_command_t cc;
    if (rand() % 101 < a) {
        cc.l = c->l - c->separator - 1;
        cc.separator = -1;
        memcpy(cc.data, &c->data[c->separator + 1], cc.l * sizeof(tele_data_t));
        // sprintf(dbg,"\r\nsub-length: %d", cc.l);
        process(&cc);
    }
}
void mod_DEL(tele_command_t *c) {
    int16_t i = 0;
    int16_t a = pop();

    if (a < 1) a = 1;

    while (delay_t[i] != 0 && i != TELE_D_SIZE) i++;

    if (i < TELE_D_SIZE) {
        delay_count++;
        if (delay_count == 1) (*update_delay)(1);
        delay_t[i] = a;

        delay_c[i].l = c->l - c->separator - 1;
        delay_c[i].separator = -1;

        memcpy(delay_c[i].data, &c->data[c->separator + 1],
               delay_c[i].l * sizeof(tele_data_t));
    }
}
void mod_S(tele_command_t *c) {
    if (tele_stack_top < TELE_STACK_SIZE) {
        tele_stack[tele_stack_top].l = c->l - c->separator - 1;
        memcpy(tele_stack[tele_stack_top].data, &c->data[c->separator + 1],
               tele_stack[tele_stack_top].l * sizeof(tele_data_t));
        tele_stack[tele_stack_top].separator = -1;
        tele_stack_top++;
        if (tele_stack_top == 1) (*update_s)(1);
    }
}
void mod_IF(tele_command_t *c) {
    condition = FALSE;
    tele_command_t cc;
    if (pop()) {
        condition = TRUE;
        cc.l = c->l - c->separator - 1;
        cc.separator = -1;
        memcpy(cc.data, &c->data[c->separator + 1], cc.l * sizeof(tele_data_t));
        // sprintf(dbg,"\r\nsub-length: %d", cc.l);
        process(&cc);
    }
}
void mod_ELIF(tele_command_t *c) {
    tele_command_t cc;
    if (!condition) {
        if (pop()) {
            condition = TRUE;
            cc.l = c->l - c->separator - 1;
            cc.separator = -1;
            memcpy(cc.data, &c->data[c->separator + 1],
                   cc.l * sizeof(tele_data_t));
            // sprintf(dbg,"\r\nsub-length: %d", cc.l);
            process(&cc);
        }
    }
}
void mod_ELSE(tele_command_t *c) {
    tele_command_t cc;
    if (!condition) {
        condition = TRUE;
        cc.l = c->l - c->separator - 1;
        cc.separator = -1;
        memcpy(cc.data, &c->data[c->separator + 1], cc.l * sizeof(tele_data_t));
        // sprintf(dbg,"\r\nsub-length: %d", cc.l);
        process(&cc);
    }
}
void mod_L(tele_command_t *c) {
    int16_t a, b, d, i;
    tele_command_t cc;
    a = pop();
    b = pop();

    if (a < b) {
        d = b - a + 1;
        for (i = 0; i < d; i++) {
            tele_vars[V_I].v = a + i;
            cc.l = c->l - c->separator - 1;
            cc.separator = -1;
            memcpy(cc.data, &c->data[c->separator + 1],
                   cc.l * sizeof(tele_data_t));
            // sprintf(dbg,"\r\nsub-length: %d", cc.l);
            process(&cc);
        }
    }
    else {
        d = a - b + 1;
        for (i = 0; i < d; i++) {
            tele_vars[V_I].v = a - i;
            cc.l = c->l - c->separator - 1;
            cc.separator = -1;
            memcpy(cc.data, &c->data[c->separator + 1],
                   cc.l * sizeof(tele_data_t));
            // sprintf(dbg,"\r\nsub-length: %d", cc.l);
            process(&cc);
        }
    }
}


/////////////////////////////////////////////////////////////////
// OPS //////////////////////////////////////////////////////////

static void op_ADD(const void *data);
static void op_SUB(const void *data);
static void op_MUL(const void *data);
static void op_DIV(const void *data);
static void op_MOD(const void *data);
static void op_RAND(const void *data);
static void op_RRAND(const void *data);
static void op_TOSS(const void *data);
static void op_MIN(const void *data);
static void op_MAX(const void *data);
static void op_LIM(const void *data);
static void op_WRAP(const void *data);
static void op_QT(const void *data);
static void op_AVG(const void *data);
static void op_EQ(const void *data);
static void op_NE(const void *data);
static void op_LT(const void *data);
static void op_GT(const void *data);
static void op_NZ(const void *data);
static void op_EZ(const void *data);
static void op_TR_TOG(const void *data);
static void op_N(const void *data);
static void op_S_ALL(const void *data);
static void op_S_POP(const void *data);
static void op_S_CLR(const void *data);
static void op_DEL_CLR(const void *data);
static void op_M_RESET(const void *data);
static void op_V(const void *data);
static void op_VV(const void *data);
static void op_P(const void *data);
static void op_P_INS(const void *data);
static void op_P_RM(const void *data);
static void op_P_PUSH(const void *data);
static void op_P_POP(const void *data);
static void op_PN(const void *data);
static void op_TR_PULSE(const void *data);
static void op_II(const void *data);
static void op_RSH(const void *data);
static void op_LSH(const void *data);
static void op_S_L(const void *data);
static void op_CV_SET(const void *data);
static void op_EXP(const void *data);
static void op_ABS(const void *data);
static void op_AND(const void *data);
static void op_OR(const void *data);
static void op_XOR(const void *data);
static void op_JI(const void *data);
static void op_SCRIPT(const void *data);
static void op_KILL(const void *data);
static void op_MUTE(const void *data);
static void op_UNMUTE(const void *data);
static void op_SCALE(const void *data);
static void op_STATE(const void *data);
static void op_ER(const void *data);


// clang-format off
#define MAKEOP(n, f, p, r, d) \
    { .name = #n, .get = f, .set = NULL, .params = p, .returns = r, .data = NULL, .doc = d }
#define OPS 54
// DO NOT INSERT in the middle. there's a hack in validate() for P and PN
static const tele_op_t tele_ops[OPS] = {
    MAKEOP(ADD     , op_ADD     , 2, true , "[A B] ADD A TO B"                          ),
    MAKEOP(SUB     , op_SUB     , 2, true , "[A B] SUBTRACT B FROM A"                   ),
    MAKEOP(MUL     , op_MUL     , 2, true , "[A B] MULTIPLY TWO VALUES"                 ),
    MAKEOP(DIV     , op_DIV     , 2, true , "[A B] DIVIDE FIRST BY SECOND"              ),
    MAKEOP(MOD     , op_MOD     , 2, true , "[A B] MOD FIRST BY SECOND"                 ),
    MAKEOP(RAND    , op_RAND    , 1, true , "[A] RETURN RANDOM NUMBER UP TO A"          ),
    MAKEOP(RRAND   , op_RRAND   , 2, true , "[A B] RETURN RANDOM NUMBER BETWEEN A AND B"),
    MAKEOP(TOSS    , op_TOSS    , 0, true , "RETURN RANDOM STATE"                       ),
    MAKEOP(MIN     , op_MIN     , 2, true , "RETURN LESSER OF TWO VALUES"               ),
    MAKEOP(MAX     , op_MAX     , 2, true , "RETURN GREATER OF TWO VALUES"              ),
    MAKEOP(LIM     , op_LIM     , 3, true , "[A B C] LIMIT C TO RANGE A TO B"           ),
    MAKEOP(WRAP    , op_WRAP    , 3, true , "[A B C] WRAP C WITHIN RANGE A TO B"        ),
    MAKEOP(QT      , op_QT      , 2, true , "[A B] QUANTIZE A TO STEP SIZE B"           ),
    MAKEOP(AVG     , op_AVG     , 2, true , "AVERAGE TWO VALUES"                        ),
    MAKEOP(EQ      , op_EQ      , 2, true , "LOGIC: EQUAL"                              ),
    MAKEOP(NE      , op_NE      , 2, true , "LOGIC: NOT EQUAL"                          ),
    MAKEOP(LT      , op_LT      , 2, true , "LOGIC: LESS THAN"                          ),
    MAKEOP(GT      , op_GT      , 2, true , "LOGIC: GREATER THAN"                       ),
    MAKEOP(NZ      , op_NZ      , 1, true , "LOGIC: NOT ZERO"                           ),
    MAKEOP(EZ      , op_EZ      , 1, true , "LOGIC: EQUALS ZERO"                        ),
    MAKEOP(TR.TOG  , op_TR_TOG  , 1, false, "[A] TOGGLE TRIGGER A"                      ),
    MAKEOP(N       , op_N       , 1, true , "TABLE FOR NOTE VALUES"                     ),
    MAKEOP(S.ALL   , op_S_ALL   , 0, false, "S: EXECUTE ALL"                            ),
    MAKEOP(S.POP   , op_S_POP   , 0, false, "S: POP LAST"                               ),
    MAKEOP(S.CLR   , op_S_CLR   , 0, false, "S: FLUSH"                                  ),
    MAKEOP(DEL.CLR , op_DEL_CLR , 0, false, "DELAY: FLUSH"                              ),
    MAKEOP(M.RESET , op_M_RESET , 0, false, "METRO: RESET"                              ),
    MAKEOP(V       , op_V       , 1, true , "TO VOLT"                                   ),
    MAKEOP(VV      , op_VV      , 1, true , "TO VOLT WITH PRECISION"                    ),
    MAKEOP(P       , op_P       , 1, true , "PATTERN: GET/SET"                          ),
    MAKEOP(P.INS   , op_P_INS   , 2, false, "PATTERN: INSERT"                           ),
    MAKEOP(P.RM    , op_P_RM    , 1, true , "PATTERN: REMOVE"                           ),
    MAKEOP(P.PUSH  , op_P_PUSH  , 1, false, "PATTERN: PUSH"                             ),
    MAKEOP(P.POP   , op_P_POP   , 0, true , "PATTERN: POP"                              ),
    MAKEOP(PN      , op_PN      , 2, true , "PATTERN: GET/SET N"                        ),
    MAKEOP(TR.PULSE, op_TR_PULSE, 1, false, "PULSE TRIGGER"                             ),
    MAKEOP(II      , op_II      , 2, false, "II"                                        ),
    MAKEOP(RSH     , op_RSH     , 2, true , "RIGHT SHIFT"                               ),
    MAKEOP(LSH     , op_LSH     , 2, true , "LEFT SHIFT"                                ),
    MAKEOP(S.L     , op_S_L     , 0, true , "STACK LENGTH"                              ),
    MAKEOP(CV.SET  , op_CV_SET  , 2, false, "CV SET"                                    ),
    MAKEOP(EXP     , op_EXP     , 1, true , "EXPONENTIATE"                              ),
    MAKEOP(ABS     , op_ABS     , 1, true , "ABSOLUTE VALUE"                            ),
    MAKEOP(AND     , op_AND     , 2, true , "LOGIC: AND"                                ),
    MAKEOP(OR      , op_OR      , 2, true , "LOGIC: OR"                                 ),
    MAKEOP(XOR     , op_XOR     , 2, true , "LOGIC: XOR"                                ),
    MAKEOP(JI      , op_JI      , 2, true , "JUST INTONE DIVISON"                       ),
    MAKEOP(SCRIPT  , op_SCRIPT  , 1, false, "CALL SCRIPT"                               ),
    MAKEOP(KILL    , op_KILL    , 0, false, "CLEAR DELAYS, STACK, SLEW"                 ),
    MAKEOP(MUTE    , op_MUTE    , 1, false, "MUTE INPUT"                                ),
    MAKEOP(UNMUTE  , op_UNMUTE  , 1, false, "UNMUTE INPUT"                              ),
    MAKEOP(SCALE   , op_SCALE   , 5, true , "SCALE NUMBER RANGES"                       ),
    MAKEOP(STATE   , op_STATE   , 1, true , "GET INPUT STATE"                           ),
    MAKEOP(ER      , op_ER      , 3, true , "EUCLIDEAN RHYTHMS"                         )
};
// clang-format on

static void op_ADD(const void *data) {
    push(pop() + pop());
}
static void op_SUB(const void *data) {
    push(pop() - pop());
}
static void op_MUL(const void *data) {
    push(pop() * pop());
}
static void op_DIV(const void *data) {
    push(pop() / pop());
}
// can be optimized:
static void op_MOD(const void *data) {
    push(pop() % pop());
}
static void op_RAND(const void *data) {
    int16_t a = pop();
    if (a == -1)
        push(0);
    else
        push(rand() % (a + 1));
}
static void op_RRAND(const void *data) {
    int16_t a, b, min, max, range;
    a = pop();
    b = pop();
    if (a < b) {
        min = a;
        max = b;
    }
    else {
        min = b;
        max = a;
    }
    range = max - min + 1;
    if (range == 0)
        push(a);
    else
        push(rand() % range + min);
}
static void op_TOSS(const void *data) {
    push(rand() & 1);
}
static void op_MIN(const void *data) {
    int16_t a, b;
    a = pop();
    b = pop();
    if (b > a)
        push(a);
    else
        push(b);
}
static void op_MAX(const void *data) {
    int16_t a, b;
    a = pop();
    b = pop();
    if (a > b)
        push(a);
    else
        push(b);
}
static void op_LIM(const void *data) {
    int16_t a, b, i;
    i = pop();
    a = pop();
    b = pop();
    if (i < a)
        push(a);
    else if (i > b)
        push(b);
    else
        push(i);
}
static void op_WRAP(const void *data) {
    int16_t a, b, i, c;
    i = pop();
    a = pop();
    b = pop();
    if (a < b) {
        c = b - a + 1;
        while (i >= b) i -= c;
        while (i < a) i += c;
    }
    else {
        c = a - b + 1;
        while (i >= a) i -= c;
        while (i < b) i += c;
    }
    push(i);
}
static void op_QT(const void *data) {
    // this rounds negative numbers rather than quantize (choose closer)
    int16_t a, b, c, d, e;
    b = pop();
    a = pop();

    c = b / a;
    d = c * a;
    e = (c + 1) * a;

    if (abs(b - d) < abs(b - e))
        push(d);
    else
        push(e);
}
static void op_AVG(const void *data) {
    push((pop() + pop()) >> 1);
}
static void op_EQ(const void *data) {
    push(pop() == pop());
}
static void op_NE(const void *data) {
    push(pop() != pop());
}
static void op_LT(const void *data) {
    push(pop() < pop());
}
static void op_GT(const void *data) {
    push(pop() > pop());
}
static void op_NZ(const void *data) {
    push(pop() != 0);
}
static void op_EZ(const void *data) {
    push(pop() == 0);
}
static void op_TR_TOG(const void *data) {
    int16_t a = pop();
    // saturate and shift
    if (a < 1)
        a = 1;
    else if (a > 4)
        a = 4;
    a--;
    if (tele_arrays[0].v[a])
        tele_arrays[0].v[a] = 0;
    else
        tele_arrays[0].v[a] = 1;
    update_tr(a, tele_arrays[0].v[a]);
}
static void op_N(const void *data) {
    int16_t a = pop();

    if (a < 0) {
        if (a < -127) a = -127;
        a = -a;
        push(-table_n[a]);
    }
    else {
        if (a > 127) a = 127;
        push(table_n[a]);
    }
}
static void op_S_ALL(const void *data) {
    for (int16_t i = 0; i < tele_stack_top; i++)
        process(&tele_stack[tele_stack_top - i - 1]);
    tele_stack_top = 0;
    (*update_s)(0);
}
static void op_S_POP(const void *data) {
    if (tele_stack_top) {
        tele_stack_top--;
        process(&tele_stack[tele_stack_top]);
        if (tele_stack_top == 0) (*update_s)(0);
    }
}
static void op_S_CLR(const void *data) {
    tele_stack_top = 0;
    (*update_s)(0);
}
static void op_DEL_CLR(const void *data) {
    clear_delays();
}
static void op_M_RESET(const void *data) {
    (*update_metro)(tele_vars[V_M].v, tele_vars[V_M_ACT].v, 1);
}
static void op_V(const void *data) {
    int16_t a = pop();
    if (a > 10)
        a = 10;
    else if (a < -10)
        a = -10;

    if (a < 0) {
        a = -a;
        push(-table_v[a]);
    }
    else
        push(table_v[a]);
}
static void op_VV(const void *data) {
    uint8_t negative = 1;
    int16_t a = pop();
    if (a < 0) {
        negative = -1;
        a = -a;
    }
    if (a > 1000) a = 1000;

    push(negative * (table_v[a / 100] + table_vv[a % 100]));
}
static void op_P(const void *data) {
    int16_t a, b;
    a = pop();
    if (a < 0) {
        if (tele_patterns[pn].l == 0)
            a = 0;
        else if (a < -tele_patterns[pn].l)
            a = 0;
        else
            a = tele_patterns[pn].l + a;
    }
    if (a > 63) a = 63;

    if (left == 0 && top > 0) {
        b = pop();
        tele_patterns[pn].v[a] = b;
        (*update_pi)();
    }
    else {
        push(tele_patterns[pn].v[a]);
    }
}
static void op_P_INS(const void *data) {
    int16_t a, b, i;
    a = pop();
    b = pop();

    if (a < 0) {
        if (tele_patterns[pn].l == 0)
            a = 0;
        else if (a < -tele_patterns[pn].l)
            a = 0;
        else
            a = tele_patterns[pn].l + a;
    }
    if (a > 63) a = 63;

    if (tele_patterns[pn].l >= a) {
        for (i = tele_patterns[pn].l; i > a; i--)
            tele_patterns[pn].v[i] = tele_patterns[pn].v[i - 1];
        if (tele_patterns[pn].l < 63) tele_patterns[pn].l++;
    }

    tele_patterns[pn].v[a] = b;
    (*update_pi)();
}
static void op_P_RM(const void *data) {
    int16_t a, i;
    a = pop();

    if (a < 0) {
        if (tele_patterns[pn].l == 0)
            a = 0;
        else if (a < -tele_patterns[pn].l)
            a = 0;
        else
            a = tele_patterns[pn].l + a;
    }
    else if (a > tele_patterns[pn].l)
        a = tele_patterns[pn].l;

    if (tele_patterns[pn].l > 0) {
        push(tele_patterns[pn].v[a]);
        for (i = a; i < tele_patterns[pn].l; i++)
            tele_patterns[pn].v[i] = tele_patterns[pn].v[i + 1];

        tele_patterns[pn].l--;
    }
    else
        push(0);
    (*update_pi)();
}
static void op_P_PUSH(const void *data) {
    int16_t a;
    a = pop();

    if (tele_patterns[pn].l < 64) {
        tele_patterns[pn].v[tele_patterns[pn].l] = a;
        tele_patterns[pn].l++;
        (*update_pi)();
    }
}
static void op_P_POP(const void *data) {
    if (tele_patterns[pn].l > 0) {
        tele_patterns[pn].l--;
        push(tele_patterns[pn].v[tele_patterns[pn].l]);
        (*update_pi)();
    }
    else
        push(0);
}
static void op_PN(const void *data) {
    int16_t a, b, c;
    a = pop();
    b = pop();

    if (a < 0)
        a = 0;
    else if (a > 3)
        a = 3;

    if (b < 0) {
        if (tele_patterns[a].l == 0)
            b = 0;
        else if (b < -tele_patterns[a].l)
            b = 0;
        else
            b = tele_patterns[a].l + b;
    }
    if (b > 63) b = 63;

    if (left == 0 && top > 0) {
        c = pop();
        tele_patterns[a].v[b] = c;
        (*update_pi)();
    }
    else {
        push(tele_patterns[a].v[b]);
    }
}
static void op_TR_PULSE(const void *data) {
    int16_t a = pop();
    // saturate and shift
    if (a < 1)
        a = 1;
    else if (a > 4)
        a = 4;
    a--;
    int16_t time = tele_arrays[4].v[a];  // pulse time
    if (time <= 0) return;               // if time <= 0 don't do anything
    tele_arrays[0].v[a] = tele_arrays[5].v[a];
    tr_pulse[a] = time;  // set time
    update_tr(a, tele_arrays[0].v[a]);
}
static void op_II(const void *data) {
    int16_t a = pop();
    int16_t b = pop();
    update_ii(a, b);
}
static void op_RSH(const void *data) {
    push(pop() >> pop());
}
static void op_LSH(const void *data) {
    push(pop() << pop());
}
static void op_S_L(const void *data) {
    push(tele_stack_top);
}
static void op_CV_SET(const void *data) {
    int16_t a = pop();
    int16_t b = pop();
    // saturate and shift
    if (a < 1)
        a = 1;
    else if (a > 4)
        a = 4;
    a--;
    if (b < 0)
        b = 0;
    else if (b > 16383)
        b = 16383;
    tele_arrays[1].v[a] = b;
    (*update_cv)(a, b, 0);
}
static void op_EXP(const void *data) {
    int16_t a = pop();
    if (a > 16383)
        a = 16383;
    else if (a < -16383)
        a = -16383;

    a = a >> 6;

    if (a < 0) {
        a = -a;
        push(-table_exp[a]);
    }
    else
        push(table_exp[a]);
}
static void op_ABS(const void *data) {
    int16_t a = pop();

    if (a < 0)
        push(-a);
    else
        push(a);
}
static void op_AND(const void *data) {
    push(pop() & pop());
}
static void op_OR(const void *data) {
    push(pop() | pop());
}
static void op_XOR(const void *data) {
    push(pop() ^ pop());
}
static void op_JI(const void *data) {
    uint32_t ji = (((pop() << 8) / pop()) * 1684) >> 8;
    while (ji > 1683) ji >>= 1;
    push(ji);
}
static void op_SCRIPT(const void *data) {
    uint16_t a = pop();
    if (a > 0 && a < 9) (*run_script)(a);
}
static void op_KILL(const void *data) {
    clear_delays();
    (*update_kill)();
}
static void op_MUTE(const void *data) {
    int16_t a;
    a = pop();

    if (a > 0 && a < 9) { (*update_mute)(a - 1, 0); }
}
static void op_UNMUTE(const void *data) {
    int16_t a;
    a = pop();

    if (a > 0 && a < 9) { (*update_mute)(a - 1, 1); }
}
static void op_SCALE(const void *data) {
    int16_t a, b, x, y, i;
    a = pop();
    b = pop();
    x = pop();
    y = pop();
    i = pop();

    push((i - a) * (y - x) / (b - a) + x);
}
static void op_STATE(const void *data) {
    int16_t a = pop();
    a--;
    if (a < 0)
        a = 0;
    else if (a > 7)
        a = 7;

    (*update_input)(a);
    push(input_states[a]);
}

static void op_ER(const void *data) {
    int16_t fill = pop();
    int16_t len = pop();
    int16_t step = pop();
    push(euclidean(fill, len, step));
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// PROCESS //////////////////////////////////////////////////////

error_t parse(char *cmd, tele_command_t *out) {
    char cmd_copy[32];
    strcpy(cmd_copy, cmd);
    const char *delim = " \n";
    const char *s = strtok(cmd_copy, delim);

    uint8_t n = 0;
    out->l = n;

    // sprintf(dbg,"\r\nparse: ");

    while (s) {
        // CHECK IF NUMBER
        if (isdigit(s[0]) || s[0] == '-') {
            out->data[n].t = NUMBER;
            out->data[n].v = strtol(s, NULL, 0);
        }
        else if (s[0] == ':')
            out->data[n].t = SEP;
        else {
            // CHECK AGAINST VARS
            int16_t i = VARS - 1;

            do {
                // print_dbg("\r\nvar '");
                // print_dbg(tele_vars[i].name);
                // print_dbg("'");

                if (!strcmp(s, tele_vars[i].name)) {
                    out->data[n].t = VAR;
                    out->data[n].v = i;
                    // sprintf(dbg,"v(%d) ", out->data[n].v);
                    break;
                }
            } while (i--);

            if (i == -1) {
                // CHECK AGAINST ARRAYS
                i = ARRAYS;

                while (i--) {
                    //   	print_dbg("\r\narrays '");
                    // print_dbg(tele_arrays[i].name);
                    // print_dbg("'");

                    if (!strcmp(s, tele_arrays[i].name)) {
                        out->data[n].t = ARRAY;
                        out->data[n].v = i;
                        // sprintf(dbg,"a(%d) ", out->data[n].v);
                        break;
                    }
                }
            }

            if (i == -1) {
                // CHECK AGAINST OPS
                i = OPS;

                while (i--) {
                    //   	print_dbg("\r\nops '");
                    // print_dbg(tele_ops[i].name);
                    // print_dbg("'");

                    if (!strcmp(s, tele_ops[i].name)) {
                        out->data[n].t = OP;
                        out->data[n].v = i;
                        // sprintf(dbg,"f(%d) ", out->data[n].v);
                        break;
                    }
                }
            }

            if (i == -1) {
                // CHECK AGAINST MOD
                i = MODS;

                while (i--) {
                    //   	print_dbg("\r\nmods '");
                    // print_dbg(tele_mods[i].name);
                    // print_dbg("'");

                    if (!strcmp(s, tele_mods[i].name)) {
                        out->data[n].t = MOD;
                        out->data[n].v = i;
                        // sprintf(dbg,"f(%d) ", out->data[n].v);
                        break;
                    }
                }
            }

            if (i == -1) {
                // CHECK AGAINST KEY
                i = KEYS;

                while (i--) {
                    //   	print_dbg("\r\nmods '");
                    // print_dbg(tele_mods[i].name);
                    // print_dbg("'");

                    if (!strcmp(s, tele_keys[i].name)) {
                        out->data[n].t = KEY;
                        out->data[n].v = i;
                        // sprintf(dbg,"f(%d) ", out->data[n].v);
                        break;
                    }
                }
            }

            if (i == -1) {
                strcpy(error_detail, s);
                return E_PARSE;
            }
        }

        s = strtok(NULL, delim);

        n++;
        out->l = n;

        if (n == COMMAND_MAX_LENGTH) return E_LENGTH;
    }

    // sprintf(dbg,"// length: %d", temp.l);

    return E_OK;
}


/////////////////////////////////////////////////////////////////
// VALIDATE /////////////////////////////////////////////////////

error_t validate(tele_command_t *c) {
    int16_t h = 0;
    uint8_t n = c->l;
    c->separator = -1;

    while (n--) {
        if (c->data[n].t == OP) {
            if (tele_ops[c->data[n].v].returns == false && n) {
                if (c->data[n - 1].t != SEP) {
                    strcpy(error_detail, tele_ops[c->data[n].v].name);
                    return E_NOT_LEFT;
                }
            }

            h -= tele_ops[c->data[n].v].params;

            if (h < 0) {
                strcpy(error_detail, tele_ops[c->data[n].v].name);
                return E_NEED_PARAMS;
            }
            h += tele_ops[c->data[n].v].returns ? 1 : 0;
            // hack for var-length params for P
            if (c->data[n].v == 29 || c->data[n].v == 34) {
                if (n == 0)
                    h--;
                else if (c->data[n - 1].t == SEP)
                    h--;
            }
        }
        else if (c->data[n].t == MOD) {
            strcpy(error_detail, tele_mods[c->data[n].v].name);
            if (n != 0)
                return E_NO_MOD_HERE;
            else if (c->separator == -1)
                return E_NEED_SEP;
            else if (h < tele_mods[c->data[n].v].params)
                return E_NEED_PARAMS;
            else if (h > tele_mods[c->data[n].v].params)
                return E_EXTRA_PARAMS;
            else
                h = 0;
        }
        else if (c->data[n].t == SEP) {
            if (c->separator != -1)
                return E_MANY_SEP;
            else if (n == 0)
                return E_PLACE_SEP;

            c->separator = n;
            if (h > 1)
                return E_EXTRA_PARAMS;
            else
                h = 0;
        }

        // RIGHT (get)
        else if (n && c->data[n - 1].t != SEP) {
            if (c->data[n].t == NUMBER || c->data[n].t == VAR ||
                c->data[n].t == KEY) {
                h++;
            }
            else if (c->data[n].t == ARRAY) {
                if (h < 1) {
                    strcpy(error_detail, tele_arrays[c->data[n].v].name);
                    return E_NEED_PARAMS;
                }
                // h-- then h++
            }
        }
        // LEFT (set)
        else {
            if (c->data[n].t == NUMBER || c->data[n].t == KEY) { h++; }
            else if (c->data[n].t == VAR) {
                if (h == 0) h++;
                // else {
                // 	h--;
                // 	if(h > 0)
                // 		return E_EXTRA_PARAMS;
                // }
            }
            else if (c->data[n].t == ARRAY) {
                if (h < 1) {
                    strcpy(error_detail, tele_arrays[c->data[n].v].name);
                    return E_NEED_PARAMS;
                }
                h--;
                if (h == 0) h++;
                // else if(h > 1)
                // return E_EXTRA_PARAMS;
            }
        }
    }

    if (h > 1)
        return E_EXTRA_PARAMS;
    else
        return E_OK;
}


/////////////////////////////////////////////////////////////////
// PROCESS //////////////////////////////////////////////////////

void process(tele_command_t *c) {
    top = 0;
    left = 0;
    int16_t i;
    int16_t n;

    if (c->separator == -1)
        n = c->l;
    else
        n = c->separator;

    // sprintf(dbg,"\r\r\nprocess (%d): %s", n, print_command(c));
    // DBG;

    while (n--) {
        left = n;
        if (c->data[n].t == NUMBER)
            push(c->data[n].v);
        else if (c->data[n].t == OP) {
            const void *data = tele_ops[c->data[n].v].data;
            tele_ops[c->data[n].v].get(data);
        }
        else if (c->data[n].t == MOD)
            tele_mods[c->data[n].v].func(c);
        else if (c->data[n].t == VAR) {
            if (tele_vars[c->data[n].v].func == NULL) {
                if (n || top == 0)
                    push(tele_vars[c->data[n].v].v);
                else
                    tele_vars[c->data[n].v].v = pop();
            }
            else
                tele_vars[c->data[n].v].func();
        }
        else if (c->data[n].t == ARRAY) {
            i = pop();

            // saturate for 1-4 indexing
            if (i < 1)
                i = 1;
            else if (i > 3)
                i = 4;
            i--;

            if (n || top == 0) {
                // sprintf(dbg,"\r\nget array %s @ %d : %d",
                // tele_arrays[c->data[n].v].name, i,
                // tele_arrays[c->data[n].v].v[i]);
                // DBG
                push(tele_arrays[c->data[n].v].v[i]);
            }
            else {
                // sprintf(dbg,"\r\nset array %s @ %d to %d",
                // tele_arrays[c->data[n].v].name, i,
                // tele_arrays[c->data[n].v].v[i]);
                // DBG
                if (tele_arrays[c->data[n].v].func)
                    tele_arrays[c->data[n].v].func(i);
                else
                    tele_arrays[c->data[n].v].v[i] = pop();
            }
        }
        else if (c->data[n].t == KEY) {
            push(tele_keys[c->data[n].v].v);
        }
    }

    // PRint16_t DEBUG OUTPUT IF VAL LEFT ON STACK
    if (top) {
        output = pop();
        output_new++;
        // sprintf(dbg,"\r\n>>> %d", output);
        // DBG
        // to_v(output);
    }
}


char *print_command(const tele_command_t *c) {
    int16_t n = 0;
    char number[8];
    char *p = pcmd;

    *p = 0;

    while (n < c->l) {
        switch (c->data[n].t) {
            case OP:
                strcpy(p, tele_ops[c->data[n].v].name);
                p += strlen(tele_ops[c->data[n].v].name) - 1;
                break;
            case ARRAY:
                strcpy(p, tele_arrays[c->data[n].v].name);
                p += strlen(tele_arrays[c->data[n].v].name) - 1;
                break;
            case NUMBER:
                itoa(c->data[n].v, number, 10);
                strcpy(p, number);
                p += strlen(number) - 1;
                break;
            case VAR:
                strcpy(p, tele_vars[c->data[n].v].name);
                p += strlen(tele_vars[c->data[n].v].name) - 1;
                break;
            case MOD:
                strcpy(p, tele_mods[c->data[n].v].name);
                p += strlen(tele_mods[c->data[n].v].name) - 1;
                break;
            case SEP: *p = ':'; break;
            case KEY:
                strcpy(p, tele_keys[c->data[n].v].name);
                p += strlen(tele_keys[c->data[n].v].name) - 1;
                break;
            default: break;
        }

        n++;
        p++;
        *p = ' ';
        p++;
    }
    p--;
    *p = 0;

    return pcmd;
}


int16_t tele_get_array(uint8_t a, uint8_t i) {
    return tele_arrays[a].v[i];
}

void tele_set_array(uint8_t a, uint8_t i, uint16_t v) {
    tele_arrays[a].v[i] = v;
}

void tele_set_val(uint8_t i, uint16_t v) {
    tele_vars[i].v = v;
}

void tele_tick(uint8_t i) {
    process_delays(i);

    // inc time
    if (tele_vars[V_TIME_ACT].v) tele_vars[V_TIME].v += i;
}

void tele_init() {
    u8 i;

    for (i = 0; i < 4; i++) {
        tele_patterns[i].i = 0;
        tele_patterns[i].l = 0;
        tele_patterns[i].wrap = 1;
        tele_patterns[i].start = 0;
        tele_patterns[i].end = 63;
    }

    for (i = 0; i < 8; i++) mutes[i] = 1;
}


const char *to_v(int16_t i) {
    static char n[3], v[7];
    int16_t a = 0, b = 0;

    if (i > table_v[8]) {
        i -= table_v[8];
        a += 8;
    }

    if (i > table_v[4]) {
        i -= table_v[4];
        a += 4;
    }

    if (i > table_v[2]) {
        i -= table_v[2];
        a += 2;
    }

    if (i > table_v[1]) {
        i -= table_v[1];
        a += 1;
    }

    if (i > table_vv[64]) {
        i -= table_vv[64];
        b += 64;
    }

    if (i > table_vv[32]) {
        i -= table_vv[32];
        b += 32;
    }

    if (i > table_vv[16]) {
        i -= table_vv[16];
        b += 16;
    }

    if (i > table_vv[8]) {
        i -= table_vv[8];
        b += 8;
    }

    if (i > table_vv[4]) {
        i -= table_vv[4];
        b += 4;
    }

    if (i > table_vv[2]) {
        i -= table_vv[2];
        b++;
    }

    if (i > table_vv[1]) {
        i -= table_vv[1];
        b++;
    }

    b++;

    itoa(a, n, 10);
    strcpy(v, n);
    strcat(v, ".");
    itoa(b, n, 10);
    strcat(v, n);
    strcat(v, "V");

    return v;
    // printf(" (VV %d %d)",a,b);
}
