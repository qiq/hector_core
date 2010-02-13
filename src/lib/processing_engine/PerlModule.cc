/**
 * External simple
 */

#include <assert.h>
#include <string.h>
#include "../common.h"
#include "PerlModule.h"

log4cxx::LoggerPtr PerlModule::logger(log4cxx::Logger::getLogger("lib.processing_engine.PerlModule"));

EXTERN_C void xs_init (pTHX);

PerlModule::PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name): Module(objects, id, threadIndex) {
	name = strdup(name);
	my_perl = perl_alloc();
	perl_construct(my_perl);
	ref = NULL;
}

PerlModule::~PerlModule() {
	perl_destruct(my_perl);
	perl_free(my_perl);
	free(name);
}

bool PerlModule::Init(vector<pair<string, string> > *c) {
	// run Perl
	vector<string> env;
	for (vector<pair<string, string> >::iterator iter = c->begin(); iter != c->end(); ++iter) {
		if (iter->first == "env") {
			env.push_back(iter->second);
		}
	}
	const char ** envv = new const char*[env.size()+1];
	for (int i = 0; i < (int)env.size(); i++) {
		envv[i] = env[i].c_str();
	}
	envv[env.size()] = NULL;

	const char *embedding[] = { "", "-e", "0" };
	perl_parse(my_perl, xs_init, 3, (char **)embedding, (char **)envv);
	delete envv;
	perl_run(my_perl);
	char s[1024];
	snprintf(s, sizeof(s), "use DummyModule; $module = %s->new();", name);
	eval_pv(s, FALSE);
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(logger, "Error initialize module " << name << " (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}
	ref = get_sv("module", 0);
	if (!SvOK(ref)) {
		LOG_ERROR(logger, "Error initialize module " << name);
		return false;
	}

	// call Init()
	AV *table = newAV();
	for (vector<pair<string, string> >::iterator iter = c->begin(); iter != c->end(); ++iter) {
		AV *row = newAV();
		av_push(row, newSVpv(iter->first.c_str(), 0));
		av_push(row, newSVpv(iter->second.c_str(), 0));
		av_push(table, newRV_noinc((SV*)row ));
	}
	SV *sv = newRV_noinc((SV*)table);
	int result;
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(sv));
        PUTBACK;
	int count = call_method("Init", G_SCALAR);
	SPAGAIN;
	if (count != 1) {
		LOG_ERROR(logger, "Error calling Init, module " << name);
		return false;
	}
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(logger, "Error calling Init, module " << name << " (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}
	result = POPi;
	PUTBACK;
	FREETMPS;
	LEAVE;
	return result == 1;
}

module_t PerlModule::getType() {
	int result = 0;
	ObjectLock();
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("getType", G_SCALAR);
	SPAGAIN;
	if (count == 1)
		result = POPi;
	PUTBACK;
	FREETMPS;
	LEAVE;
	ObjectUnlock();
	return (module_t)result;
}

Resource *PerlModule::Process(Resource *resource) {
	int result = 0;
	int ptr = (int)&resource;
	ObjectLock();
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSViv(ptr)));
        PUTBACK;
	int count = call_method("Process", G_SCALAR);
	SPAGAIN;
	if (count == 1)
		result = POPi;
	PUTBACK;
	FREETMPS;
	LEAVE;
	ObjectUnlock();
	return (Resource*)result;
}

char *PerlModule::getValueSync(const char *name) {
	SV *sv;
	int count;
	char *result = NULL;
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(name, 0)));
        PUTBACK;
	count = call_method("getValueSync", G_SCALAR);
	SPAGAIN;
	if (count == 1) {
		sv = POPs;
		STRLEN len;
		char *s = SvPV(sv, len);
		result = strndup(s, len);
	}
	PUTBACK;
	FREETMPS;
	LEAVE;

	return result;
}

bool PerlModule::setValueSync(const char *name, const char *value) {
	int result = 0;
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(name, 0)));
        XPUSHs(sv_2mortal(newSVpv(value, 0)));
        PUTBACK;
	int count = call_method("setValueSync", G_SCALAR);
	SPAGAIN;
	if (count == 1)
		result = POPi;
	PUTBACK;
	FREETMPS;
	LEAVE;
	return result == 1;
}

vector<string> *PerlModule::listNamesSync() {
	vector<string> *result = new vector<string>();
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("listNamesSync", G_SCALAR);
	SPAGAIN;
	if (count == 1) {
		SV *sv = POPs;
		if (SvTYPE(SvRV(sv)) == SVt_PVAV) {
			AV *av = (AV*)SvRV(sv);
			int alen = av_len(av);

			for (int i = 0; i <= alen; i++) {
				STRLEN len;
				SV **val = av_fetch(av, i, 0);
				char *s = SvPV(*val, len);
				char *s2 = strndup(s, len);
				result->push_back(s2);
				free(s2);
			}
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	return result;
}

void PerlModule::SaveCheckpointSync(const char *path, const char *id) {
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(path, 0)));
        XPUSHs(sv_2mortal(newSVpv(id, 0)));
        PUTBACK;
	call_method("setValueSync", G_DISCARD);
	PUTBACK;
	FREETMPS;
	LEAVE;
}

void PerlModule::RestoreCheckpointSync(const char *path, const char *id) {
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(path, 0)));
        XPUSHs(sv_2mortal(newSVpv(id, 0)));
        PUTBACK;
	call_method("setValueSync", G_DISCARD);
	PUTBACK;
	FREETMPS;
	LEAVE;
}

