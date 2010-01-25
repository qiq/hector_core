/** Class representing one message in RPC call
 */


#ifndef _RPCMESSAGE_H_
#define _RPCMESSAGE_H_

#include <config.h>

#include <assert.h>
#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "RPCMessage.pb.h"

using namespace std;

// used for non-C++ and remote modules
typedef struct {
	uint8_t type;
	uint32_t id;
	uint32_t length;
} message_header_t;

class RPCMessage {
protected:
	hector::resources::RPCMessage m;

	static log4cxx::LoggerPtr logger;
public:
	RPCMessage();
	~RPCMessage() {};

	hector::resources::RPCMessage getMethod();

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

// bool Init(vector<pair<string, string> >* c)

inline void RPCMessage::set_Init(vector<pair<string, string> >* c) {
	m.set_method(hector::resources::RPCMessage::INIT);
	for (vector<pair<string, string> >::iterator iter = c->begin(); iter != c->end(); ++iter) {
		m.add_str_args(iter->first);
		m.add_str_args(iter->second);
	}
}

inline const char *RPCMessage::get_Init(int index, bool first) {
	if (index*2+1 >= m.str_args_size())
		return NULL;
	return m.str_args(index * 2 + first ? 1 : 0).c_str();
}

inline void RPCMessage::set_response_Init(bool result) {
	m.set_method(hector::resources::RPCMessage::INIT);
	m.add_bool_args(result);
}

inline bool RPCMessage::get_response_Init() {
	assert(bool_args_size() == 1);
	return m.bool_args(0);
}

// void SaveCheckpoint(const char *path, const char *id)

inline void RPCMessage::set_SaveCheckpoint(const char *path, const char *id) {
	m.set_method(hector::resources::RPCMessage::SAVE_CHECKPOINT);
	m.add_str_args(path);
	m.add_str_args(id);
}

inline const char *RPCMessage::get_SaveCheckpoint(bool first) {
	assert(m.str_args.size() == 2);
	return m.str_args(first ? 0 : 1).c_str();
}

inline void RPCMessage::set_response_SaveCheckpoint() {
	m.set_method(hector::resources::RPCMessage::SAVE_CHECKPOINT);
}

// void RestoreCheckpoint(const char *path, const char *id)

inline void RPCMessage::set_RestoreCheckpoint(const char *path, const char *id) {
	m.set_method(hector::resources::RPCMessage::RESTORE_CHECKPOINT);
	m.add_str_args(path);
	m.add_str_args(id);
}

inline const char *RPCMessage::get_RestoreCheckpoint(bool first) {
	assert(m.str_args.size() == 2);
	return m.str_args(first ? 0 : 1).c_str();
}

inline void RPCMessage::set_response_RestoreCheckpoint() {
	m.set_method(hector::resources::RPCMessage::RESTORE_CHECKPOINT);
}

// const char *getValue(const char *name)

inline void RPCMessage::set_getValue(const char *name) {
	m.set_method(hector::resources::RPCMessage::GET_VALUE);
	m.add_str_args(name);
}

inline const char *RPCMessage::get_getValue() {
	assert(m.str_args.size() == 1);
	return m.str_args(0).c_str();
}

inline void RPCMessage::set_response_getValue(const char *name) {
	m.set_method(hector::resources::RPCMessage::GET_VALUE);
	m.add_str_args(name);
}

inline const char *RPCMessage::get_response_getValue() {
	assert(m.str_args.size() == 1);
	return m.str_args(0).c_str();
}

// bool setValue(const char *name, const char *value)

inline void RPCMessage::set_setValue(const char *name, const char *value) {
	m.set_method(hector::resources::RPCMessage::SET_VALUE);
	m.add_str_args(name);
	m.add_str_args(value);
}

inline const char *RPCMessage::get_setValue(bool first) {
	assert(m.str_args_size() == 2);
	return m.str_args(first ? 0 : 1).c_str();
}

inline void RPCMessage::set_response_setValue(bool result) {
	m.set_method(hector::resources::RPCMessage::SET_VALUE);
	m.add_bool_args(result);
}

inline bool RPCMessage::get_response_setValue() {
	assert(m.bool_args_size() == 1);
	return m.bool_args(0);
}

// vector<string> *listNames()

inline void RPCMessage::set_listNames() {
	m.set_method(hector::resources::RPCMessage::LIST_NAMES);
}

inline void RPCMessage::set_response_listNames(const char *name) {
	m.set_method(hector::resources::RPCMessage::LIST_NAMES);
	m.add_str_args(name);
}

inline const char *RPCMessage::get_response_listNames(int index) {
	if (index >= m.str_args_size())
		return NULL;
	return m.str_args(index).c_str();
}

#endif
