
#include "RPCMessage.h"

using namespace std;

log4cxx::LoggerPtr RPCMessage::logger(log4cxx::Logger::getLogger("lib.processing_chain.RPCMessage"));

RPCMessage::RPCMessage() {
	// check library vs header file versions
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

string *RPCMessage::serialize() {
	string *result = new string();
	if (!m.SerializeToString(result)) {
		LOG4CXX_ERROR(logger, "Cannot serialize Resource");
		delete result;
		return NULL;
	}
	return result;
}

bool RPCMessage::deserialize(string *s) {
	m.ParseFromString(*s);
	return true;
}
