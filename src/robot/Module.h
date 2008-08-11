/**
 * Module, pure virtual class, ancestor of all modules.
 */

#ifndef _MODULE_H_
#define _MODULE_H_

#include "Config.h"
#include "Resource.h"

class Module {
public:
	Module() {};
	virtual ~Module() {};
	virtual bool Init(Config *config, const char *name) = 0;
};

#endif
