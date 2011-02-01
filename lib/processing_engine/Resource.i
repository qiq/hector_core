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

        // generic API for getting/setting of attributes
        virtual const std::string &GetAttrString(const char *name);
        virtual const std::string &GetAttrString(ResourceAttrInfo *ai);
        virtual int GetAttrInt(const char *name);
        virtual int GetAttrInt(ResourceAttrInfo *ai);
        virtual long GetAttrLong(const char *name);
        virtual long GetAttrLong(ResourceAttrInfo *ai);
        virtual const IpAddr &GetAttrIpAddr(const char *name);
        virtual const IpAddr &GetAttrIpAddr(ResourceAttrInfo *ai);
        virtual const std::string &GetAttrArrayString(const char *name, int index);
        virtual const std::string &GetAttrArrayString(ResourceAttrInfo *ai, int index);
        virtual int GetAttrArrayInt(const char *name, int index);
        virtual int GetAttrArrayInt(ResourceAttrInfo *ai, int index);
        virtual long GetAttrArrayLong(const char *name, int index);
        virtual long GetAttrArrayLong(ResourceAttrInfo *ai, int index);
        virtual const IpAddr &GetAttrArrayIpAddr(const char *name, int index);
        virtual const IpAddr &GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index);
        virtual const std::string &GetAttrHashString(const char *name, const std::string &index);
        virtual const std::string &GetAttrHashString(ResourceAttrInfo *ai, const std::string &index);
        virtual int GetAttrHashInt(const char *name, const std::string &index);
        virtual int GetAttrHashInt(ResourceAttrInfo *ai, const std::string &index);
        virtual long GetAttrHashLong(const char *name, const std::string &index);
        virtual long GetAttrHashLong(ResourceAttrInfo *ai, const std::string &index);
        virtual const IpAddr &GetAttrHashIpAddr(const char *name, const std::string &index);
        virtual const IpAddr &GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index);

        virtual void SetAttrString(const char *name, const std::string &value);
        virtual void SetAttrString(ResourceAttrInfo *ai, const std::string &value);
        virtual void SetAttrInt(const char *name, int value);
        virtual void SetAttrInt(ResourceAttrInfo *ai, int value);
        virtual void SetAttrLong(const char *name, long value);
        virtual void SetAttrLong(ResourceAttrInfo *ai, long value);
        virtual void SetAttrIpAddr(const char *name, IpAddr &value);
        virtual void SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value);
        virtual void SetAttrArrayString(const char *name, int index, const std::string &value);
        virtual void SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value);
        virtual void SetAttrArrayInt(const char *name, int index, int value);
        virtual void SetAttrArrayInt(ResourceAttrInfo *ai, int index, int value);
        virtual void SetAttrArrayLong(const char *name, int index, long value);
        virtual void SetAttrArrayLong(ResourceAttrInfo *ai, int index, long value);
        virtual void SetAttrArrayIpAddr(const char *name, int index, IpAddr &value);
        virtual void SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value);
        virtual void SetAttrHashString(const char *name, const std::string &index, const std::string &value);
        virtual void SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value);
        virtual void SetAttrHashInt(const char *name, const std::string &index, int value);
        virtual void SetAttrHashInt(ResourceAttrInfo *ai, const std::string &index, int value);
        virtual void SetAttrHashLong(const char *name, const std::string &index, long value);
        virtual void SetAttrHashLong(ResourceAttrInfo *ai, const std::string &index, long value);
        virtual void SetAttrHashIpAddr(const char *name, const std::string &index, IpAddr &value);
        virtual void SetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index, IpAddr &value);
};
