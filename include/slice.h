#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include "export.h"
#include <string>
#include <cstring>
#include <cstddef>
#include <cassert>


namespace minidb{
    class MINIDB_EXPORT Slice{
        private:
            const char* data_;
            size_t size_;
        public:
            // 定义构造函数
            Slice() : data_(""), size_(0) {}
            Slice(const char* d, size_t n) : data_(d), size_(n) {}
            Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}
            Slice(const char* s) : data_(s), size_(strlen(s)) {}
            
            Slicer(const Slice&) = default;
            Slice& operator=(const Slice&) = default;

            // 定义geter
            const char* data() const { return data_;}
            size_t size() const { return size_;}

            // 定义迭代器
            const char* begin() const { return data();}
            const char* end() const { return data() + size();}

            bool empty() const {return size_ == 0;}
            
            const char* operator[](size_t n) const{
                assert(n < size());
                return data_[n];
            }

            void clear(){
                data_ = "";
                size_ = 0;
            }
            // 把这个数据片的前n个字节删除
            void remove_prefix(size_t n){
                assert(n <= size());
                data_ += n;
                size_ += n;
            }
            std::string ToString() const { return std::string(data_, size_);}

            int compare(const Slice& b) const;
            //判断这个数据片是否以x开头
            bool starts_with(const Slice& x) const{
                return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
            }
    };//end of Slice
    
    inline bool operator == (const Slice& x, const Slice& y){
        return ((x.size() == y.size()) &&
                memcmp(x.data(), y.data(), x.size()) == 0);
    }

    inline bool operator != (const Slice& x, const Slice& y){ return !(x == y); }

    inline int Slice::compare(const Slice& b) const{
        const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
        int r = memcmp(data_, b.data_, min_len);
        if(r == 0){
            if(size_ < b.size_)
                r = -1;
            else if(size_ > b.size_)
                r = +1;
        }
        return r;
    }
}//namespace minidb

#endif