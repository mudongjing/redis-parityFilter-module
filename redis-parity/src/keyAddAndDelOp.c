//
// Created by dongmu on 21.12.12.
//

#include "keyAddAndDelOp.h"
#include "filterInit.h"
#include "assist/parity.h"
static pfEntry * createKey(RedisModuleKey *key,long long conflictp){
    pfEntry * pf=initFilter(conflictp);
    if (pf!=NULL){
        RedisModule_ModuleTypeSetValue(key, PFType, pf);
    }
    return pf;
}
static const char *statusStrerror(int status) {
    switch (status) {
        case PF_MISSING:
        case PF_EMPTY:
            return "ERR not found";
        case PF_MISMATCH:
            return REDISMODULE_ERRORMSG_WRONGTYPE;
        case PF_OK:
            return "ERR item exists";
        default:
            return "Unknown error";
    }
}
static int getKey(RedisModuleKey *key,RedisModuleType *expType,pfEntry* pf){
    pf=NULL;
    if (key==NULL)return PF_MISSING;
    int type = RedisModule_KeyType(key);
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
        return PF_EMPTY;
    }else if (type == REDISMODULE_KEYTYPE_MODULE && RedisModule_ModuleTypeGetType(key) == expType) {
        pf = RedisModule_ModuleTypeGetValue(key);
        return PF_OK;
    } else {
        return PF_MISMATCH;
    }
}

// pf.add 键名 元素字符串 可能的冲突概率
// 向指定键名传入元素，附带一个冲突概率，只会在初始化键的时候使用，其它时候即使输入概率也不会使用
static int addKeyAndValue(RedisModuleCtx *ctx,RedisModuleString **argv,int argc){
    RedisModule_AutoMemory(ctx);
    RedisModule_ReplicateVerbatim(ctx);
    if (argc<3) return REDISMODULE_ERR;
    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
    pfEntry *pf;
    int status=getKey(key,PFType,pf);
    if (status== PF_EMPTY){
        long long conflict;
        if (argc==4){
            RedisModule_StringToLongLong(argv[3],&conflict);
            pf=createKey(key,conflict);
        }
    }else if (status != PF_OK){
        return RedisModule_ReplyWithError(ctx, statusStrerror(status));
    }
    size_t n;
    const char* ele=RedisModule_StringPtrLen(argv[2],&n);
    judgeAndRes res=addString(ele,pf);
    if (res.judge == false){
        return RedisModule_ReplyWithError(ctx,res.descrip);
    }
    return REDISMODULE_OK;
}

// pf.del 键名 元素字符串
static int delKey(RedisModuleCtx *ctx,RedisModuleString **argv,int argc){
    RedisModule_AutoMemory(ctx);
    RedisModule_ReplicateVerbatim(ctx);
    if (argc<3) return REDISMODULE_ERR;
    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
    pfEntry *pf;
    int status=getKey(key,PFType,pf);
    if (status == PF_OK){
        size_t n;
        const char* ele=RedisModule_StringPtrLen(argv[2],&n);
        delString(ele,pf);
        return REDISMODULE_OK;
    } else return REDISMODULE_ERR;
}

// pf.exist 键名 元素字符串
static bool checkValueInKey(RedisModuleCtx *ctx,RedisModuleString **argv,int argc){
    RedisModule_AutoMemory(ctx);
    RedisModule_ReplicateVerbatim(ctx);
    if (argc<3) return REDISMODULE_ERR;
    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
    pfEntry *pf;
    int status=getKey(key,PFType,pf);
    if (status == PF_OK){
        size_t n;
        const char* ele=RedisModule_StringPtrLen(argv[2],&n);
        bool res=judgeString(ele,pf);
        return res;
    } else return false;
}