%include "std_string.i"

%newobject Resource::Serialize();

%{
#include "Resource.h"
%}

%ignore GetAttrInt32(ResourceAttrInfo *ai);
%ignore GetAttrUInt32(ResourceAttrInfo *ai);
%ignore GetAttrInt64(ResourceAttrInfo *ai);
%ignore GetAttrUInt64(ResourceAttrInfo *ai);
%ignore GetAttrString(ResourceAttrInfo *ai);
%ignore GetAttrIpAddr(ResourceAttrInfo *ai);
%ignore GetAttrArrayInt32(ResourceAttrInfo *ai);
%ignore GetAttrArrayUInt32(ResourceAttrInfo *ai);
%ignore GetAttrArrayInt64(ResourceAttrInfo *ai);
%ignore GetAttrArrayUInt64(ResourceAttrInfo *ai);
%ignore GetAttrArrayString(ResourceAttrInfo *ai, int index);
%ignore GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index);
%ignore GetAttrHashInt32(ResourceAttrInfo *ai);
%ignore GetAttrHashUInt32(ResourceAttrInfo *ai);
%ignore GetAttrHashInt64(ResourceAttrInfo *ai);
%ignore GetAttrHashUInt64(ResourceAttrInfo *ai);
%ignore GetAttrHashString(ResourceAttrInfo *ai, const std::string &index);
%ignore GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index);
%ignore SetAttrInt32(ResourceAttrInfo *ai, int32_t value);
%ignore SetAttrUInt32(ResourceAttrInfo *ai, uint32_t value);
%ignore SetAttrInt64(ResourceAttrInfo *ai, int64_t value);
%ignore SetAttrUInt64(ResourceAttrInfo *ai, uint64_t value);
%ignore SetAttrString(ResourceAttrInfo *ai, const std::string &value);
%ignore SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value);
%ignore SetAttrArrayInt32(ResourceAttrInfo *ai, int index, int32_t value);
%ignore SetAttrArrayUInt32(ResourceAttrInfo *ai, int index, uint32_t value);
%ignore SetAttrArrayInt64(ResourceAttrInfo *ai, int index, int64_t value);
%ignore SetAttrArrayUInt64(ResourceAttrInfo *ai, int index, uint64_t value);
%ignore SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value);
%ignore SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value);
%ignore SetAttrHashInt32(ResourceAttrInfo *ai, const std::string &index, int32_t value);
%ignore SetAttrHashUInt32(ResourceAttrInfo *ai, const std::string &index, uint32_t value);
%ignore SetAttrHashInt64(ResourceAttrInfo *ai, const std::string &index, int64_t value);
%ignore SetAttrHashUInt64(ResourceAttrInfo *ai, const std::string &index, uint64_t value);
%ignore SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value);
%ignore SetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index, IpAddr &value);
%ignore Clear(ResourceAttrInfo *ai);
%ignore DeleteHashItem(ResourceAttrInfo *ai, const std::string&);
%ignore GetCount(ResourceAttrInfo *ai);
%ignore GetKeys(ResourceAttrInfo *ai);
%ignore GetValuesInt32(ResourceAttrInfo *ai);
%ignore GetValuesUInt32(ResourceAttrInfo *ai);
%ignore GetValuesInt64(ResourceAttrInfo *ai);
%ignore GetValuesUInt64(ResourceAttrInfo *ai);
%ignore GetValuesString(ResourceAttrInfo *ai);
%ignore GetValuesIpAddr(ResourceAttrInfo *ai);

