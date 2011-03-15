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
        virtual bool IsProtobufResource();
        virtual std::string *Serialize() = 0;
        virtual bool Deserialize(const char *data, int size) = 0;
        virtual std::vector<ResourceAttrInfo*> *GetAttrInfoList() = 0;
        virtual int GetTypeId() = 0;
        virtual const char *GetTypeString(bool terse = false) = 0;
        virtual int GetSize() = 0;
        virtual std::string ToString(Object::LogLevel logLevel) = 0;
        virtual int GetId();
        virtual void SetId(int id);
        virtual int GetStatus();
        virtual void SetStatus(int status);

        virtual const std::string GetAttrString(const char *name);
        virtual const std::string GetAttrString(ResourceAttrInfo *ai);
        virtual int GetAttrInt(const char *name);
        virtual int GetAttrInt(ResourceAttrInfo *ai);
        virtual long GetAttrLong(const char *name);
        virtual long GetAttrLong(ResourceAttrInfo *ai);
        virtual const IpAddr GetAttrIpAddr(const char *name);
        virtual const IpAddr GetAttrIpAddr(ResourceAttrInfo *ai);
        virtual const std::string GetAttrArrayString(const char *name, int index);
        virtual const std::string GetAttrArrayString(ResourceAttrInfo *ai, int index);
        virtual int GetAttrArrayInt(const char *name, int index);
        virtual int GetAttrArrayInt(ResourceAttrInfo *ai, int index);
        virtual long GetAttrArrayLong(const char *name, int index);
        virtual long GetAttrArrayLong(ResourceAttrInfo *ai, int index);
        virtual const IpAddr GetAttrArrayIpAddr(const char *name, int index);
        virtual const IpAddr GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index);
        virtual const std::string GetAttrHashString(const char *name, const std::string &index);
        virtual const std::string GetAttrHashString(ResourceAttrInfo *ai, const std::string &index);
        virtual int GetAttrHashInt(const char *name, const std::string &index);
        virtual int GetAttrHashInt(ResourceAttrInfo *ai, const std::string &index);
        virtual long GetAttrHashLong(const char *name, const std::string &index);
        virtual long GetAttrHashLong(ResourceAttrInfo *ai, const std::string &index);
        virtual const IpAddr GetAttrHashIpAddr(const char *name, const std::string &index);
        virtual const IpAddr GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index);
        
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

        virtual void Clear(ResourceAttrInfo *ai);
        virtual void Clear(const char *name);
        virtual void DeleteHashItem(ResourceAttrInfo *ai, const std::string&);
        virtual void DeleteHashItem(const char *name, const std::string&);
        virtual int GetCount(ResourceAttrInfo *ai);
        virtual int GetCount(const char *name);
        virtual std::vector<std::string> *GetKeys(ResourceAttrInfo *ai);
        virtual std::vector<std::string> *GetKeys(const char *name);
        virtual std::vector<std::string> *GetValuesString(ResourceAttrInfo *ai);
        virtual std::vector<std::string> *GetValuesString(const char *name);
        virtual std::vector<int> *GetValuesInt(ResourceAttrInfo *ai);
        virtual std::vector<int> *GetValuesInt(const char *name);
        virtual std::vector<long> *GetValuesLong(ResourceAttrInfo *ai);
        virtual std::vector<long> *GetValuesLong(const char *name);
        virtual std::vector<IpAddr> *GetValuesIpAddr(ResourceAttrInfo *ai);
        virtual std::vector<IpAddr> *GetValuesIpAddr(const char *name);

        void SetFlag(Flag flag);
        void ResetFlag(Flag flag);
        bool IsSetFlag(Flag flag);

        Resource *GetAttachedResource();
        void SetAttachedResource(Resource *attachedResource);
        void ClearAttachedResource();

        std::string ToStringShort();

        // static methods common to all Resources
        static bool Serialize(Resource *resource, google::protobuf::io::CodedOutputStream *stream);
        static Resource *Deserialize(google::protobuf::io::CodedInputStream *stream, int *totalSize);
};
