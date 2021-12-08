//
// Created by dongmu on 21.12.6.
//

#include "base.h"
#include "queue.h"
#ifndef REDIS_PARITY_PARITYHASH_H
#define REDIS_PARITY_PARITYHASH_H

#endif //REDIS_PARITY_PARITYHASH_H


//计算字符串哈希值
int getHashValue(char data[]);

// 对字符串做无限的变换操作
/*
 * original为原始的字符串，result为之前变换得到的结果，这次会在它的基础上在做操作，index为上次从原始字符串提取字符的位置
 */
StrAndIndex * loopChangeString(char original[],char result[],int index);

// 对指定过滤的指定位置的位修改为指定值，用于对位图添加元素或删除元素
// 不包括lastBitmap
// index 就是直接的对应层的位置
// 如果发现停留点出现冲突则返回false
bool trySet(Queue *pos,pfEntry pf,bool b);

bool judgeConflict(Queue *pos,pfEntry pf);

