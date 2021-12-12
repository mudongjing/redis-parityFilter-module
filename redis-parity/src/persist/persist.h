//
// Created by dongmu on 21.12.12.
//

#include "../redismodule.h"
#include "../assist/base.h"

#ifndef REDIS_PARITY_PERSIST_H
#define REDIS_PARITY_PERSIST_H

#endif //REDIS_PARITY_PERSIST_H

static void *PFRdbLoad(RedisModuleIO *io, int encver);
static void PFRdbSave(RedisModuleIO *io, void *obj);

static void PFAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value);

static size_t PFMemUsage(const void *value);