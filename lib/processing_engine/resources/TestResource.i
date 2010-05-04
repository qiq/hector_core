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

        string *Serialize();
        bool Deserialize(string *s);
        int getSize();

        void setStr(const char *str);
        const char *getStr();
};
