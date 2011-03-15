/**
 * 
 */

#ifndef _LIB_PROCESSING_ENGINE_PERL_INTERPTERERS_H_
#define _LIB_PROCESSING_ENGINE_PERL_INTERPTERERS_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "PlainLock.h"
#include "PerlResourceInterpreter.h"

class PerlInterpreters {
public:
	PerlInterpreters() {};
	~PerlInterpreters();

	PerlResourceInterpreter *AcquirePerl();
	void ReleasePerl(PerlResourceInterpreter *pi);

protected:
	// locking access to the following variables
	PlainLock lock;
	// Perl interpreters that we use, it is a heap, where lowest-occupied interpreter is first
	std::vector<PerlResourceInterpreter*> interpreters;

	static log4cxx::LoggerPtr logger;
};

#endif
