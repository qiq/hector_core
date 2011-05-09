%include "std_string.i"

%{
#include "ResourceInputStream.h"
%}

class ResourceInputStream {
public:
        ResourceInputStream();
        ~ResourceInputStream();

        virtual bool ReadString(std::string *buffer) = 0;
        virtual bool ReadLittleEndian32(uint32_t *value) = 0;
        virtual bool ReadVarint32(uint32_t *value) = 0;
        virtual bool ReadLittleEndian64(uint64_t *value) = 0;
        virtual bool ReadVarint64(uint64_t *value) = 0;
        virtual bool Skip(int count) = 0;
        virtual int ByteCount() = 0;

        virtual bool ParseMessage(google::protobuf::Message &msg, int size = 0) = 0;
};
