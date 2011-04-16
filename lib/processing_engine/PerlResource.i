%include "std_string.i"

%newobject PerlResource::Serialize();

%{
#include "PerlResource.h"
%}

class PerlResource : public Resource {
public:
        PerlResource(PerlResourceInterpreter *perl, const char *name);
        ~PerlResource();
        bool Init();
        Resource *Clone();
        void Clear();
        bool Serialize(ResourceOutputStream &output);
        bool Deserialize(ResourceInputStream &input);
        int GetSize();
        ResourceInfo *GetResourceInfo();
        std::string ToString(Object::LogLevel logLevel);

        int32_t GetInt32(const std::string &name);
        uint32_t GetUInt32(const std::string &name);
        int64_t GetInt64(const std::string &name);
        uint64_t GetUInt64(const std::string &name);
        std::string GetString(const std::string &name);
        IpAddr GetIpAddr(const std::string &name);
        int32_t GetArrayInt32(const std::string &name, int index);
        uint32_t GetArrayUInt32(const std::string &name, int index);
        int64_t GetArrayInt64(const std::string &name, int index);
        uint64_t GetArrayUInt64(const std::string &name, int index);
        std::string GetArrayString(const std::string &name, int index);
        IpAddr GetArrayIpAddr(const std::string &name, int index);
        int32_t GetHashInt32(const std::string &name, const std::string &index);
        uint32_t GetHashUInt32(const std::string &name, const std::string &index);
        int64_t GetHashInt64(const std::string &name, const std::string &index);
        uint64_t GetHashUInt64(const std::string &name, const std::string &index);
        std::string GetHashString(const std::string &name, const std::string &index);
        IpAddr GetHashIpAddr(const std::string &name, const std::string &index);

        void SetInt32(const std::string &name, int32_t value);
        void SetUInt32(const std::string &name, uint32_t value);
        void SetInt64(const std::string &name, int64_t value);
        void SetUInt64(const std::string &name, uint64_t value);
        void SetString(const std::string &name, const std::string &value);
        void SetIpAddr(const std::string &name, IpAddr &value);
        void SetArrayInt32(const std::string &name, int index, int32_t value);
        void SetArrayUInt32(const std::string &name, int index, uint32_t value);
        void SetArrayInt64(const std::string &name, int index, int64_t value);
        void SetArrayUInt64(const std::string &name, int index, uint64_t value);
        void SetArrayString(const std::string &name, int index, const std::string &value);
        void SetArrayIpAddr(const std::string &name, int index, IpAddr &value);
        void SetHashInt32(const std::string &name, const std::string &index, int32_t value);
        void SetHashUInt32(const std::string &name, const std::string &index, uint32_t value);
        void SetHashInt64(const std::string &name, const std::string &index, int64_t value);
        void SetHashUInt64(const std::string &name, const std::string &index, uint64_t value);
        void SetHashString(const std::string &name, const std::string &index, const std::string &value);
        void SetHashIpAddr(const std::string &name, const std::string &index, IpAddr &value);

        void Clear(const std::string&);
        int Count(const std::string&);
        void DeleteItem(const std::string&, const std::string&);
        std::vector<std::string> *Keys(const std::string&);
        std::vector<int32_t> *Int32Values(const std::string&);
        std::vector<uint32_t> *UInt32Values(const std::string&);
        std::vector<int64_t> *Int64Values(const std::string&);
        std::vector<uint64_t> *UInt64Values(const std::string&);
        std::vector<std::string> *StringValues(const std::string&);
        std::vector<IpAddr> *IpAddrValues(const std::string&);

        PerlResourceInterpreter *GetPerlResourceInterpreter();
};
