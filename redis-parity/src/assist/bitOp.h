//
// Created by dongmu on 21.12.6.
//

#include "base.h"
#ifndef REDIS_PARITY_BITOP_H
#define REDIS_PARITY_BITOP_H

#endif //REDIS_PARITY_BITOP_H

/*
 * 实现关于位图的操作。
 * 包括，对指定位置置为true，或false，又或是获取某个位置的值。
 * 隐含的，当对应的位置当前不存在，则自动添加位图，直到超过指定限值。
 */

void setTrueOrFalseOnChar(int pos,char* string,bool v);

bool getValueFromChar(int pos,char* string);


/**
 * 实际的位图是数组内部存储的大量的字符数组。对于位图的操作，我们一般就是在缪一个步骤知道当前的层数，哈希计算得到层数中的index,
 * 最后，给出的data 就是现在的位图，我们需要在其中找到对应位置的比特做修改。
 * 当index出现
 * @param layer
 * @param index
 * @param data
 * @param v
 */
void setTrueOrFalse(int layer,int index,bitMap bitmap,bool v);

/**
 *
 * @param layer
 * @param index
 * @param data
 * @return
 */
bool getValue(int layer,int index,bitMap bitmap);


int expanPfEntry(int targetLayer,pfEntry pf);