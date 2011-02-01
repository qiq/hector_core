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
	void SetContext();
	bool Init();

	SV *NewPointerObj(void *ptr, const char *type, int flags);
	int ConvertPtr(SV *obj, void **ptr, const char *type, int flags);

private:
	static swig_type_info *(*Perl_TypeQuery)(const char *type);
	static SV *(*Perl_NewPointerObj)(void *ptr, swig_type_info *type, int flags);
	static int (*Perl_ConvertPtr)(SV *obj, void **ptr, swig_type_info *ty, int flags);
	static std::tr1::unordered_map<std::string, swig_type_info*> typeInfoCache;

	PerlInterpreter *my_perl;

	static log4cxx::LoggerPtr logger;
};

inline void EmbeddedPerl::SetContext() {
	PERL_SET_CONTEXT(my_perl);
}

#endif
