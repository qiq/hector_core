%{
#include "Resource.h"
%}

class Resource {
public:
        Resource();
        virtual ~Resource();
        virtual int getId() = 0;
        virtual void setId(int id) = 0;
        virtual int getStatus() = 0;
        virtual void setStatus(int status) = 0;
        virtual int getSize() = 0;
        virtual resource_t getType() = 0;

        virtual string *serialize() = 0;
        virtual bool deserialize(string *s) = 0;
};
