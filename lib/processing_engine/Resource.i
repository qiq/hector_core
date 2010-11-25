%include "std_string.i"

%newobject Resource::Serialize();

%{
#include "Resource.h"
%}

class Resource {
public:
        Resource();
        virtual ~Resource();
        virtual Resource *Clone() = 0;
        virtual std::string *Serialize() = 0;
        virtual bool Deserialize(const char *data, int size) = 0;
        virtual int getTypeId() = 0;
        virtual const char *getTypeStr() = 0;
        virtual const char *getModuleStr() = 0;
        virtual int getId() = 0;
        virtual void setId(int id) = 0;
        virtual int getStatus() = 0;
        virtual void setStatus(int status) = 0;
        void setStatusDeleted();
        bool isStatusDeleted();
        virtual int getSize() = 0;
        virtual std::string toString(Object::LogLevel logLevel) = 0;
        std::string toStringShort();
        Resource *getAttachedResource();
        void setAttachedResource(Resource *attachedResource);
        void clearAttachedResource();

        // static methods common to all Resources
        static Resource *CreateResource(int id);
        static int NameToId(const char *name);
        static const int RESOURCE_DELETED;
};
