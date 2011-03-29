%include "std_string.i"

%newobject Resource::MessageSerialize();

%{
#include "ProtobufResource.h"
%}

class ProtobufResource : public Resource {
public:
        ProtobufResource();
        virtual ~ProtobufResource();
        bool IsProtobufResource();

        virtual int GetSerializedSize() = 0;
        virtual bool SerializeWithCachedSize(google::protobuf::io::CodedOutputStream *output) = 0;
        virtual bool Deserialize(google::protobuf::io::CodedInputStream *input) = 0;
};
