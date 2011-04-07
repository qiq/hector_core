%include "std_string.i"

%{
#include "ResourceOutputStream.h"
%}

class ResourceOutputStream {
public:
        ResourceOutputStream(int fd);
        ResourceOutputStream(std::string *s);
        ~ResourceOutputStream();

        // generic methods
        void WriteRaw(const void *buffer, int size);
        void WriteString(const std::string &buffer);
        void WriteLittleEndian32(uint32_t *value);
        void WriteVarint32(uint32_t *value);
        bool Skip(int count);

        // protobuf methods
        google::protobuf::io::CodedOutputStream *GetCodedOutputStream();
};

