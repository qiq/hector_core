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
        int getTypeId();
        const char *getTypeStr();
        const char *getModuleStr();
        int getId();
        void setId(int id);
        int getStatus();
        void setStatus(int status);
        std::string *Serialize();
        bool Deserialize(std::string *s);
        int getSerializedSize();
        bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output);
        bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size);
        int getSize();
        std::string *toString(Object::LogLevel logLevel);

        void setStr(const std::string &str);
        const std::string &getStr();

};
