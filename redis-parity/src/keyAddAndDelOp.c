//
// Created by dongmu on 21.12.12.
//

#include "keyAddAndDelOp.h"
#include "filterInit.h"
#include "assist/parity.h"
#include "persist/persist.h"
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
static void bitmapCopy(bitMap pb,packBitMap bp,int high,int weight,int efflen){
    pb.effcLen=bp.effcLen;
    for (int i = 0; i < high; ++i) {
        if (i<efflen){
            char* s=malloc(weight);
            *s=*bp.oneChar[i];
            pb.oneChar[i]=s;
        } else pb.oneChar[i]=NULL;
    }
}
static void loadAof(RedisModuleCtx *ctx,RedisModuleString **argv,int argc){
    RedisModule_AutoMemory(ctx);
    RedisModule_ReplicateVerbatim(ctx);
    if (argc<3) return ;
    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
    pfEntry *pf;
    int status=getKey(key,PFType,pf);
    if (status!=PF_OK){
        size_t bufLen;
        const char* buf=RedisModule_StringPtrLen(argv[3], &bufLen);
        const pfChainHeader* header=(const void*)buf;
        pf=RedisModule_Calloc(1,sizeof(*pf));
        pf->isBigData=header->isBigData;
        pf->conflictP=header->conflictP;
        pf->layerStartStop=header->layerStartStop;
        pf->high=header->high;
        pf->weight=header->weight;
        pf->effLen=header->effLen;
        bitmapCopy(pf->mainBitMap,header->mainBitMap,header->high,header->weight,header->effLen);
        bitmapCopy(pf->pureBitMap,header->pureBitMap,header->high,header->weight,header->effLen);
        bitmapCopy(pf->stopBitMap,header->stopBitMap,header->high,header->weight,header->effLen);
        bitmapCopy(pf->lastBitMap,header->lastBitMap,header->high,header->weight*2,header->effLen);
        pf->slicechain->initLayer=header->slicechain->initLayer;
        pf->slicechain->len=header->slicechain->len;
        for (int i = 0; i < pf->slicechain->len; ++i) {
            sliceBitMapArray* psarr=malloc(sizeof(sliceBitMapArray));
            psarr->layer=header->slicechain->sliceArray[i]->layer;
            psarr->len=header->slicechain->sliceArray[i]->len;
            psarr->lastWeight=header->slicechain->sliceArray[i]->lastWeight;
            for (int j = 0; j < psarr->len; ++j) {
                sliceBitMap* psbm=malloc(sizeof(sliceBitMap));
                psbm->weight=header->slicechain->sliceArray[i]->slice[j]->weight;
                char* main=malloc(psbm->weight);
                char* pure=malloc(psbm->weight);
                char* stop=malloc(psbm->weight);
                char* last=malloc(psbm->weight*2);
                *main=*header->slicechain->sliceArray[i]->slice[j]->mainBitMap;
                *pure=*header->slicechain->sliceArray[i]->slice[j]->pureBitMap;
                *stop=*header->slicechain->sliceArray[i]->slice[j]->stopBitMap;
                *last=*header->slicechain->sliceArray[i]->slice[j]->lastBitMap;
                psbm->mainBitMap=main;
                psbm->pureBitMap=pure;
                psbm->stopBitMap=stop;
                psbm->lastBitMap=last;
                psarr->slice[j]=psbm;
            }
            pf->slicechain->sliceArray[i]=psarr;
        }
        RedisModule_ModuleTypeSetValue(key,PFType,pf);
    }
}