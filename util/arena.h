#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace minidb{
    class Arena{
        public:
            Arena();
            Arena(const Arena& ) = delete;
            Arena& operator=(const Arena& ) = delete;
            ~Arena();

            char* Allocate(size_t bytes);
            char* AllocateAligned(size_t bytes);

            size_t MemoryUsage() const{
                return memory_usage_.load(std::memory_order_relaxed);
            }
        private:
            char* AllocateFallback(size_t bytes);
            char* AllocateNewblock(size_t block_bytes);

            char* alloc_ptr_;   //分配到哪里了
            size_t alloc_bytes_remaining_;  //专用于小对象分配中，记录一个4KB的内存块中剩余可分配的空间
            std::vector<char*> blocks_;     //都分配了哪些块，记录了每个块的起始地址
            std::atomic<size_t> memory_usage_;       //内存使用量
    };
    inline char* Aerna::Allocate(size_t bytes){
        assert(bytes > 0);
        //当前内存页面内剩余空间足以放下新申请的对象
        if(bytes <= alloc_bytes_remaining_){
            char* result = alloc_ptr_;
            alloc_ptr_ += bytes;
            alloc_bytes_remaining_ -= bytes;
            return result;
        }
        return AllocateFallback(bytes);
    }
}
#endif