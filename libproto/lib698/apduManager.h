#ifndef APDUMANAGER_H
#define APDUMANAGER_H

#include "lib698.h"

#ifdef __cplusplus
extern "C" {
#endif

extern boolean userDataManager(u8* buf, u16 bufSize, apdu_p pApdu);

#ifdef __cplusplus
}
#endif

#endif // APDUMANAGER_H
