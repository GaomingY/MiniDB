#ifdef STORAGE_MINIDB_LOG_WRITER_H_
#define STORAGE_MINIDB_LOG_WRITER_H_

#include <cstdint>
#include "db/log_format.h"
#include "slice.h"
#include "status.h"

namespace minidb{
    class WritableFile;

    namespace log{
        class Writer{
            private:
                Status EmitPhysicalRecord(RecordType type, const char* ptr, size_t length);
                WritableFile* dest_;
                int block_offset_;
                uint32_t type_crc[kMaxRecordType + 1];
            public:
                explicit Writer(WritableFile* dest);
                Writer(WritableFile* dest, uint64_t dest_length);

                Writer9(const Writer& ) = delete;
                Writer& operator=(const Writer& ) = delete;

                !Writer();

                Status AddRecord(const Slice& slice);
        };
    }
}

#endif // STORAGE_MINIDB_LOG_WRITER_H_