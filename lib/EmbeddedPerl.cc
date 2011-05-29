/**
 */

#include <assert.h>
#include <string.h>
#include "common.h"
#include "EmbeddedPerl.h"
#include "LibraryLoader.h"
#include "Object.h"

EXTERN_C void xs_init (pTHX);

using namespace std;

log4cxx::LoggerPtr EmbeddedPerl::logger(log4cxx::Logger::getLogger("lib.EmbeddedPerl"));

EmbeddedPerl::EmbeddedPerl() {
	void *old = GetContext();
	my_perl = perl_alloc();
	perl_construct(my_perl);
	SetContext(old);
}

EmbeddedPerl::~EmbeddedPerl() {
	void *old = GetContext();
	SetContext();
	perl_destruct(my_perl);
	perl_free(my_perl);
	if (old != my_perl)
		SetContext(old);
}

bool EmbeddedPerl::Init() {
	void *old = GetContext();
	SetContext();

	// run Perl
	const char *embedding[] = { "", "-e", "0" };
	perl_parse(my_perl, xs_init, 3, (char **)embedding, NULL);
	perl_run(my_perl);

	// use Hector;
	ENTER;
	SAVETMPS;
	eval_pv("use Hector;", FALSE);
	FREETMPS;
	LEAVE;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error import Hector module (" << SvPV_nolen(ERRSV) << ")");
		SetContext(old);
		return false;
	}

	// save SWIG functions to call
	Perl_TypeQuery = (swig_type_info*(*)(const char*))LibraryLoader::LoadLibrary("Hector.so", "Perl_TypeQuery_Wrapper");
	if (!Perl_TypeQuery) {
		LOG4CXX_ERROR(logger, "Cannot find Perl_TypeQuery_Wrapper");
		SetContext(old);
		return false;
	}
	Perl_NewPointerObj = (SV*(*)(void*, swig_type_info*, int))LibraryLoader::LoadLibrary("Hector.so", "Perl_NewPointerObj_Wrapper");
	if (!Perl_NewPointerObj) {
		LOG4CXX_ERROR(logger, "Cannot find Perl_NewPointerObj_Wrapper");
		SetContext(old);
		return false;
	}
	Perl_ConvertPtr = (int(*)(SV*, void**, swig_type_info *ty, int))LibraryLoader::LoadLibrary("Hector.so", "Perl_ConvertPtr_Wrapper");
	if (!Perl_ConvertPtr) {
		LOG4CXX_ERROR(logger, "Cannot find Perl_ConvertPtr_Wrapper");
		SetContext(old);
		return false;
	}

	SetContext(old);
	return true;
}

SV *EmbeddedPerl::NewPointerObj(void *ptr, const char *type, int flags) {
	// typeInfoCache is used only with GIL lock held, so no need to apply further lock
	tr1::unordered_map<string, swig_type_info*>::iterator iter = typeInfoCache.find(type);
	swig_type_info *info;
	if (iter != typeInfoCache.end()) {
		info = iter->second;
	} else {
		info = (*Perl_TypeQuery)(type);
		if (!info)
			return NULL;
		typeInfoCache[type] = info;
	}
	return (*Perl_NewPointerObj)(ptr, info, flags);
}

int EmbeddedPerl::ConvertPtr(SV *obj, void **ptr, const char *type, int flags) {
	// typeInfoCache is used only with GIL lock held, so no need to apply further lock
	tr1::unordered_map<string, swig_type_info*>::iterator iter = typeInfoCache.find(type);
	swig_type_info *info;
	if (iter != typeInfoCache.end()) {
		info = iter->second;
	} else {
		info = (*Perl_TypeQuery)(type);
		if (!info)
			return -1;
		typeInfoCache[type] = info;
	}
	return (*Perl_ConvertPtr)(obj, ptr, info, flags);
}
