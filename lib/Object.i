%include "typemaps.i"
%include "std_vector.i"
%include "std_string.i"

%newobject Object::GetValue(const char *name);

%{
#include "Object.h"
%}

class Object {
public:
        Object(ObjectRegistry *objects, const char *id);
        Object(ObjectRegistry *objects, const char *id, int index);
        ~Object();
        ObjectRegistry *GetObjectRegistry();
        void ObjectLockRead();
        void ObjectLockWrite();
        void ObjectUnlock();
        const char *GetId();
        char *GetValue(const char *name);
        bool SetValue(const char *name, const char *value);
        std::vector<std::string> *ListNames();
        void SaveCheckpoint(const char *path);
        void RestoreCheckpoint(const char *path);

        void log_trace(const std::string &s);
        void log_debug(const std::string &s);
        void log_info(const std::string &s);
        void log_error(const std::string &s);
        void log_fatal(const std::string &s);

        enum LogLevel {
                TRACE = 0,
                DEBUG =  1,
                INFO = 2,
                ERROR = 3,
                FATAL =  4,
        };
};
