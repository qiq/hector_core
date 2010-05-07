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
        vector<string> *getIds();

        char *getObjectValue(const char *id, const char *name);
        bool setObjectValue(const char *id, const char *name, const char *value);
};
