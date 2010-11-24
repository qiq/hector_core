%include "std_string.i"

%newobject Resource::MessageSerialize();

%{
#include "ProtobufResource.h"
%}

class ProtobufResource : public Resource {
public:
        ProtobufResource();
        virtual ~ProtobufResource();

        virtual int getSerializedSize() = 0;
        virtual bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output) = 0;
        virtual bool SerializeWithCachedSizes(google::protobuf::io::ZeroCopyOutputStream *output) = 0;
        virtual bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size) = 0;
protected:
        // helper methods for (de)serialization
        std::string *MessageSerialize(google::protobuf::Message *msg);
        bool MessageDeserialize(google::protobuf::Message *msg, const char *data, int size);
        bool MessageSerialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output);
        int MessageGetSerializedSize(google::protobuf::Message *msg);
        bool MessageSerializeWithCachedSizes(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output);
        bool MessageDeserialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyInputStream *input, int size);
};
