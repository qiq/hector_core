/**
 * External simple
 */

#include "common.h"
#include "RPC.h"

log4cxx::LoggerPtr RPC::logger(log4cxx::Logger::getLogger("lib.processing_chain.RPC"));

RPC::RPC(Connection *connection) {
	this->connection = connection;
	messageId = 0;
}

RPC::~RPC() {
	delete connection;
}

bool RPC::_Send(int type, int id, string *serialIn, bool lock) {
	message_header_t header;
	if (lock)
		read_lock.lock();
	header.type = type;
	header.id = id;
	header.length = serialIn->length();
	if (connection->ReadWrite((char *)&header, sizeof(header), NULL, 0) != sizeof(header)) {
		if (lock)
			read_lock.unlock();
		LOG4CXX_ERROR(logger, "Cannot write header");
		return false;
	}
	if (connection->ReadWrite((char *)serialIn->c_str(), serialIn->length(), NULL, 0) != serialIn->length()) {
		if (lock)
			read_lock.unlock();
		LOG4CXX_ERROR(logger, "Cannot write data");
		return false;
	}
	if (lock)
		read_lock.unlock();
	return true;
}

bool RPC::_Send(int type, int *id, string *serialIn, bool lock) {
	message_header_t header;
	read_lock.lock();
	header.type = type;
	header.id = messageId;
	header.length = serialIn->length();
	if (connection->ReadWrite((char *)&header, sizeof(header), NULL, 0) != sizeof(header)) {
		if (lock)
			read_lock.unlock();
		LOG4CXX_ERROR(logger, "Cannot write header");
		return false;
	}
	if (connection->ReadWrite((char *)serialIn->c_str(), serialIn->length(), NULL, 0) != serialIn->length()) {
		if (lock)
			read_lock.unlock();
		LOG4CXX_ERROR(logger, "Cannot write data");
		return false;
	}
	*id = messageId++;
	if (lock)
		read_lock.unlock();
	return true;
}

string *RPC::_Receive(int *type, int *id, bool lock) {
	message_header_t header;
	if (lock)
		write_lock.lock();
	if (connection->ReadWrite(NULL, 0, (char *)&header, sizeof(header)) != sizeof(header)) {
		if (lock)
			write_lock.unlock();
		LOG4CXX_ERROR(logger, "Cannot read header");
		return NULL;
	}
	char *sin = new char[header.length];
	if (connection->ReadWrite(NULL, 0, sin, header.length) != (int)header.length) {
		if (lock)
			write_lock.unlock();
		delete sin;
		LOG4CXX_ERROR(logger, "Cannot read resource");
		return NULL;
	}
	if (lock)
		write_lock.unlock();

	string *serialOut = new string(sin, header.length);
	delete sin;
	*type = header.type;
	*id = header.id;
	return serialOut;
}

string *RPC::SendReceive(int *type, string *serialIn) {
	read_lock.lock();
	write_lock.lock();
	int id;
	if (!_Send(*type, &id, serialIn, false)) {
		write_lock.unlock();
		read_lock.unlock();
		return NULL;
	}
	int idOut;
	string *serialOut = _Receive(type, &idOut, false);
	write_lock.unlock();
	read_lock.unlock();
	if (!serialOut)
		return NULL;
	if (id != idOut) {
		LOG4CXX_ERROR(logger, "Invalid message id");
		delete serialOut;
		return NULL;
	}
	return serialOut;
}

//bool RPC::SendMessage(RPCMessage *message, int id) {
//	string *serialIn = message->serialize();
//	int type = 0;
//	return Send(0, id, serialIn);
//}

bool RPC::SendReceiveMessage(RPCMessage *message) {
	string *serialIn = message->serialize();
	int type = 0;
	string *serialOut = SendReceive(&type, serialIn);
	if (!serialOut || type != 0) {
		delete serialIn;
		return false;
	}
	delete serialIn;
	if (!message->deserialize(serialOut)) {
		delete serialOut;
		return false;
	}
	delete serialOut;
	return true;
}
