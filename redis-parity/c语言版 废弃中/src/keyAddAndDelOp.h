//
// Created by dongmu on 21.12.12.
//
#include "assist/base.h"
#ifndef REDIS_PARITY_KEYADDANDDELOP_H
#define REDIS_PARITY_KEYADDANDDELOP_H

#endif //REDIS_PARITY_KEYADDANDDELOP_H


static RedisModuleType *PFType;

static int getKey(RedisModuleKey *key,RedisModuleType *expType,pfEntry* pf);

static int addKeyAndValue(RedisModuleCtx *ctx,RedisModuleString **argv,int argc);
static int delKey(RedisModuleCtx *ctx,RedisModuleString **argv,int argc);
static bool checkValueInKey(RedisModuleCtx *ctx,RedisModuleString **argv,int argc);

static void loadAof(RedisModuleCtx *ctx,RedisModuleString **argv,int argc);