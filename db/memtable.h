#ifdef STORAGE_MINIDB_DB_MEMTABLE_H_
#define STORAGE_MINIDB_DB_MEMTABLE_H_

#include <string>

#include "db/skiplist.h"
#include "db/dbformat.h"
#include "db.h"
#include "util/arena.h"

namespace minidb{
    class InternalKeyComparator;
    class MemTableIterator;

    class MemTable{
        private:
            friend class MemTableIterator;
            friend class MemTableBackwardIterator;

            struct KeyComparator{
                const InternalKeyComparator comparator;
                explicit KeyComparator(const InternalKeyComparator& c) : comparator(c) {}
                int operator()(const char* a, const char* b) const;
            };

            typedef SkipList<const char*, KeyComparator> Table;

            ~MemTable();

            //键比较器
            KeyComparator comparator_;
            int refs_;
            //内存分配器
            Arena arena_;
            //基于skiplist实现的表结构，用于真正存储MemTable中数据
            Table table_;
        public:
            explicit MemTable(const InternalKeyComparator& comparator);

            MemTable(const MemTable&) = delete;
            MemTable& operator=(const MemTable&) = delete;

            void Ref() { ++refs;}

            //记录引用次数，当引用次数为0时删除MemTable对象，说明容量已满，成为readonly MemTable，准备落盘
            void Unref(){
                --refs;
                assert(refs >= 0);
                if(refs <= 0){
                    delete this;
                }
            }

            size_t ApproximateMemoryUsage();

            Iterator* NewIterator();

            //MemTable只实现读和写接口
            void Add(SequenceNumber seq, ValueType type, const Slice& key,
                        const Slice& value);

            bool get(const lookupKey& key, std::string* value, Status* s);
    };
}

#endif