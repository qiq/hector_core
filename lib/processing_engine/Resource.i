%include "std_string.i"

%newobject Resource::Serialize();

%{
#include "Resource.h"
%}

class Resource {
public:
        enum Flag {
                DELETED = 1,
                SKIP = 2,
        };
        
        Resource();
        virtual ~Resource();
        virtual Resource *Clone() = 0;
        virtual void Clear();
        virtual bool isProtobufResource();
        virtual std::string *Serialize() = 0;
        virtual bool Deserialize(const char *data, int size) = 0;
        virtual int getTypeId() = 0;
        virtual const char *getTypeStr() = 0;
        virtual const char *getModuleStr() = 0;
        virtual int getId() = 0;
        virtual void setId(int id) = 0;
        virtual int getStatus() = 0;
        virtual void setStatus(int status) = 0;
        void setFlag(Flag flag);
        void resetFlag(Flag flag);
        bool isSetFlag(Flag flag);
        virtual int getSize() = 0;
        virtual std::string toString(Object::LogLevel logLevel) = 0;
        std::string toStringShort();
        Resource *getAttachedResource();
        void setAttachedResource(Resource *attachedResource);
        void clearAttachedResource();

        // static methods common to all Resources
        static int NextResourceId();
        static bool Serialize(Resource *resource, google::protobuf::io::CodedOutputStream *stream);
        static Resource *Deserialize(google::protobuf::io::CodedInputStream *stream, int *totalSize);
};
