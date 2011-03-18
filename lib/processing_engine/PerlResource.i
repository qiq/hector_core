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
        std::string *Serialize();
        bool Deserialize(const char *data, int size);
        std::vector<ResourceAttrInfo*> *GetAttrInfoList();
        int GetTypeId();
        const char *GetTypeString(bool terse = false);
        const char *GetObjectName();
        int GetSize();
        std::string ToString(Object::LogLevel logLevel);

        std::string GetString(const std::string &name);
        int GetInt(const std::string &name);
        long GetLong(const std::string &name);
        IpAddr GetIpAddr(const std::string &name);
        std::string GetArrayString(const std::string &name, int index);
        int GetArrayInt(const std::string &name, int index);
        long GetArrayLong(const std::string &name, int index);
        IpAddr GetArrayIpAddr(const std::string &name, int index);
        std::string GetHashString(const std::string &name, const std::string &index);
        int GetHashInt(const std::string &name, const std::string &index);
        long GetHashLong(const std::string &name, const std::string &index);
        IpAddr GetHashIpAddr(const std::string &name, const std::string &index);

        void SetString(const std::string &name, const std::string &value);
        void SetInt(const std::string &name, int value);
        void SetLong(const std::string &name, long value);
        void SetIpAddr(const std::string &name, IpAddr &value);
        void SetArrayString(const std::string &name, int index, const std::string &value);
        void SetArrayInt(const std::string &name, int index, int value);
        void SetArrayLong(const std::string &name, int index, long value);
        void SetArrayIpAddr(const std::string &name, int index, IpAddr &value);
        void SetHashString(const std::string &name, const std::string &index, const std::string &value);
        void SetHashInt(const std::string &name, const std::string &index, int value);
        void SetHashLong(const std::string &name, const std::string &index, long value);
        void SetHashIpAddr(const std::string &name, const std::string &index, IpAddr &value);

        void Clear(const std::string&);
        int Count(const std::string&);
        void DeleteItem(const std::string&, const std::string&);
        std::vector<std::string> *Keys(const std::string&);
        std::vector<std::string> *StringValues(const std::string&);
        std::vector<int> *IntValues(const std::string&);
        std::vector<long> *LongValues(const std::string&);
        std::vector<IpAddr> *IpAddrValues(const std::string&);

        PerlResourceInterpreter *GetPerlResourceInterpreter();
};
