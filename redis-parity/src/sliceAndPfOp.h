//
// Created by dongmu on 21.12.9.
//
#include "queue.h"
#include "bitOp.h"
#ifndef REDIS_PARITY_SLICEANDPFOP_H
#define REDIS_PARITY_SLICEANDPFOP_H

#endif //REDIS_PARITY_SLICEANDPFOP_H

int calPosQ(Queue *pos,uint64_t weight);

bool setOrJudgeForPf(int pos,int layer,pfEntry pf,bool isSet,bool isStop,Queue *posQ);
bool setOrJudgeForSlice(int pos,sliceBitMapArray sliceArray,bool isSet,bool isStop,Queue *posQ);