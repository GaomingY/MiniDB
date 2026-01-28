#include "db/log_writer.h"

#include <cstdint>

#include "util/coding.h"
#include "util/crc32c.h"
#include "env.h"

namespace minidb{
    namespace log{
        static void InitTypeCrc(uint32_t* type_crc){
            for(int i = 0 ; i <= kMaxRecordType ; i++){
                char t = static_cast<char>(i);
                type_crc[i] = crc32c::Value(&t, 1);
            }
        }

        Writer::Writer(WritableFile* dest) : dest_(dest), block_offset_(0){
            InitTypeCrc(type_crc);
        }

        Writer::Writer(WritableFile* dest, uint64_t dest_length) 
            : dest_(dest), block_offset_(dest_length % kBlockSize){
            InitTypeCrc(type_crc);
        }

        Writer::~Writer() = default;

        Status Writer::AddRecord(const Slice& slice){
            const char* ptr = slice.data();
            size_t left = slice.size();

            Status s;
            bool begin = true;
            do{
                const int leftover = kBlockSize - block_offset_;
                assert(leftover >= 0);
                //如果这个日志块的剩余大小小于元数据长度，也就是7B，那么说明它的剩余空间太小了，不再能够使用，于是就把剩余所有位置0
                if(leftover < kHeaderSize){
                    if(leftover > 0){
                        static_assert(kHeaderSize == 7, "");
                        dest_->Append(Slice("\x00\x00\x00\x00\x00\x00", leftover));
                    }
                    block_offset_ = 0;
                }

                assert(kBlockSize - block_offset_ - kHeaderSize >= 0);

                const size_t avail = kBlockSize - block_offset_ - kHeaderSize;
                const size_t fragment_lenngth = (left < avail) ? left : avail;

                RecordType type;

                const bool end = (left == fragment_lenngth);
                if (begin && end) {
                    type = kFullType;
                  } else if (begin) {
                    type = kFirstType;
                  } else if (end) {
                    type = kLastType;
                  } else {
                    type = kMiddleType;
                  }
              
                  s = EmitPhysicalRecord(type, ptr, fragment_length);
                  ptr += fragment_length;
                  left -= fragment_length;
                  begin = false;
            } while(s.ok() && left > 0);
            return s;
        }

        Status Writer::EmitPhysicalRecord(RecordType t, const char* ptr,
            size_t length) {
            assert(length <= 0xffff);  // 长度必须小于64KB
            assert(block_offset_ + kHeaderSize + length <= kBlockSize);

        // Format the header
        char buf[kHeaderSize];
        buf[4] = static_cast<char>(length & 0xff);
        buf[5] = static_cast<char>(length >> 8);
        buf[6] = static_cast<char>(t);

        // Compute the crc of the record type and the payload.
            uint32_t crc = crc32c::Extend(type_crc[t], ptr, length);
        crc = crc32c::Mask(crc);  // Adjust for storage
        EncodeFixed32(buf, crc);

        // Write the header and the payload
        Status s = dest_->Append(Slice(buf, kHeaderSize));
        if (s.ok()) {
            s = dest_->Append(Slice(ptr, length));
            if (s.ok()) {
            s = dest_->Flush();
            }
        }
        block_offset_ += kHeaderSize + length;
        return s;
        }
    }
}