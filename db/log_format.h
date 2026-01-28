#ifdef STORAGE_MINIDB_LOG_FORMAT_H_
#define STORAGE_MINIDB_LOG_FORMAT_H_

namespace minidb{
    namespace log{
        //定义了每个块的类型
        enum RecordType{
            kZeroType = 0;      //块剩余的空间非常小
            kFullType = 1;      //日志记录比较小(<32KB)，可以用一个块保存
            kFirstType = 2;     //一条日志的第一块
            kMiddleType = 3;    //一条日志的中间块
            kLastType = 4;      //一条日志的最后一块
        };
        static const int kMaxRecordType = kLastType;
        //日志文件需要分块，每块的大小为32768Byte，也就是32KB
        static const int kBlockSize = 32768;
        //日志元数据长度，包含4B的校验和，2B的长度，1B的类型
        static const int kHeaderSize = 4 + 2 + 1;
    }
}

#endif // STORAGE_MINIDB_LOG_FORMAT_H_