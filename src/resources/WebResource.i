%{
#include "WebResource.h"
%}

class WebResource : public Resource {
public:
        WebResource();
        ~WebResource();
        int getStatus();
        void setStatus(int status);
        int getId();
        void setId(int id);
        int getSize();

        void setURL(const char *url);
        string *getURL();

        string *serialize();
        bool deserialize(string *s);
};
