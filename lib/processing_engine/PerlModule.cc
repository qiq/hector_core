/**
 * N.B.: Only one thread can use Perl interpreter at the time, so we had to lock it for write.
 */

#include <assert.h>
#include <string.h>
#include "common.h"
#include "PerlModule.h"

using namespace std;

PerlModule::PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name): Module(objects, id, threadIndex) {
	this->name = strdup(name);
	// remove .pm at the end of module name
	char *dot = strrchr(this->name, '.');
	if (dot)
		*dot = '\0';
	this->threadIndex = threadIndex;
	perl = new EmbeddedPerl();
	ref = NULL;
}

PerlModule::~PerlModule() {
	delete perl;
	free(name);
}

bool PerlModule::Init(vector<pair<string, string> > *c) {
	perl->SetContext();
	SV *sv = &PL_sv_undef;
	// first stage?
	if (c) {
		// run Perl
		if (!perl->Init())
			return false;

		// create Perl module
		SV *_object = get_sv("_object", TRUE);
		sv_setsv(_object, perl->NewPointerObj(const_cast<void*>(static_cast<const void*>(this)), "Object *", 0));
		char s[1024];
		snprintf(s, sizeof(s), "use %s; $_module = %s->new($_object, '%s', %d);", name, name, GetId(), threadIndex);
		ENTER;
		SAVETMPS;
		eval_pv(s, FALSE);
		FREETMPS;
		LEAVE;
		if (SvTRUE(ERRSV)) {
			LOG_ERROR(this, "Error initialize module " << name << " (" << SvPV_nolen(ERRSV) << ")");
			return false;
		}
		ref = get_sv("_module", 0);
		if (!SvOK(ref)) {
			LOG_ERROR(this, "Error initialize module " << name);
			return false;
		}

		AV *table = newAV();
		for (vector<pair<string, string> >::iterator iter = c->begin(); iter != c->end(); ++iter) {
			AV *row = newAV();
			av_push(row, newSVpv(iter->first.c_str(), 0));
			av_push(row, newSVpv(iter->second.c_str(), 0));
			av_push(table, newRV_noinc((SV*)row ));
		}
		sv = sv_2mortal(newRV_noinc((SV*)table));
		
	}
	// call Init()
	bool result = false;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv);
        PUTBACK;
	int count = call_method("Init", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling Init (" << SvPV_nolen(ERRSV) << ")");
		POPs;
		result = false;
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling Init (no result)");
		result = false;
	} else {
		SV *resultSV = POPs;
		if (!SvIOK(resultSV)) {
			LOG_ERROR(this, "Error calling Init (invalid result)");
			result = false;
		} else {
			result = (SvIV(resultSV) != 0);
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	return result;
}

Module::Type PerlModule::GetType() {
	ObjectLockWrite();
	perl->SetContext();
	Module::Type result = Module::INVALID;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("GetType", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling GetType (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling GetType (no result)");
	} else {
		SV *resultSV = POPs;
		if (!SvIOK(resultSV)) {
			LOG_ERROR(this, "Error calling GetType (invalid result)");
		} else {
			result = (Module::Type)SvIV(resultSV);
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	ObjectUnlock();
	return result;
}

void PerlModule::StartSync() {
	perl->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	call_method("Start", G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG_ERROR(this, "Error calling Start");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

void PerlModule::StopSync() {
	perl->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	call_method("Stop", G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG_ERROR(this, "Error calling Stop");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

void PerlModule::PauseSync() {
	perl->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	call_method("Pause", G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG_ERROR(this, "Error calling Pause");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

void PerlModule::ResumeSync() {
	perl->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	call_method("Resume", G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG_ERROR(this, "Error calling Resume");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

Resource *PerlModule::ProcessInputSync(bool sleep) {
	perl->SetContext();
	Resource *result = NULL;

	// call Process method: $module->Process($resource)
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("ProcessInput", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling ProcessInput (" << SvPV_nolen(ERRSV) << ")");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return result;
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling ProcessInput");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return result;
	}
	SV *resourceSV = SvREFCNT_inc(POPs);
	PUTBACK;
	FREETMPS;
	LEAVE;

	// get Resource C++ pointer & DISOWN
	void *r = NULL;
	if (perl->ConvertPtr(resourceSV, &r, "Resource *", 0x01) >= 0) {
		result = reinterpret_cast<Resource*>(r);
	} else {
		LOG_ERROR(this, "Error calling ProcessInput");
	}

	// delete Perl resource object
	SvREFCNT_dec(resourceSV);

	return result;
}

Resource *PerlModule::ProcessOutputSync(Resource *resource) {
	perl->SetContext();
	Resource *result = NULL;
	SV *resourceSV;
	if (resource) {
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "%s *", resource->GetObjectName());
		resourceSV = perl->NewPointerObj(const_cast<void*>(static_cast<const void*>(resource)), buffer, 0x01);
		if (!resourceSV)
			return NULL;
	} else {
		resourceSV = &PL_sv_undef;
	}

	// call Process method: $module->Process($resource)
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(resourceSV);
        PUTBACK;
	int count = call_method("ProcessOutput", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling ProcessOutput (" << SvPV_nolen(ERRSV) << ")");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return result;
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling ProcessOutput");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return result;
	}
	resourceSV = SvREFCNT_inc(POPs);
	PUTBACK;
	FREETMPS;
	LEAVE;

	// get Resource C++ pointer & DISOWN
	void *r = NULL;
	if (perl->ConvertPtr(resourceSV, &r, "Resource *", 0x01) >= 0) {
		result = reinterpret_cast<Resource*>(r);
	} else {
		LOG_ERROR(this, "Error calling ProcessOutput");
	}

	// delete Perl resource object
	SvREFCNT_dec(resourceSV);

	return result;
}

Resource *PerlModule::ProcessSimpleSync(Resource *resource) {
	perl->SetContext();
	Resource *result = NULL;
	SV *resourceSV;
	if (resource) {
		// create new instance of a resource (of given type)
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "%s *", resource->GetObjectName());
		resourceSV = perl->NewPointerObj(const_cast<void*>(static_cast<const void*>(resource)), buffer, 0x01);
		if (!resourceSV)
			return NULL;
	} else {
		resourceSV = &PL_sv_undef;
	}

	// call Process method: $module->Process($resource)
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(resourceSV);
        PUTBACK;
	int count = call_method("ProcessSimple", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling ProcessSimple (" << SvPV_nolen(ERRSV) << ")");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return result;
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling ProcessSimple");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return result;
	}
	resourceSV = SvREFCNT_inc(POPs);
	PUTBACK;
	FREETMPS;
	LEAVE;

	// get Resource C++ pointer & DISOWN
	void *r = NULL;
	if (perl->ConvertPtr(resourceSV, &r, "Resource *", 0x01) >= 0) {
		result = reinterpret_cast<Resource*>(r);
	} else {
		LOG_ERROR(this, "Error calling ProcessSimple");
	}

	// delete Perl resource object
	SvREFCNT_dec(resourceSV);

	return result;
}

bool PerlModule::ProcessMultiSync(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectingResources, int *processingResources) {
	perl->SetContext();
	SV *inputResourcesSV;
	SV *outputResourcesSV;
	bool result = false;

	if (processingResources)
		*processingResources = 0;
	if (expectingResources)
		*expectingResources = 1;

	// convert inputResources and outputResources into Perl objects
	if (inputResources) {
		AV *a = newAV();
		while (inputResources->size() > 0) {
			Resource *resource = inputResources->front();
			char buffer[100];
			snprintf(buffer, sizeof(buffer), "%s *", resource->GetObjectName());
			SV *resourceSV = perl->NewPointerObj(const_cast<void*>(static_cast<const void*>(resource)), buffer, 0x01);
			if (!resourceSV)
				return false;
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
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(inputResourcesSV);
        XPUSHs(outputResourcesSV);
        PUTBACK;
	int count = call_method("ProcessMulti", G_ARRAY|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling ProcessMulti (" << SvPV_nolen(ERRSV) << ")");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return false;
	} else if (count != 3) {
		LOG_ERROR(this, "Error calling ProcesMulti (invalid number of return arguments)");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return false;
	}
	SV *resultSV = POPs;
	if (!SvIOK(resultSV)) {
		LOG_ERROR(this, "Error calling ProcessMulti (invalid result 3)");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return false;
	}
	if (processingResources)
		*processingResources = SvIV(resultSV);
	resultSV = POPs;
	if (!SvIOK(resultSV)) {
		LOG_ERROR(this, "Error calling ProcessMulti (invalid result 2)");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return false;
	}
	if (expectingResources)
		*expectingResources = SvIV(resultSV);
	resultSV = POPs;
	if (!SvIOK(resultSV)) {
		LOG_ERROR(this, "Error calling ProcessMulti (invalid result 1)");
		PUTBACK;
		FREETMPS;
		LEAVE;
		return false;
	}
	result = SvIV(resultSV) != 0;
	PUTBACK;
	FREETMPS;
	LEAVE;

	// create back inputResource and outputResource from Perl objects & DISOWN them
	// inputResources queue was cleaned before, outputResource was not
	SV *sv;
	AV *a = (AV*)SvRV(inputResourcesSV);
	while ((sv = av_shift(a)) != &PL_sv_undef) {
		void *r = NULL;
		if (perl->ConvertPtr(sv, &r, "Resource *", 0x01) >= 0) {
			inputResources->push(reinterpret_cast<Resource*>(r));
		} else {
			LOG_ERROR(this, "Error calling ProcessMulti: invalid input resource");
		}
	}
	a = (AV*)SvRV(outputResourcesSV);
	while ((sv = av_shift(a)) != &PL_sv_undef) {
		void *r = NULL;
		if (perl->ConvertPtr(sv, &r, "Resource *", 0x01) >= 0) {
			outputResources->push(reinterpret_cast<Resource*>(r));
		} else {
			LOG_ERROR(this, "Error calling ProcessMulti: invalid outputresource");
		}
	}

	// delete Perl resource object
	SvREFCNT_dec(inputResourcesSV);
	SvREFCNT_dec(outputResourcesSV);

	return result;
}

char *PerlModule::GetPropertySync(const char *name) {
	ObjectUnlock();
	ObjectLockWrite();	// we need write lock for Perl
	perl->SetContext();
	int count;
	char *result = NULL;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(name, 0)));
        PUTBACK;
	count = call_method("GetProperty", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling GetProperty (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling GetProperty (no result)");
	} else {
		SV *sv = POPs;
		if (SvPOK(sv)) {
			STRLEN len;
			char *s = SvPV(sv, len);
			result = strndup(s, len);
		} else if (SvIOK(sv)) {
			char s[1024];
			snprintf(s, sizeof(s), "%d", (int)SvIV(sv));
			result = strdup(s);
		} else if (SvNOK(sv)) {
			char s[1024];
			snprintf(s, sizeof(s), "%lf", SvNV(sv));
			result = strdup(s);
		} else {
			LOG_ERROR(this, "Error calling GetProperty (invalid result type)");
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;

	return result;
}

bool PerlModule::SetPropertySync(const char *name, const char *value) {
	perl->SetContext();
	bool result = false;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(name, 0)));
        XPUSHs(sv_2mortal(newSVpv(value, 0)));
        PUTBACK;
	int count = call_method("SetProperty", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling SetProperty (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling SetProperty (no result)");
	} else {
		SV *resultSV = POPs;
		if (!SvIOK(resultSV)) {
			LOG_ERROR(this, "Error calling SetProperty (invalid result)");
		} else {
			result = (SvIV(resultSV) != 0);
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	return result;
}

vector<string> *PerlModule::ListPropertiesSync() {
	ObjectUnlock();
	ObjectLockWrite();	// we need write lock for Perl
	perl->SetContext();
	vector<string> *result = NULL;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("ListProperties", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG_ERROR(this, "Error calling ListProperties (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG_ERROR(this, "Error calling ListProperties (no result)");
	} else {
		SV *sv = POPs;
		if (SvTYPE(SvRV(sv)) != SVt_PVAV) {
			LOG_ERROR(this, "Error calling ListProperties (invalid result)");
		} else {
			result = new vector<string>();
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
	perl->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(path, 0)));
        XPUSHs(sv_2mortal(newSVpv(id, 0)));
        PUTBACK;
	call_method("SaveCheckpoint", G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG_ERROR(this, "Error calling RestoreCheckpont");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

void PerlModule::RestoreCheckpointSync(const char *path, const char *id) {
	perl->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(path, 0)));
        XPUSHs(sv_2mortal(newSVpv(id, 0)));
        PUTBACK;
	call_method("RestoreCheckpoint", G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG_ERROR(this, "Error calling RestoreCheckpont");
	PUTBACK;
	FREETMPS;
	LEAVE;
}
