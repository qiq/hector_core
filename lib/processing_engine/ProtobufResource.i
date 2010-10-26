%include "std_string.i"

%newobject Resource::toString();
%newobject Resource::Serialize();

%{
#include "ProtobufResource.h"
%}

class ProtobufResource {
public:
        ProtobufResource();
        virtual ~ProtobufResource();
        virtual ProtobufResource *Clone() = 0;
        virtual int getTypeId() = 0;
        virtual const char *getTypeStr() = 0;
        virtual const char *getModuleStr() = 0;
        virtual int getId() = 0;
        virtual void setId(int id) = 0;
        virtual int getStatus() = 0;
        virtual void setStatus(int status) = 0;
        virtual std::string *Serialize() = 0;
        virtual bool Deserialize(const char *data, int size) = 0;
        virtual int getSize() = 0;
        virtual std::string *toString(Object::LogLevel logLevel) = 0;
};
