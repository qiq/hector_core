%newobject TestResource::Serialize();
%newobject TestResource::toString();

%{
#include "TestResource.h"
%}

class TestResource : public Resource {
public:
        TestResource();
        ~TestResource();
        Resource *Clone();
        int getTypeId();
        const char *getTypeStr();
        int getId();
        void setId(int id);
        int getStatus();
        void setStatus(int status);

        std::string *Serialize();
        bool Deserialize(std::string *s);
        int getSize();
        char *toString();

        void setStr(const char *str);
        const char *getStr();
};
