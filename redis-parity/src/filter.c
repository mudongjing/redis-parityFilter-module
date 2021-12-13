//
// Created by dongmu on 21.12.6.
//
#include "redismodule.h"
#include <stdlib.h>
#include "persist/persist.h"
#include "keyAddAndDelOp.h"


static int PFAdd_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    return addKeyAndValue(ctx,argv,argc);
}
static int PFDel_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    return delKey(ctx,argv,argc);
}
static bool PFJudge_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    return checkValueInKey(ctx,argv,argc);
}
static int PFAof_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    loadAof(ctx,argv,argc);
}
static void freeBitMap(bitMap bitmap){
    for (int i = 0; i < bitmap.effcLen; ++i) {
        RedisModule_Free(bitmap.oneChar[i]);
    }
    RedisModule_Free(&bitmap);
}
static void freeChain(sliceChain* chain){
    for (int i = 0; i < chain->len; ++i) {
        sliceBitMapArray*  array=chain->sliceArray[i];
        for (int j = 0; j < array->len; ++j) {
            sliceBitMap * slice=array->slice[j];
            RedisModule_Free(slice->mainBitMap);
            RedisModule_Free(slice->pureBitMap);
            RedisModule_Free(slice->stopBitMap);
            RedisModule_Free(slice->lastBitMap);
        }
        RedisModule_Free(array);
    }
    RedisModule_Free(chain);
}
static void PFFree(void* value) {
    pfEntry *pf=value;
    freeBitMap(pf->mainBitMap);
    freeBitMap(pf->pureBitMap);
    freeBitMap(pf->stopBitMap);
    freeBitMap(pf->lastBitMap);
    freeChain(pf->slicechain);
    RedisModule_Free(pf);
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    if (RedisModule_Init(ctx, "pfliter", 1, REDISMODULE_APIVER_1) !=
        REDISMODULE_OK) {
        return REDISMODULE_ERR;
    }


#define CREATE_CMD(name, tgt, attr)                                                                \
    do {                                                                                           \
        if (RedisModule_CreateCommand(ctx, name, tgt, attr, 1, 1, 1) != REDISMODULE_OK) {          \
            return REDISMODULE_ERR;                                                                \
        }                                                                                          \
    } while (0)
#define CREATE_WRCMD(name, tgt) CREATE_CMD(name, tgt, "write deny-oom")
#define CREATE_ROCMD(name, tgt) CREATE_CMD(name, tgt, "readonly fast")
    CREATE_WRCMD("PF.ADD", PFAdd_RedisCommand);
    CREATE_WRCMD("PF.DEL", PFDel_RedisCommand);
    CREATE_WRCMD("PF.EXIST", PFJudge_RedisCommand);
    CREATE_WRCMD("PF.LOAD",PFAof_RedisCommand);

    static RedisModuleTypeMethods typeprocs = {.version = REDISMODULE_TYPE_METHOD_VERSION,
            .rdb_load = PFRdbLoad,
            .rdb_save = PFRdbSave,
            .aof_rewrite = PFAofRewrite,
            .free = PFFree,
            .mem_usage = PFMemUsage};
    PFType = RedisModule_CreateDataType(ctx, "parityFli", 1, &typeprocs);
    if (PFType == NULL) {
        return REDISMODULE_ERR;
    }
}