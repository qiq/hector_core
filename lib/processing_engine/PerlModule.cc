/**
 * N.B.: Only one thread can use Perl interpreter at the time, so we had to lock it for write.
 */

#include <assert.h>
#include <string.h>
#include "../common.h"
#include "PerlModule.h"

log4cxx::LoggerPtr PerlModule::logger(log4cxx::Logger::getLogger("lib.processing_engine.PerlModule"));

EXTERN_C void xs_init (pTHX);

// based on SWIG code

XS(_wrap_new_Any) {
	int argvi = 0;
	int ecode;
	unsigned long addr;
	char *name = NULL;
	dXSARGS;

	if ((items < 2) || (items > 2)) {
		sv_setpvf(GvSV(PL_errgv),"%s %s\n", "RuntimeError", "Usage: new_Any(string, unsigned long);");
		croak(Nullch);
	}

	if (!SvPOK(ST(0))) {
		sv_setpvf(GvSV(PL_errgv),"%s %s\n", "RuntimeError", "Invalid name (string)");
		croak(Nullch);
	}
	STRLEN len = 0;
	char *cstr = SvPV(ST(0), len); 
	name = strndup(cstr, len);

	if (!SvIOK(ST(1))) {
		sv_setpvf(GvSV(PL_errgv),"%s %s\n", "RuntimeError", "Invalid address (unsigned long)");
		croak(Nullch);
	}
	addr = SvIV(ST(1));

// SWIG_Perl_NewPointerObj(SWIG_MAYBE_PERL_OBJECT void *ptr, swig_type_info *t, int flags) {
	SV *sv = sv_newmortal();
	void *ptr = const_cast< void * >((void *)addr);

// SWIG_MakePtr(sv, ptr, _swigt__p_TestResource, SWIG_OWNER | SWIG_SHADOW);
	SV *self;
	SV *obj=newSV(0);
	HV *hash=newHV();
	HV *stash;
	sv_setref_pv(obj, name, ptr);
	stash=SvSTASH(SvRV(obj));
	    HV *hv;
	    GV *gv=*(GV**)hv_fetch(stash, "OWNER", 5, TRUE);
	    if (!isGV(gv))
		gv_init(gv, stash, "OWNER", 5, FALSE);
	    hv=GvHVn(gv);
	    hv_store_ent(hv, obj, newSViv(1), 0);
	sv_magic((SV *)hash, (SV *)obj, 'P', Nullch, 0);
	SvREFCNT_dec(obj);
	self=newRV_noinc((SV *)hash);
	sv_setsv(sv, self);
	SvREFCNT_dec((SV *)self);
	sv_bless(sv, stash);
//
	ST(argvi) = sv;
//
	argvi++ ;

	free(name);

	XSRETURN(argvi);
}

// based on SWIG code

void *convert_ptr(SV *sv, bool disown) {
	void *ptr = NULL;
	SV *tsv = 0;
	/* If magical, apply more magic */
	if (SvGMAGICAL(sv))
		mg_get(sv);

	/* Check to see if this is an object */
	if (sv_isobject(sv)) {
		IV tmp = 0;
		tsv = (SV*) SvRV(sv);
		if ((SvTYPE(tsv) == SVt_PVHV)) {
			MAGIC *mg;
			if (SvMAGICAL(tsv)) {
				mg = mg_find(tsv,'P');
				if (mg) {
					sv = mg->mg_obj;
					if (sv_isobject(sv)) {
						tsv = (SV*)SvRV(sv);
						tmp = SvIV(tsv);
					}
				}
			} else {
				return NULL;
			}
		} else {
			tmp = SvIV(tsv);
		}
   		/* SWIG: DISOWN implementation: we need a perl guru to check this one. */
		if (tsv && disown) {
			SV *obj = sv;
			HV *stash = SvSTASH(SvRV(obj));
			GV *gv = *(GV**) hv_fetch(stash, "OWNER", 5, TRUE);
			if (isGV(gv)) {
				HV *hv = GvHVn(gv);
				if (hv_exists_ent(hv, obj, 0))
					hv_delete_ent(hv, obj, 0, 0);
			}
		}
		return (void*)(unsigned long)(tmp);
	} 
	return NULL;
}

SV *PerlModule::CreatePerlResource(Resource *resource) {
	SV *result;
	const char *type = resource->getTypeStr();
	// initialize resource type
	if (initialized.find(type) == initialized.end()) {
		char s[1024];
		snprintf(s, sizeof(s), "package Hector::%s; sub new2 { my $pkg = shift; my $self = Hectorc::new_Any($pkg, shift); bless $self, $pkg if defined($self); }", type);
		eval_pv(s, FALSE);
		if (SvTRUE(ERRSV)) {
			LOG_ERROR(logger, "Error initialize " << type << " (" << SvPV_nolen(ERRSV) << ")");
			return NULL;
		}
		initialized.insert(type);
	}

	// create new instance of a resource (of given type): Hector::XXXResource->new2(0xabc)
	char s[1024];
	snprintf(s, sizeof(s), "Hector::%s", type);
	dSP;
	ENTER;
        PUSHMARK(SP);
	XPUSHs(sv_2mortal(newSVpv(s, strlen(s))));
        XPUSHs(sv_2mortal(newSViv((unsigned long)resource)));
        PUTBACK;
	int count = call_method("new2", G_SCALAR);
	SPAGAIN;
	if (count != 1) {
		LOG_ERROR(logger, "Error calling new2 for " << type);
		return NULL;
	}
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(logger, "Error calling Init, module " << name << " (" << SvPV_nolen(ERRSV) << ")");
		return NULL;
	}
	result = SvREFCNT_inc(POPs);
	PUTBACK;
	FREETMPS;
	LEAVE;

	return result;
}

