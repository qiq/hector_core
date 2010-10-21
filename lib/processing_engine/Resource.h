/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "Object.h"

using namespace std;

class Resource {
public:
	Resource() {};
	virtual ~Resource() {};
	// create copy of a resource
	virtual Resource *Clone() = 0;
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	virtual int getTypeId() = 0;
	// type string of a resource
	virtual const char *getTypeStr() = 0;
	// module prefix (e.g. Hector for Hector::TestResource)
	virtual const char *getModuleStr() = 0;
	// id should be unique across all resources
	virtual int getId() = 0;
	virtual void setId(int id) = 0;
	// status may be tested in Processor to select target queue
	virtual int getStatus() = 0;
	virtual void setStatus(int status) = 0;

	// save and restore resource
	virtual string *Serialize() = 0;
	virtual bool Deserialize(string *s) = 0;
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int getSize() = 0;
	// return string representation of the resource (e.g. for debugging purposes)
	virtual char *toString(Object::LogLevel = Object::INFO) = 0;

protected:
	static log4cxx::LoggerPtr logger;
};

class ResourceInfo {
public:
	typedef enum {
		UNKNOWN,
		STRING,
		INT,
		LONG,
		IP4,		// IPv4 address
		IP6,		// IPv6 address
		STRING2,	// string->string
		STRINGN,	// [string]
	} FieldType;

	ResourceInfo() {};
	~ResourceInfo() {};

	FieldType getType();

	virtual const char *getString(Resource*);
	virtual int getInt(Resource*);
	virtual long getLong(Resource*);
	virtual ip4_addr_t getIp4Addr(Resource*);
	virtual ip6_addr_t getIp6Addr(Resource*);
	virtual const char *getString2(Resource*, const char*);
	virtual const char *getStringN(Resource*, int);

	virtual void setString(Resource*, const char*);
	virtual void setInt(Resource*, int);
	virtual void setLong(Resource*, long);
	virtual void setIp4Addr(Resource*, ip4_addr_t);
	virtual void setIp6Addr(Resource*, ip6_addr_t);
	virtual void setString2(Resource*, const char*, const char*);
	virtual void setStringN(Resource*, int, const char*);

	virtual void clear(Resource*);
	virtual void clearString2(Resource*, const char*);
	virtual void clearStringN(Resource*, int);

protected:
	ResourceInfo::FieldType type;
	union {
		const char *(Resource::*s)();
		int (Resource::*i)();
		long (Resource::*l)();
		ip4_addr_t (Resource::*a4)();
		ip6_addr_t (Resource::*a6)();
		const char *(Resource::*s2)(const char*);
		const char *(Resource::*sn)(int);
	} get_u;
	union {
		void (Resource::*s)(const char *);
		void (Resource::*i)(int);
		void (Resource::*l)(long);
		void (Resource::*a4)(ip4_addr_t);
		void (Resource::*a6)(ip6_addr_t);
		void (Resource::*s2)(const char*, const char*);
		void (Resource::*sn)(int, const char*);
	} set_u;
	union {
		void (Resource::*c)();
		void (Resource::*s2)(const char*);
		void (Resource::*sn)(int);
	} clear_u;
};

inline ResourceInfo::FieldType ResourceInfo::getType() {
	return type;
}
#endif
