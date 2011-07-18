%include "std_string.i"

%{
#include "ResourceOutputStream.h"
%}

class ResourceOutputStream {
public:
        ResourceOutputStream();
        ~ResourceOutputStream();

        virtual void WriteString(const std::string &buffer) = 0;
        virtual void WriteLittleEndian32(uint32_t *value) = 0;
        virtual void WriteVarint32(uint32_t *value) = 0;
        virtual void WriteLittleEndian64(uint64_t value) = 0;
        virtual void WriteVarint64(uint64_t value) = 0;
        virtual void WriteRaw(const char *data, int size) = 0;
        virtual bool Skip(int count) = 0;

        virtual void SerializeMessage(google::protobuf::Message &msg, bool saveSize = true) = 0;
};

