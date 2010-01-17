/**
 * Module, pure virtual class, ancestor of all modules.
 */

#ifndef _MODULE_H_
#define _MODULE_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "Object.h"
#include "Resource.h"

typedef enum {
	MODULE_INPUT =  0,
	MODULE_OUTPUT = 1,
	MODULE_SIMPLE = 2,
	MODULE_MULTI =  3,
	MODULE_SELECT = 4
} module_t;

class Module : public Object {
public:
	Module(ObjectRegistry *objects, const char *id): Object(objects, id) {};
	virtual ~Module() {};
	virtual bool Init(vector<pair<string, string> > *args) = 0;
	virtual module_t getType() = 0;
	virtual Resource *Process(Resource *resource);
	virtual int Process(Resource **in, Resource **out);
	virtual void SaveCheckpoint(const char *path, const char *id) = 0;
	virtual void RestoreCheckpoint(const char *path, const char *id) = 0;

	virtual char *getValue(const char *name) = 0;
	virtual bool setValue(const char *name, const char *value) = 0;
	virtual vector<string> *listNames() = 0;

protected:
	static log4cxx::LoggerPtr logger;
};

#endif
