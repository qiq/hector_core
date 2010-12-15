%include "std_string.i"

%newobject TestProtobufResource::Serialize();

%{
#include "TestProtobufResource.h"
%}

class TestProtobufResource : public ProtobufResource {
public:
        TestProtobufResource();
        ~TestProtobufResource();
        Resource *Clone();
        std::string *Serialize();
        int GetSerializedSize();
        bool SerializeWithCachedSize(google::protobuf::io::CodedOutputStream *output);
        bool Deserialize(const char *data, int size);
        bool Deserialize(google::protobuf::io::CodedInputStream *input);
        int getTypeId();
        const char *getTypeStr();
        const char *getModuleStr();
        int getSize();
        std::string toString(Object::LogLevel logLevel);

        void setStr(const std::string &str);
        const std::string &getStr();

};
