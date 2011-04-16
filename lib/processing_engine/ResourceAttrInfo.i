
%{
#include "ResourceAttrInfo.h"
%}

class ResourceAttrInfo {
public:
        enum FieldType {
                UNKNOWN,
                STRING,
                INT32,
                UINT32,
                INT64,
                UINT64,
                DOUBLE,
                IP,
                ARRAY_STRING,
                ARRAY_INT32,
                ARRAY_UINT32,
                ARRAY_INT64,
                ARRAY_UINT64,
                ARRAY_DOUBLE,
                ARRAY_IP,
                HASH_STRING,
                HASH_INT32,
                HASH_UINT32,
                HASH_INT64,
                HASH_UINT64,
                HASH_DOUBLE,
                HASH_IP,
        };

        ResourceAttrInfo(int typeId_): type(UNKNOWN), typeId(typeId_) {};
        virtual ~ResourceAttrInfo() {};
        FieldType GetType();
        void SetType(FieldType type);
        int GetTypeId();
        void SetTypeId(int id);
        std::string &GetName();
        void SetName(std::string &name);

        virtual const std::string GetString(Resource*) = 0;
        virtual int32_t GetInt32(Resource*) = 0;
        virtual uint32_t GetUInt32(Resource*) = 0;
        virtual int64_t GetInt64(Resource*) = 0;
        virtual uint64_t GetUInt64(Resource*) = 0;
        virtual double GetDouble(Resource*) = 0;
        virtual IpAddr GetIpAddr(Resource*) = 0;
        virtual const std::string GetArrayString(Resource*, int) = 0;
        virtual int32_t GetArrayInt32(Resource*, int) = 0;
        virtual uint32_t GetArrayUInt32(Resource*, int) = 0;
        virtual int64_t GetArrayInt64(Resource*, int) = 0;
        virtual uint64_t GetArrayUInt64(Resource*, int) = 0;
        virtual double GetArrayDouble(Resource*, int) = 0;
        virtual IpAddr GetArrayIpAddr(Resource*, int) = 0;
        virtual const std::string GetHashString(Resource*, const std::string&) = 0;
        virtual int32_t GetHashInt32(Resource*, const std::string&) = 0;
        virtual uint32_t GetHashUInt32(Resource*, const std::string&) = 0;
        virtual int64_t GetHashInt64(Resource*, const std::string&) = 0;
        virtual uint64_t GetHashUInt64(Resource*, const std::string&) = 0;
        virtual double GetHashDouble(Resource*, const std::string&) = 0;
        virtual IpAddr GetHashIpAddr(Resource*, const std::string&) = 0;

        virtual void SetString(Resource*, const std::string&) = 0;
        virtual void SetInt32(Resource*, int32_t) = 0;
        virtual void SetUInt32(Resource*, uint32_t) = 0;
        virtual void SetInt64(Resource*, int64_t) = 0;
        virtual void SetUInt64(Resource*, uint64_t) = 0;
        virtual void SetDouble(Resource*, double) = 0;
        virtual void SetIpAddr(Resource*, IpAddr&) = 0;
        virtual void SetArrayString(Resource*, int, const std::string&) = 0;
        virtual void SetArrayInt32(Resource*, int, int32_t) = 0;
        virtual void SetArrayUInt32(Resource*, int, uint32_t) = 0;
        virtual void SetArrayInt64(Resource*, int, int64_t) = 0;
        virtual void SetArrayUInt64(Resource*, int, uint64_t) = 0;
        virtual void SetArrayDouble(Resource*, int, double) = 0;
        virtual void SetArrayIpAddr(Resource*, int, IpAddr&) = 0;
        virtual void SetHashString(Resource*, const std::string&, const std::string&) = 0;
        virtual void SetHashInt32(Resource*, const std::string&, int32_t) = 0;
        virtual void SetHashUInt32(Resource*, const std::string&, uint32_t) = 0;
        virtual void SetHashInt64(Resource*, const std::string&, int64_t) = 0;
        virtual void SetHashUInt64(Resource*, const std::string&, uint64_t) = 0;
        virtual void SetHashDouble(Resource*, const std::string&, double) = 0;
        virtual void SetHashIpAddr(Resource*, const std::string&, IpAddr&) = 0;

        virtual void Clear(Resource*) = 0;
        virtual void DeleteHashItem(Resource*, const std::string&) = 0;

        virtual int GetCount(Resource*) = 0;
        virtual std::vector<std::string> *GetKeys(Resource*) = 0;
        virtual std::vector<std::string> *GetValuesString(Resource*) = 0;
        virtual std::vector<int32_t> *GetValuesInt32(Resource*) = 0;
        virtual std::vector<uint32_t> *GetValuesUInt32(Resource*) = 0;
        virtual std::vector<int64_t> *GetValuesInt64(Resource*) = 0;
        virtual std::vector<uint64_t> *GetValuesUInt64(Resource*) = 0;
        virtual std::vector<double> *GetValuesDouble(Resource*) = 0;
        virtual std::vector<IpAddr> *GetValuesIpAddr(Resource*) = 0;
};
