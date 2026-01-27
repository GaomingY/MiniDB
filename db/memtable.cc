#include "db/memtable.h"
#include "db/dbformat.h"
#include "comparator.h"
#include "env.h"
#include "iterator.h"
#include "util/coding.h"

namespace minidb{
    void MemTable::Add(SequenceNumber s, ValueType type, const Slice& key, const Slice& value){

        size_t key_size = key.size();
        size_t val_size = value.size();
        size_t internal_key_size = key_size + 8;
        //对于每个结点的key和value，MemTable都要分别存储它们各自的真值和长度
        //长度的类型是变长整数
        const size_t encoded_len = VarintLength(internal_key_size)+
                                    internal_key_size + VarintLength(val_size) + 
                                    val_size;
        //为新添加的数据分配内存
        char* buf = arena_.Allocate(encoded+len);
        //将key的长度值编码成一个变长整数并将其填入分配的空间中
        char* p = EncodeVarint32(buf, internal_key_size);
        //将key的实际数据复制到分配的空间中
        std::memcpy(p, key.data(), key_size);
        p += key_size;
        EncodeFixed64(p, (s << 8) | type);
        p += 8;
        //同样的操作对value在处理一遍
        p = EncodeVarint32(p, val_size);
        std::memcpy(p, value.data(), val_size);
        assert(p + val_size == buf + encoded_len);
        //此时用户追加写的内容已经顺利放到了内存中，但没有到跳表中，也没有和其他数据建立联系(有序联系)
        //调用跳表的Insert方法将数据插入到跳表中
        table_.Insert(buf);
    }
}