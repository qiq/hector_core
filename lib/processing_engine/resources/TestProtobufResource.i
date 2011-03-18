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
        void Clear();
        std::string *Serialize();
        int GetSerializedSize();
        bool SerializeWithCachedSize(google::protobuf::io::CodedOutputStream *output);
        bool Deserialize(const char *data, int size);
        bool Deserialize(google::protobuf::io::CodedInputStream *input);
        std::vector<ResourceAttrInfo*> *GetAttrInfoList();
        int GetTypeId();
        const char *GetTypeString(bool terse = false);
        const char *GetObjectName();
        int GetSize();
        std::string ToString(Object::LogLevel logLevel);

        int GetId();
        void SetId(int id);
        int GetStatus();
        void SetStatus(int status);

        void SetStr(const std::string &str);
        const std::string GetStr();

        static bool IsTestProtobufResource(Resource *resource);
};
