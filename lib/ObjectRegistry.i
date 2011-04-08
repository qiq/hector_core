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

        bool RegisterObject(Object *obj);
        bool UnregisterObject(const char *id);
        Object *GetObject(const char *id);
        std::vector<std::string> *GetIds();

        char *GetObjectProperty(const char *id, const char *name);
        bool SetObjectProperty(const char *id, const char *name, const char *value);
};
