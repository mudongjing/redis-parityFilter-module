//
// Created by dongmu on 21.12.9.
//

#include "base.h"

#ifndef REDIS_PARITY_FILTERINIT_H
#define REDIS_PARITY_FILTERINIT_H

#endif //REDIS_PARITY_FILTERINIT_H


#define BIGDATA_SIZE 28

// 一层的副本数量不能超过 2^6 个，也即前缀数量不超过6

// 最大的高度不超过 2^7


pfEntry initFilter(int conflict);