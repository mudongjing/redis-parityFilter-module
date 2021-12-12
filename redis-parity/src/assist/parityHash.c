//
// Created by dongmu on 21.12.6.
//

#include "parityHash.h"
#include "bitOp.h"
#include "../sliceAndPfOp.h"
// 对给定的字符串计算对应的哈希值,并返回正数
int getHashValue(char data[]){
    const int p = 16777619;
    int hash = (int) 2166136261L;

    for (int i = 0; i < strlen(data); i++) hash = (hash ^ data[i]) * p;
    hash += hash << 13;
    hash ^= hash >>7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return abs(hash);
}

char* expanString(char* data){
    int len=sizeof(data)/sizeof(char);
    if (len - strlen(data) < 8){
        char* newdata = RedisModule_Calloc(1,len + 8);
        for (int i = 0; i < len; ++i) {
            newdata[i]=data[i];
        }
        return newdata;
    }else return data;
}

// 在原始的字符串基础上，不断的循环向后添加字符
StrAndIndex * loopChangeString(char original[],char data[],int index){
    StrAndIndex *sat =RedisModule_Calloc(1,sizeof(*sat));
    int len=strlen(original)-1;
    if (data==NULL){
        sat->index=len;
        char* newOriginal=RedisModule_Calloc(1,sizeof(original));
        strcpy(newOriginal,original);
        sat->result=newOriginal;
        return sat;
    }
    if (index>=len) index=0;
    else index++;
    data=expanString(data);
    strcat(data,original[index]);

    sat->index=index;
    sat->result=data;
    return sat;
}

// 判断时，如果发现冲突，返回true
bool setOrJudge(Queue *pos,pfEntry pf,bool set){
    bool isBig = pf.isBigData;
    int minEffStopLayer = pf.layerStartStop;// 最低的有效停留层
    sliceChain* chain = pf.slicechain;
    sliceBitMapArray* sliceArray=chain->sliceArray;
    int chainSize;
    if(chain!=NULL){
        chainSize=chain->len; // 具有副本的层数
    }
    int maxEffStopLayer=minEffStopLayer+chainSize-1;
    QNode* node=pos->front;
    if (isBig==true){
        int tmp=0;
        while(tmp<minEffStopLayer){
            node=node->next;
            tmp++;
        }
    }
    if (set==NULL){//表示仅作判断冲突
        int effLen=pf.effLen;
        for (int i = 0; i < effLen; ++i) {
            int index=node->data;
            node=node->next;
            bool isStop=false;
            if (node==NULL) isStop=true;
            if (i<minEffStopLayer){
                if(setOrJudgeForPf(index,i,pf,NULL,false,NULL)==true) return true;
            }else if (i>= minEffStopLayer && i<= maxEffStopLayer){
                if(setOrJudgeForSlice(index,sliceArray[i-minEffStopLayer],NULL,isStop,pos)==true)
                    return true;
            }else{
                if (setOrJudgeForPf(index,i,pf,NULL,isStop,pos)==true) return true;
            }
        }
    }else{//直接上手根据 set值操作
        int high=pf.high;
        for (int i = 0; i < high; ++i) {
            int index=node->data;
            node=node->next;
            bool isStop=false;
            if (node==NULL) isStop=true;
            if (i<minEffStopLayer){
                setOrJudgeForPf(index,i,pf,set,isStop,pos);
            }else if (i>= minEffStopLayer && i<= maxEffStopLayer){
                setOrJudgeForSlice(index,sliceArray[i-minEffStopLayer],set,isStop,pos);
            }else{
                setOrJudgeForPf(index,i,pf,set,isStop,pos);
            }
            if (isStop==true) return true;
        }
        return false;
    }
}



bool trySet(Queue *pos,pfEntry pf,bool b){
    // 先判断是否存在冲突
    // 不存在，再做添加，或存在，再做删除
    bool conflict=judgeConflict(pos,pf);
    if (conflict==true && b==false){//出现冲突，删除
        setOrJudge(pos,pf,b);
    }else if(conflict==false && b==true){// 没有冲突，加入
        setOrJudge(pos,pf,b);
    }else{
        return false;
    }
    return true;
}

bool judgeConflict(Queue *pos,pfEntry pf){
    return setOrJudge(pos,pf,NULL);
}
