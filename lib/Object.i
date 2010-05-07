%{
#include "Object.h"
%}


class Object {
public:
        Object(ObjectRegistry *objects, const char *id);
        Object(ObjectRegistry *objects, const char *id, int index);
        ~Object();
        ObjectRegistry *getObjectRegistry();
        void ObjectLockRead();
        void ObjectLockWrite();
        void ObjectUnlock();
        const char *getId();
        char *getValue(const char *name);
        bool setValue(const char *name, const char *value);
        vector<string> *listNames();
        void SaveCheckpoint(const char *path, const char *id);
        void RestoreCheckpoint(const char *path, const char *id);

        void log_trace(const char *s);
        void log_debug(const char *s);
        void log_info(const char *s);
        void log_error(const char *s);
        void log_fatal(const char *s);
};
