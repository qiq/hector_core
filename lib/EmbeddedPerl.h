/**
 * Embedding Perl interpreter
 */

#ifndef _LIB_PROCESSING_ENGINE_EMBEDDED_PERL_H_
#define _LIB_PROCESSING_ENGINE_EMBEDDED_PERL_H_

#include <config.h>

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#undef New	// protocol buffers
#undef Pause	// Processor

#include <cstdarg>
#include <log4cxx/logger.h>
#include "PlainLock.h"

struct swig_type_info;

class EmbeddedPerl {
public:
	EmbeddedPerl();
	~EmbeddedPerl();
	void *GetContext();
	void SetContext(void *ctx = NULL);
	bool Init();

	SV *NewPointerObj(void *ptr, const char *type, int flags);
	int ConvertPtr(SV *obj, void **ptr, const char *type, int flags);

private:
	PerlInterpreter *my_perl;

	swig_type_info *(*Perl_TypeQuery)(const char *type);
	SV *(*Perl_NewPointerObj)(void *ptr, swig_type_info *type, int flags);
	int (*Perl_ConvertPtr)(SV *obj, void **ptr, swig_type_info *ty, int flags);
	std::tr1::unordered_map<std::string, swig_type_info*> typeInfoCache;

	static log4cxx::LoggerPtr logger;
};

inline void *EmbeddedPerl::GetContext() {
	return PERL_GET_CONTEXT;
}

// ctx == NULL: set current perl as a context
inline void EmbeddedPerl::SetContext(void *ctx) {
	if (ctx)
		PERL_SET_CONTEXT(ctx);
	else
		PERL_SET_CONTEXT(my_perl);
}

#endif
