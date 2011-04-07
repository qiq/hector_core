%include "std_string.i"

%{
#include "ResourceInputStream.h"
%}

class ResourceInputStream {
public:
        ResourceInputStream(int fd);
        ResourceInputStream(std::string &s);
        ~ResourceInputStream();

        // generic methods
        bool ReadRaw(void *buffer, int size);
        bool ReadString(std::string *buffer, int size);
        bool ReadLittleEndian32(uint32_t *value);
        bool ReadVarint32(uint32_t *value);
        bool Skip(int count);

        // protobuf methods
        google::protobuf::io::CodedInputStream::Limit PushLimit(int byte_limit);
        void PopLimit(google::protobuf::io::CodedInputStream::Limit limit);
        google::protobuf::io::CodedInputStream *GetCodedInputStream();
};