class ResourceInfo {
public:
        ResourceInfo();
        ~ResourceInfo();
        int GetTypeId();
        void SetTypeId(int typeId);
        const char *GetTypeString();
        void SetTypeString(const char *typeString);
        const char *GetTypeStringTerse();
        void SetTypeStringTerse(const char *typeStringTerse);
        const char *GetObjectName();
        void SetObjectName(const char *typeId);
        std::vector<ResourceAttrInfo*> *GetAttrInfoList();
        void SetAttrInfoList(std::vector<ResourceAttrInfo*> *attrInfoList);
};

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
        virtual bool Serialize(ResourceOutputStream &output) = 0;
        virtual bool Deserialize(ResourceInputStream &input, bool headerOnly) = 0;
        virtual bool Skip(ResourceInputStream &input) = 0;
        virtual int GetSize() = 0;
        virtual ResourceInfo *GetResourceInfo() = 0;
        virtual std::string ToString(Object::LogLevel logLevel) = 0;

        virtual int GetId();
        virtual void SetId(int id);
        virtual int GetStatus();
        virtual void SetStatus(int status);

        virtual int32_t GetAttrInt32(const char *name);
        virtual int32_t GetAttrInt32(ResourceAttrInfo *ai);
        virtual uint32_t GetAttrUInt32(const char *name);
        virtual uint32_t GetAttrUInt32(ResourceAttrInfo *ai);
        virtual int64_t GetAttrInt64(const char *name);
        virtual int64_t GetAttrInt64(ResourceAttrInfo *ai);
        virtual uint64_t GetAttrUInt64(const char *name);
        virtual uint64_t GetAttrUInt64(ResourceAttrInfo *ai);
        virtual const std::string GetAttrString(const char *name);
        virtual const std::string GetAttrString(ResourceAttrInfo *ai);
        virtual const IpAddr GetAttrIpAddr(const char *name);
        virtual const IpAddr GetAttrIpAddr(ResourceAttrInfo *ai);
        virtual int32_t GetAttrArrayInt32(const char *name, int index);
        virtual int32_t GetAttrArrayInt32(ResourceAttrInfo *ai, int index);
        virtual uint32_t GetAttrArrayUInt32(const char *name, int index);
        virtual uint32_t GetAttrArrayUInt32(ResourceAttrInfo *ai, int index);
        virtual int64_t GetAttrArrayInt64(const char *name, int index);
        virtual int64_t GetAttrArrayInt64(ResourceAttrInfo *ai, int index);
        virtual uint64_t GetAttrArrayUInt64(const char *name, int index);
        virtual uint64_t GetAttrArrayUInt64(ResourceAttrInfo *ai, int index);
        virtual const std::string GetAttrArrayString(const char *name, int index);
        virtual const std::string GetAttrArrayString(ResourceAttrInfo *ai, int index);
        virtual const IpAddr GetAttrArrayIpAddr(const char *name, int index);
        virtual const IpAddr GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index);
        virtual int32_t GetAttrHashInt32(const char *name, const std::string &index);
        virtual int32_t GetAttrHashInt32(ResourceAttrInfo *ai, const std::string &index);
        virtual uint32_t GetAttrHashUInt32(const char *name, const std::string &index);
        virtual uint32_t GetAttrHashUInt32(ResourceAttrInfo *ai, const std::string &index);
        virtual int64_t GetAttrHashInt64(const char *name, const std::string &index);
        virtual int64_t GetAttrHashInt64(ResourceAttrInfo *ai, const std::string &index);
        virtual uint64_t GetAttrHashUInt64(const char *name, const std::string &index);
        virtual uint64_t GetAttrHashUInt64(ResourceAttrInfo *ai, const std::string &index);
        virtual const std::string GetAttrHashString(const char *name, const std::string &index);
        virtual const std::string GetAttrHashString(ResourceAttrInfo *ai, const std::string &index);
        virtual const IpAddr GetAttrHashIpAddr(const char *name, const std::string &index);
        virtual const IpAddr GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index);

        virtual void SetAttrInt32(const char *name, int32_t value);
        virtual void SetAttrInt32(ResourceAttrInfo *ai, int32_t value);
        virtual void SetAttrUInt32(const char *name, uint32_t value);
        virtual void SetAttrUInt32(ResourceAttrInfo *ai, uint32_t value);
        virtual void SetAttrInt64(const char *name, int64_t value);
        virtual void SetAttrInt64(ResourceAttrInfo *ai, int64_t value);
        virtual void SetAttrUInt64(const char *name, uint64_t value);
        virtual void SetAttrUInt64(ResourceAttrInfo *ai, uint64_t value);
        virtual void SetAttrString(const char *name, const std::string &value);
        virtual void SetAttrString(ResourceAttrInfo *ai, const std::string &value);
        virtual void SetAttrIpAddr(const char *name, IpAddr &value);
        virtual void SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value);
        virtual void SetAttrArrayInt32(const char *name, int index, int32_t value);
        virtual void SetAttrArrayInt32(ResourceAttrInfo *ai, int index, int32_t value);
        virtual void SetAttrArrayUInt32(const char *name, int index, uint32_t value);
        virtual void SetAttrArrayUInt32(ResourceAttrInfo *ai, int index, uint32_t value);
        virtual void SetAttrArrayInt64(const char *name, int index, int64_t value);
        virtual void SetAttrArrayInt64(ResourceAttrInfo *ai, int index, int64_t value);
        virtual void SetAttrArrayUInt64(const char *name, int index, uint64_t value);
        virtual void SetAttrArrayUInt64(ResourceAttrInfo *ai, int index, uint64_t value);
        virtual void SetAttrArrayString(const char *name, int index, const std::string &value);
        virtual void SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value);
        virtual void SetAttrArrayIpAddr(const char *name, int index, IpAddr &value);
        virtual void SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value);
        virtual void SetAttrHashInt32(const char *name, const std::string &index, int32_t value);
        virtual void SetAttrHashInt32(ResourceAttrInfo *ai, const std::string &index, int32_t value);
        virtual void SetAttrHashUInt32(const char *name, const std::string &index, uint32_t value);
        virtual void SetAttrHashUInt32(ResourceAttrInfo *ai, const std::string &index, uint32_t value);
        virtual void SetAttrHashInt64(const char *name, const std::string &index, int64_t value);
        virtual void SetAttrHashInt64(ResourceAttrInfo *ai, const std::string &index, int64_t value);
        virtual void SetAttrHashUInt64(const char *name, const std::string &index, uint64_t value);
        virtual void SetAttrHashUInt64(ResourceAttrInfo *ai, const std::string &index, uint64_t value);
        virtual void SetAttrHashString(const char *name, const std::string &index, const std::string &value);
        virtual void SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value);
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
        virtual std::vector<int32_t> *GetValuesInt32(ResourceAttrInfo *ai);
        virtual std::vector<int32_t> *GetValuesInt32(const char *name);
        virtual std::vector<uint32_t> *GetValuesUInt32(ResourceAttrInfo *ai);
        virtual std::vector<uint32_t> *GetValuesUInt32(const char *name);
        virtual std::vector<int64_t> *GetValuesInt64(ResourceAttrInfo *ai);
        virtual std::vector<int64_t> *GetValuesInt64(const char *name);
        virtual std::vector<uint64_t> *GetValuesUInt64(ResourceAttrInfo *ai);
        virtual std::vector<uint64_t> *GetValuesUInt64(const char *name);
        virtual std::vector<std::string> *GetValuesString(ResourceAttrInfo *ai);
        virtual std::vector<std::string> *GetValuesString(const char *name);
        virtual std::vector<IpAddr> *GetValuesIpAddr(ResourceAttrInfo *ai);
        virtual std::vector<IpAddr> *GetValuesIpAddr(const char *name);

        void SetFlag(Flag flag);
        void ResetFlag(Flag flag);
        bool IsSetFlag(Flag flag);

        Resource *GetAttachedResource();
        void SetAttachedResource(Resource *attachedResource);
        void ClearAttachedResource();

        int GetTypeId();
        const char *GetTypeString(bool terse = false);
        const char *GetObjectName();
        std::vector<ResourceAttrInfo*> *GetAttrInfoList();
        std::string ToStringShort();

        // static methods common to all Resources
        static void CreateRegistry();
        static void DeleteRegistry();
        static ResourceRegistry *GetRegistry();
        static bool SerializeResource(Resource *resource, ResourceOutputStream &stream, bool saveType = false, bool saveIdStatus = false);
        static Resource *DeserializeResource(ResourceInputStream &stream, int resourceType, int *totalSize);
};
