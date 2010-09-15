%include "typemaps.i"
%include "std_vector.i"
%include "std_string.i"

%newobject ObjectRegistry::getObjectValue(const char *id, const char *name);

%{
#include "ObjectRegistry.h"
%}

class ObjectRegistry {
public:
        ObjectRegistry() {};
        ~ObjectRegistry() {};

        bool registerObject(Object *obj);
        bool unregisterObject(const char *id);
        Object *getObject(const char *id);
        std::vector<std::string> *getIds();

        char *getObjectValue(const char *id, const char *name);
        bool setObjectValue(const char *id, const char *name, const char *value);
};
