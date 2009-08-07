/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <config.h>

#include <stdint.h>
#include <string>
#include <vector>
#include <log4cxx/logger.h>

using namespace std;

class Resource {
protected:
	uint32_t status;

	static log4cxx::LoggerPtr logger;
public:
	Resource() {};
	~Resource() {};
	int getStatus();
	void setStatus(int status);
	virtual int getSize() = 0;
	virtual string *serialize(bool serializeContent) = 0;
	virtual bool deserialize(string *s) = 0;
};

inline int Resource::getStatus() {
	return status;
}

inline void Resource::setStatus(int status) {
	this->status = status;
}

#endif
