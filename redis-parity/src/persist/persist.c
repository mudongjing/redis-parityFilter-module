//
// Created by dongmu on 21.12.12.
//

#include "persist.h"

static void bitMapLoad(RedisModuleIO *io,bitMap mpsl,int high){
    bitMap *bitmap=RedisModule_Calloc(1,sizeof(bitMap));
    bitmap->effcLen=RedisModule_LoadUnsigned(io);
    for (int i = 0; i < high; ++i) {
        char* s=RedisModule_Calloc(1,sizeof(s));
        size_t  weight;
        s=RedisModule_LoadStringBuffer(io,&weight);
        bitmap->oneChar[i]=s;
    }
    mpsl=*bitmap;
}

static void *PFRdbLoad(RedisModuleIO *io, int encver){
    if (encver > 1) {
        return NULL;
    }
    pfEntry * pf=RedisModule_Calloc(1,sizeof(pf));
    pf->isBigData=RedisModule_LoadSigned(io);
    pf->conflictP=RedisModule_LoadUnsigned(io);
    pf->layerStartStop=RedisModule_LoadUnsigned(io);
    pf->high=RedisModule_LoadUnsigned(io);
    pf->weight=RedisModule_LoadUnsigned(io);
    pf->effLen=RedisModule_LoadUnsigned(io);

    bitMapLoad(io,pf->mainBitMap,pf->high);
    bitMapLoad(io,pf->pureBitMap,pf->high);
    bitMapLoad(io,pf->stopBitMap,pf->high);
    bitMapLoad(io,pf->lastBitMap,pf->high);
    pf->slicechain=RedisModule_Calloc(1,sizeof(void*));
    pf->slicechain->initLayer=RedisModule_LoadUnsigned(io);
    pf->slicechain->len=RedisModule_LoadUnsigned(io);
    pf->slicechain->sliceArray=RedisModule_Calloc(1,sizeof(void*));
    for (int i = 0; i < pf->slicechain->len; ++i) {
        pf->slicechain->sliceArray[i]->layer=RedisModule_LoadUnsigned(io);
        pf->slicechain->sliceArray[i]->len=RedisModule_LoadUnsigned(io);
        pf->slicechain->sliceArray[i]->lastWeight=RedisModule_LoadUnsigned(io);
        uint32_t len=pf->slicechain->sliceArray[i]->len;
        pf->slicechain->sliceArray[i]->slice=RedisModule_Calloc(1,sizeof(void*));
        for (int j = 0; j < len; ++j) {
            pf->slicechain->sliceArray[i]->slice[i]->weight=RedisModule_LoadUnsigned(io);
            size_t weight;
            pf->slicechain->sliceArray[i]->slice[i]->mainBitMap=RedisModule_Calloc(1,sizeof(char*));
            pf->slicechain->sliceArray[i]->slice[i]->mainBitMap=RedisModule_LoadStringBuffer(io,&weight);

            pf->slicechain->sliceArray[i]->slice[i]->pureBitMap=RedisModule_Calloc(1,sizeof(char*));
            pf->slicechain->sliceArray[i]->slice[i]->pureBitMap=RedisModule_LoadStringBuffer(io,&weight);

            pf->slicechain->sliceArray[i]->slice[i]->stopBitMap=RedisModule_Calloc(1,sizeof(char*));
            pf->slicechain->sliceArray[i]->slice[i]->stopBitMap=RedisModule_LoadStringBuffer(io,&weight);

            pf->slicechain->sliceArray[i]->slice[i]->lastBitMap=RedisModule_Calloc(1,sizeof(char*));
            pf->slicechain->sliceArray[i]->slice[i]->lastBitMap=RedisModule_LoadStringBuffer(io,&weight);
        }
    }
    return pf;
}

static void saveBitMap(RedisModuleIO *io,bitMap bitmap,int size,int high){
    RedisModule_SaveUnsigned(io,bitmap.effcLen);
    char** onechar=bitmap.oneChar;
    for (int i = 0; i < high; ++i) {
        RedisModule_SaveStringBuffer(io,(const char*)onechar[i],size);
    }
}
static void saveSliceMap(RedisModuleIO *io,sliceBitMap** map,int len){
    for (int i = 0; i < len; ++i) {
        sliceBitMap * slice=map[i];
        RedisModule_SaveUnsigned(io,slice->weight);
        RedisModule_SaveStringBuffer(io,slice->mainBitMap,slice->weight);
        RedisModule_SaveStringBuffer(io,slice->pureBitMap,slice->weight);
        RedisModule_SaveStringBuffer(io,slice->stopBitMap,slice->weight);
        RedisModule_SaveStringBuffer(io,slice->lastBitMap,slice->weight*2);
    }
}

static void PFRdbSave(RedisModuleIO *io, void *obj){
    pfEntry* pf=obj;
    RedisModule_SaveSigned(io,pf->isBigData);
    RedisModule_SaveUnsigned(io,pf->conflictP);
    RedisModule_SaveUnsigned(io,pf->layerStartStop);
    RedisModule_SaveUnsigned(io,pf->high);
    RedisModule_SaveUnsigned(io,pf->weight);
    RedisModule_SaveUnsigned(io,pf->effLen);

    saveBitMap(io,pf->mainBitMap,pf->weight,pf->high);
    saveBitMap(io,pf->pureBitMap,pf->weight,pf->high);
    saveBitMap(io,pf->stopBitMap,pf->weight,pf->high);
    saveBitMap(io,pf->lastBitMap,pf->weight*2,pf->high);

    RedisModule_SaveUnsigned(io,pf->slicechain->initLayer);
    RedisModule_SaveUnsigned(io,pf->slicechain->len);
    sliceBitMapArray ** chainArray=pf->slicechain->sliceArray;
    for (int i = 0; i < pf->slicechain->len; ++i) {
        sliceBitMapArray * array=chainArray[i];
        RedisModule_SaveUnsigned(io,array->layer);
        RedisModule_SaveUnsigned(io,array->len);
        RedisModule_SaveUnsigned(io,array->lastWeight);
        saveSliceMap(io,array->slice,array->len);
    }
}

static void PFAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value){
    pfEntry *pf=value;

}

static size_t PFMemUsage(const void *value){
    const pfEntry *pf=value;
    size_t size=sizeof(*pf);
    size += sizeof(pf->mainBitMap)*4;
    size += sizeof(pf->mainBitMap.oneChar[0])*pf->mainBitMap.effcLen*3;
    size += sizeof(pf->lastBitMap.oneChar[0])*pf->mainBitMap.effcLen;
    size += sizeof(*pf->slicechain);
    for (int i = 0; i < pf->slicechain->len; ++i) {
        sliceBitMapArray*  arr=pf->slicechain->sliceArray[i];
        size += sizeof(*arr);
        for (int j = 0; j < arr->len; ++j) {
            size += sizeof(*arr->slice[j]);
        }
    }
    return size;
}