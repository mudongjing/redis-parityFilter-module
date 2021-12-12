//
// Created by dongmu on 21.12.6.
//


#include "parityHash.h"
#ifndef REDIS_PARITY_PARITY_H
#define REDIS_PARITY_PARITY_H

#endif //REDIS_PARITY_PARITY_H
/*
 * 利用编写的位图操作，负责元素插入和删除的具体操作，以及判断操作。
 *
 */


judgeAndRes addString(char* string, pfEntry* pf);

judgeAndRes delString(char* string,pfEntry* pf);

bool judgeString(char* string,pfEntry* pf);