PerlModule::PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name): Module(objects, id, threadIndex) {
	this->name = strdup(name);
	// remove .pm at the end of module name
	char *dot = strrchr(this->name, '.');
	if (dot)
		*dot = '\0';
	this->threadIndex = threadIndex;
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
	const char *embedding[] = { "", "-e", "0" };
	perl_parse(my_perl, xs_init, 3, (char **)embedding, NULL);
	PERL_SET_CONTEXT(my_perl);
	perl_run(my_perl);

	// init SWIG extension (_new_Any)
	newXS("Hectorc::new_Any", _wrap_new_Any, (char*)__FILE__);

	// create Hector::Object::new2()
	eval_pv("package Hector::Object; sub new2 { my $pkg = shift; my $self = Hectorc::new_Any($pkg, shift); bless $self, $pkg if defined($self); }", FALSE);
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(logger, "Error initialize Hector::Object (new2) (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}

	// call Hector::Object->new2()
	char s[1024];
	snprintf(s, sizeof(s), "use Hector; $_object = Hector::Object->new2(%ld); $_object->DISOWN()", (unsigned long)dynamic_cast<Object*>(this));
	eval_pv(s , FALSE);
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(logger, "Error initialize Hector::Object (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}

	// create Perl module
	snprintf(s, sizeof(s), "use %s; $_module = %s->new($_object, '%s', %d);", name, name, getId(), threadIndex);
	eval_pv(s, FALSE);
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(logger, "Error initialize module " << name << " (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}
	ref = get_sv("_module", 0);
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

Module::Type PerlModule::getType() {
	ObjectLockWrite();
	PERL_SET_CONTEXT(my_perl);
	int result = 0;
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
	return (Module::Type)result;
}

Resource *PerlModule::Process(Resource *resource) {
	ObjectLockWrite();
	PERL_SET_CONTEXT(my_perl);
	Resource *result = NULL;
	SV *resourceSV;
	if (resource) {
		// create new instance of a resource (of given type)
		if (!(resourceSV = CreatePerlResource(resource))) {
			ObjectUnlock();
			return NULL;
		}
	} else {
		resourceSV = &PL_sv_undef;
	}

	// call Process method: $module->Process($resource)
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(resourceSV));
        PUTBACK;
	int count = call_method("Process", G_SCALAR);
	SPAGAIN;
	if (count == 1)
		resourceSV = SvREFCNT_inc(POPs);
	PUTBACK;
	FREETMPS;
	LEAVE;

	// get Resource C++ pointer & DISOWN
	result = reinterpret_cast<Resource*>(convert_ptr(resourceSV, true));

	// delete Perl resource object
	SvREFCNT_dec(resourceSV);

	ObjectUnlock();
	return result;
}

int PerlModule::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	ObjectLockWrite();
	PERL_SET_CONTEXT(my_perl);
	int result = 0;
	SV *inputResourcesSV;
	SV *outputResourcesSV;

	// convert inputResources and outputResources into Perl objects
	if (inputResources) {
		AV *a = newAV();
		while (inputResources->size() > 0) {
			SV *resourceSV = CreatePerlResource(inputResources->front());
			if (!resourceSV) {
				ObjectUnlock();
				return 0;
			}
			av_push(a, newSVsv(resourceSV));
			inputResources->pop();
		}
		inputResourcesSV = newRV_noinc((SV*)a);
	} else {
		inputResourcesSV = &PL_sv_undef;
	}

	if (outputResources) {
		AV *a = newAV();
		outputResourcesSV = newRV_noinc((SV*)a);
	} else {
		outputResourcesSV = &PL_sv_undef;
	}

	// call Process method: $module->Process($resource)
	dSP;
	ENTER;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(inputResourcesSV);
        XPUSHs(outputResourcesSV);
        PUTBACK;
	int count = call_method("ProcessMulti", G_SCALAR);
	SPAGAIN;
	if (count == 1)
		result = POPi;
	PUTBACK;
	FREETMPS;
	LEAVE;

	// create back inputResource and outputResource from Perl objects & DISOWN them
	// inputResources queue was cleaned before, outputResource was not
	SV *sv;
	while ((sv = av_shift((AV*)SvRV(inputResourcesSV))) != &PL_sv_undef) {
		inputResources->push(reinterpret_cast<Resource*>(convert_ptr(sv, true)));
	}
	while ((sv = av_shift((AV*)SvRV(outputResourcesSV))) != &PL_sv_undef) {
		outputResources->push(reinterpret_cast<Resource*>(convert_ptr(sv, true)));
	}

	// delete Perl resource object
	SvREFCNT_dec(inputResourcesSV);
	SvREFCNT_dec(outputResourcesSV);

	ObjectUnlock();
	return result;
}

char *PerlModule::getValueSync(const char *name) {
	ObjectUnlock();
	ObjectLockWrite();	// we need write lock for Perl
	PERL_SET_CONTEXT(my_perl);
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
	PERL_SET_CONTEXT(my_perl);
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
	ObjectUnlock();
	ObjectLockWrite();	// we need write lock for Perl
	PERL_SET_CONTEXT(my_perl);
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
	PERL_SET_CONTEXT(my_perl);
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
	PERL_SET_CONTEXT(my_perl);
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
