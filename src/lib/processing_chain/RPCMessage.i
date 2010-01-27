%{
#include "RPCMessage.h"
%}

class RPCMessage {
public:
        RPCMessage();
        ~RPCMessage();

        // bool Init(vector<pair<string, string> >* c)
        void set_Init(vector<pair<string, string> >* c);
        const char *get_Init(int index, bool first);
        void set_response_Init(bool result);
        bool get_response_Init();
        // void SaveCheckpoint(const char *path, const char *id)
        void set_SaveCheckpoint(const char *path, const char *id);
        const char *get_SaveCheckpoint(bool first);
        void set_response_SaveCheckpoint();
        // void RestoreCheckpoint(const char *path, const char *id)
        void set_RestoreCheckpoint(const char *path, const char *id);
        const char *get_RestoreCheckpoint(bool first);
        void set_response_RestoreCheckpoint();

        // const char *getValue(const char *name)
        void set_getValue(const char *name);
        const char *get_getValue();
        void set_response_getValue(const char *name);
        const char *get_response_getValue();
        // bool setValue(const char *name, const char *value)
        void set_setValue(const char *name, const char *value);
        const char *get_setValue(bool first);
        void set_response_setValue(bool result);
        bool get_response_setValue();
        // vector<string> *listNames()
        void set_listNames();
        void set_response_listNames(const char *name);
        const char *get_response_listNames(int index);

        string *serialize();
        bool deserialize(string *s);
};
