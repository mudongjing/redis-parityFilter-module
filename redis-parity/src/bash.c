//
// Created by dongmu on 21.12.10.
//

#include "base.h"

pair getPAndW(int conflict){
    pair *res=RedisModule_Calloc(1,sizeof(res));
    res->copy=true;
    if (conflict<= 42 && conflict>=40){
        res->p=4;
        res->count=conflict-8-3;
        res->hight=32;
    }else if (conflict<=39 && conflict>=21){
        res->p=3;
        res->count=conflict-6-3;
        res->hight=32
    }else if (conflict<=20 && conflict>=17){
        res->p=2;
        res->count=conflict-4-3;
        res->hight=16;
    } else if (conflict==16){
        res->p=1;
        res->count=conflict-2-3;
        res->hight=8
    }else if (conflict>=13){
        res->copy=false;
        res->p=3;
        res->count=conflict-6;
        res->hight=32;
    }
    return *res;
}