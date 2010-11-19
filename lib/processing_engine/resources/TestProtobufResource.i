%include "std_string.i"

%newobject TestProtobufResource::Serialize();
%newobject TestProtobufResource::toString();

%{
#include "TestProtobufResource.h"
%}

class TestProtobufResource : public ProtobufResource {
public:
        TestProtobufResource();
        ~TestProtobufResource();
        Resource *Clone();
        std::string *Serialize();
        bool Deserialize(const char *data, int size);
        int getSerializedSize();
        bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output);
        bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size);
        int getTypeId();
        const char *getTypeStr();
        const char *getModuleStr();
        int getId();
        void setId(int id);
        int getStatus();
        void setStatus(int status);
        int getSize();
        std::string *toString(Object::LogLevel logLevel);

        void setStr(const std::string &str);
        const std::string &getStr();

};
