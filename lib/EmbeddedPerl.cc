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
	my_perl = perl_alloc();
	perl_construct(my_perl);
}

EmbeddedPerl::~EmbeddedPerl() {
	SetContext();
	perl_destruct(my_perl);
	perl_free(my_perl);
}

bool EmbeddedPerl::Init() {
	// run Perl
	const char *embedding[] = { "", "-e", "0" };
	perl_parse(my_perl, xs_init, 3, (char **)embedding, NULL);
	SetContext();
	perl_run(my_perl);

	// use Hector;
	eval_pv("use Hector;", FALSE);
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error import Hector module (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}

	// save SWIG functions to call
	Perl_TypeQuery = (swig_type_info*(*)(const char*))LibraryLoader::LoadLibrary("Hector.so", "Perl_TypeQuery_Wrapper");
	if (!Perl_TypeQuery) {
		LOG4CXX_ERROR(logger, "Cannot find Perl_TypeQuery_Wrapper");
		return false;
	}
	Perl_NewPointerObj = (SV*(*)(void*, swig_type_info*, int))LibraryLoader::LoadLibrary("Hector.so", "Perl_NewPointerObj_Wrapper");
	if (!Perl_NewPointerObj) {
		LOG4CXX_ERROR(logger, "Cannot find Perl_NewPointerObj_Wrapper");
		return false;
	}
	Perl_ConvertPtr = (int(*)(SV*, void**, swig_type_info *ty, int))LibraryLoader::LoadLibrary("Hector.so", "Perl_ConvertPtr_Wrapper");
	if (!Perl_ConvertPtr) {
		LOG4CXX_ERROR(logger, "Cannot find Perl_ConvertPtr_Wrapper");
		return false;
	}

	return true;
}

SV *EmbeddedPerl::NewPointerObj(void *ptr, const char *type, int flags) {
	SetContext();
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
	SetContext();
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
