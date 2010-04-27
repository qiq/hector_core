%{
#include "TestResource.h"
%}

class TestResource : public Resource {
public:
        TestResource();
        ~TestResource();
        Resource *Clone();
        int getId();
        void setId(int id);
        int getStatus();
        void setStatus(int status);
        int getSize();

        void setStr(const char *str);
        const char *getStr();

        string *Serialize();
        bool Deserialize(string *s);
};
