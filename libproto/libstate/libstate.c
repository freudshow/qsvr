#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libstate.h"

typedef struct {
    u8 cur;
    u8 next;

}trans_s;

typedef enum {
    state_start = 0,
    state_len,
    state_ctl,
    state_user_data,
    state_cs,
    state_end
} state3762_e;

typedef enum {
    state_infoDomain = 0,
    state_address,
    state_afn,
    state_data
}state3762userData_e;

//static u8 StateTransTable3762[][] = {

//};


int recgonizeFrame(u8* buf, u16 len)
{
    (void)buf;
    (void)len;

    return 0;
}
