%{
#include "TestProtobufResource.h"
%}

class TestProtobufResource : public ProtobufResource {
public:
        TestResource();
        ~TestResource();
        Resource *Clone();
        int getTypeId();
        const char *getTypeStr();
        int getId();
        void setId(int id);
        int getStatus();
        void setStatus(int status);
        string *Serialize();
        bool Deserialize(string *s);
        int getSerializedSize();
        bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output);
        bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size);
        int getSize();

        void setStr(const char *str);
        const char *getStr();

};
