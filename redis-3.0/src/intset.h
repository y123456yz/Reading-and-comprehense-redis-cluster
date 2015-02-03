/*
 * Copyright (c) 2009-2012, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __INTSET_H
#define __INTSET_H
#include <stdint.h>

/*
整数集合 API?

函数             作用                                     时间复杂度

intsetNew       创建一个新的整数集合。                          O(1) 
intsetAdd       将给定元素添加到整数集合里面。                  O(N) 
intsetRemove    从整数集合中移除给定元素。                      O(N) 
intsetFind      检查给定值是否存在于集合。                      因为底层数组有序，查找可以通过二分查找法来进行， 所以复杂度为 O(\log N) 。 
intsetRandom    从整数集合中随机返回一个元素。                  O(1) 
intsetGet       取出底层数组在给定索引上的元素。                O(1) 
intsetLen       返回整数集合包含的元素个数。                    O(1) 
intsetBlobLen   返回整数集合占用的内存字节数。                  O(1) 

*/

/*
升级
每当我们要将一个新元素添加到整数集合里面， 并且新元素的类型比整数集合现有所有元素的类型都要长时， 整数集合需要先进行
升级（upgrade）， 然后才能将新元素添加到整数集合里面。

降级
整数集合不支持降级操作， 一旦对数组进行了升级， 编码就会一直保持升级后的状态。
举个例子， 对于图 6-11 所示的整数集合来说， 即使我们将集合里唯一一个真正需要使用 int64_t 类型来保存的元素 4294967295 删除了， 
整数集合的编码仍然会维持 INTSET_ENC_INT64 ， 底层数组也仍然会是 int64_t 类型的，


升级整数集合并添加新元素共分为三步进行：
1.根据新元素的类型， 扩展整数集合底层数组的空间大小， 并为新元素分配空间。
2.将底层数组现有的所有元素都转换成与新元素相同的类型， 并将类型转换后的元素放置到正确的位上， 而且在放置元素的过程中， 
  需要继续维持底层数组的有序性质不变。
3.将新元素添加到底层数组里面。
*/
typedef struct intset {

    /*
 虽然 intset 结构将 contents 属性声明为 int8_t 类型的数组， 但实际上 contents 数组并不保存任何 int8_t 类型的值 —— 
 contents 数组的真正类型取决于 encoding 属性的值：
 
 ?如果 encoding 属性的值为 INTSET_ENC_INT16 ， 那么 contents 就是一个 int16_t 类型的数组， 数组里的每个项都是一个 int16_t 
 类型的整数值 （最小值为 -32,768 ，最大值为 32,767 ）。
 ?如果 encoding 属性的值为 INTSET_ENC_INT32 ， 那么 contents 就是一个 int32_t 类型的数组， 数组里的每个项都是一个 int32_t 
 类型的整数值 （最小值为 -2,147,483,648 ，最大值为 2,147,483,647 ）。
 ?如果 encoding 属性的值为 INTSET_ENC_INT64 ， 那么 contents 就是一个 int64_t 类型的数组， 数组里的每个项都是一个 int64_t 
 类型的整数值 （最小值为 -9,223,372,036,854,775,808 ，最大值为 9,223,372,036,854,775,807 ）。
 */
    // 编码方式 // 保存元素所使用的类型的长度
    uint32_t encoding;//注意:当向一个底层为 int16_t 数组的整数集合添加一个 int64_t 类型的整数值时， 整数集合已有的所有元素都会被转换成 int64_t 类型

    // 集合包含的元素数量
    uint32_t length; // 元素个数 length 属性记录了整数集合包含的元素数量， 也即是 contents 数组的长度。

    // 保存元素的数组
    int8_t contents[];// 保存元素的数组 各个项在数组中按值的大小从小到大有序地排列， 并且数组中不包含任何重复项。

} intset;

intset *intsetNew(void);
intset *intsetAdd(intset *is, int64_t value, uint8_t *success);
intset *intsetRemove(intset *is, int64_t value, int *success);
uint8_t intsetFind(intset *is, int64_t value);
int64_t intsetRandom(intset *is);
uint8_t intsetGet(intset *is, uint32_t pos, int64_t *value);
uint32_t intsetLen(intset *is);
size_t intsetBlobLen(intset *is);

#endif // __INTSET_H
