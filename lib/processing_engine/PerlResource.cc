/**
 * N.B.: Only one thread can use Perl interpreter at the time, so we had to lock it for write.
 */

#include <assert.h>
#include <string.h>
#include "common.h"
#include "PerlResource.h"
#include "ResourceAttrInfoP.h"

using namespace std;

log4cxx::LoggerPtr PerlResource::logger(log4cxx::Logger::getLogger("PerlResource"));
PerlInfoMap PerlResource::infoMap;

PerlInfoMap::~PerlInfoMap() {
	infoLock.Lock();
	for (tr1::unordered_map<int, ResourceInfo*>::iterator iter = infoMap.begin(); iter != infoMap.end(); ++iter)
		delete iter->second;
	infoLock.Unlock();
}

void PerlInfoMap::Lock() {
	infoLock.Lock();
}

void PerlInfoMap::Unlock() {
	infoLock.Unlock();
}

ResourceInfo *PerlInfoMap::GetResourceInfo(int typeId) {
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = infoMap.find(typeId);
	return iter == infoMap.end() ? NULL : iter->second;
}

void PerlInfoMap::SetResourceInfo(ResourceInfo *info) {
	infoMap[info->GetTypeId()] = info;
}

PerlResource::PerlResource(PerlResourceInterpreter *perl, const char *name) {
	this->perl = perl;
	this->name = strdup(name);
	// remove .pm at the end of module name
	int len = strlen(this->name);
	if (len > 4 && !memcmp(this->name+len-3, ".pm", 3))
		this->name[len-3] = '\0';
	typeId = 0;
	ref = NULL;
}

PerlResource::~PerlResource() {
	// delete object in Perl interpreter
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	char s[1024];
	snprintf(s, sizeof(s), "$_resource%d = undef;", GetId());
	eval_pv(s, FALSE);
	if (SvTRUE(ERRSV))
		LOG4CXX_ERROR(logger, "Error destroying resource " << name << " (" << SvPV_nolen(ERRSV) << ")");
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	
	free(name);
}

bool PerlResource::Init(bool reportErrors) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	char s[1024];

	// test whether it is a resource
	snprintf(s, sizeof(s), "use %s;", name);
	eval_pv(s, FALSE);
	if (SvTRUE(ERRSV)) {
		perl->GetPerl()->SetContext(old);
		perl->Unlock();
		if (reportErrors)
			LOG4CXX_ERROR(logger, "Error initialize resource " << name << " (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}
	snprintf(s, sizeof(s), "%s::ISA", name);
	AV *isa = get_av(s, 0);
	if (!isa) {
		perl->GetPerl()->SetContext(old);
		perl->Unlock();
		if (reportErrors)
			LOG4CXX_ERROR(logger, "Not a Resource descendant (no ISA) " << name);
		return false;
	}
	bool found = false;
	for (int i = 0; i <= av_len(isa); i++) {
		SV **item = av_fetch(isa, i, 0);
		if (item && SvPOK(*item)) {
			STRLEN len;
			char *s = SvPV(*item, len);
			if (!strncmp(s, "Resource", 9)) {
				found = true;
				break;
			}
		}
	}
	if (!found) {
		perl->GetPerl()->SetContext(old);
		perl->Unlock();
		if (reportErrors)
			LOG4CXX_ERROR(logger, "Not a Resource descendant: " << name);
		return false;
	}

	// create resource
	char id[20];
	snprintf(id, sizeof(id), "_resource%d", GetId());
	SV *_object = get_sv("_object", TRUE);
	sv_setsv(_object, perl->GetPerl()->NewPointerObj(const_cast<void*>(static_cast<const void*>(this)), "Resource *", 0));
	snprintf(s, sizeof(s), "use %s; $%s = %s->new($_object, %d);", name, id, name, GetId());
	eval_pv(s, FALSE);
	if (SvTRUE(ERRSV)) {
		perl->GetPerl()->SetContext(old);
		perl->Unlock();
		if (reportErrors)
			LOG4CXX_ERROR(logger, "Error initialize resource " << name << " (" << SvPV_nolen(ERRSV) << ")");
		return false;
	}
	ref = get_sv(id, 0);
	if (!SvOK(ref)) {
		perl->GetPerl()->SetContext(old);
		perl->Unlock();
		if (reportErrors)
			LOG4CXX_ERROR(logger, "Error initialize resource " << name);
		return false;
	}
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	return true;
}

Resource *PerlResource::Clone() {
	return new PerlResource(*this);
}

void PerlResource::Clear() {
	Resource::Clear();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	call_method("Clear", G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG4CXX_ERROR(logger, "Error calling Clear() (" << SvPV_nolen(ERRSV) << ")");
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

bool PerlResource::Serialize(ResourceOutputStream &output) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	bool result = false;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(perl->GetPerl()->NewPointerObj(const_cast<void*>(static_cast<const void*>(&output)), "ResourceOutputStream", 0x01)));
        PUTBACK;
	int count = call_method("Serialize", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling Serialize (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling Serialize (no result)");
	} else {
		SV *resultSV = POPs;
		if (!SvIOK(resultSV)) {
			LOG4CXX_ERROR(logger, "Error calling Serialize (invalid result)");
		} else {
			result = (SvIV(resultSV) != 0);
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	return result;
}

bool PerlResource::Deserialize(ResourceInputStream &input) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	bool result = false;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(perl->GetPerl()->NewPointerObj(const_cast<void*>(static_cast<const void*>(&input)), "ResourceInputStream", 0x01)));
        PUTBACK;
	int count = call_method("Deserialize", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling Deserialize (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling Deserialize (no result)");
	} else {
		SV *resultSV = POPs;
		if (!SvIOK(resultSV)) {
			LOG4CXX_ERROR(logger, "Error calling Deserialize (invalid result)");
		} else {
			result = (SvIV(resultSV) != 0);
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

inline ResourceInfo *PerlResource::GetResourceInfo() {
	ResourceInfo *result = NULL;
	if (typeId) {
		infoMap.Lock();
		result = infoMap.GetResourceInfo(typeId);
		if (result) {
			infoMap.Unlock();
			return result;
		}
	}

	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("GetResourceInfo", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling GetResourceInfo (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling GetResourceInfo (no result)");
	} else {
		bool error = false;
		result = new ResourceInfo();
		result->SetObjectName("PerlResource");
		while (true) {
			SV *resultSV = POPs;
			if (SvTYPE(SvRV(resultSV)) != SVt_PVHV) {
				LOG4CXX_ERROR(logger, "Error calling GetResourceInfo (expected hash reference)");
				error = true;
				break;
			}
			HV *h = (HV*)SvRV(resultSV);
			SV **p;
			STRLEN len;
			char *s;

			// typeId
			p = hv_fetch(h, "typeId", strlen("typeId"), 0);
			if (!p || !SvIOK(*p)) {
				LOG4CXX_ERROR(logger, "Error calling GetResourceInfo (typeId not present or non-integer)");
				error = true;
				break;
			}
			result->SetTypeId(SvIV(*p));

			// typeString
			p = hv_fetch(h, "typeString", strlen("typeString"), 0);
			if (!p || !SvPOK(*p)) {
				LOG4CXX_ERROR(logger, "Error calling GetResourceInfo (typeString not present or non-string)");
				error = true;
				break;
			}
			s = SvPV(*p, len);
			s = strndup(s, len);
			result->SetTypeString(s);
			free(s);
	
			// typeStringTerse
			p = hv_fetch(h, "typeStringTerse", strlen("typeStringTerse"), 0);
			if (!p || !SvPOK(*p)) {
				LOG4CXX_ERROR(logger, "Error calling GetResourceInfo (typeStringTerse not present or non-string)");
				error = true;
				break;
			}
			s = SvPV(*p, len);
			s = strndup(s, len);
			result->SetTypeStringTerse(s);
			free(s);

			// objectName
			result->SetObjectName("PerlResource");

			// attrInfoList
			p = hv_fetch(h, "attrInfoList", strlen("attrInfoList"), 0);
			if (!p || SvTYPE(SvRV(*p)) != SVt_PVAV) {
				LOG4CXX_ERROR(logger, "Error calling GetResourceInfo (attrInfoList is not an array refrence)");
				error = true;
				break;
			}
			vector<ResourceAttrInfo*> *attrInfoList = new vector<ResourceAttrInfo*>();
			AV *a = (AV*)SvRV(*p);
			SV *sv;
			while ((sv = av_shift(a)) != &PL_sv_undef) {
				if (SvTYPE(SvRV(sv)) == SVt_PVAV) {
					AV *item = (AV*)SvRV(sv);
					SV *second = av_pop(item);
					SV *first = av_pop(item);
					if (SvPOK(first) && SvPOK(second)) {
						ResourceAttrInfoP<PerlResource> *ai = new ResourceAttrInfoP<PerlResource>(result->GetTypeId());
						STRLEN len;
						char *s = SvPV(first, len);
						char *name = strndup(s, len);
						s = SvPV(second, len);
						if (!strcmp(s, "INT32")) {
							ai->InitInt32(name, &PerlResource::GetInt32, &PerlResource::SetInt32);
						} else if (!strcmp(s, "UINT32")) {
							ai->InitUInt32(name, &PerlResource::GetUInt32, &PerlResource::SetUInt32);
						} else if (!strcmp(s, "INT64")) {
							ai->InitInt64(name, &PerlResource::GetInt64, &PerlResource::SetInt64);
						} else if (!strcmp(s, "UINT64")) {
							ai->InitUInt64(name, &PerlResource::GetUInt64, &PerlResource::SetUInt64);
						} else if (!strcmp(s, "STRING")) {
							ai->InitString(name, &PerlResource::GetString, &PerlResource::SetString);
						} else if (!strcmp(s, "IP")) {
							ai->InitIpAddr(name, &PerlResource::GetIpAddr, &PerlResource::SetIpAddr);
						} else if (!strcmp(s, "ARRAY_INT32")) {
							ai->InitArrayInt32(name, &PerlResource::GetArrayInt32, &PerlResource::SetArrayInt32, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_UINT32")) {
							ai->InitArrayUInt32(name, &PerlResource::GetArrayUInt32, &PerlResource::SetArrayUInt32, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_INT64")) {
							ai->InitArrayInt64(name, &PerlResource::GetArrayInt64, &PerlResource::SetArrayInt64, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_UINT64")) {
							ai->InitArrayUInt64(name, &PerlResource::GetArrayUInt64, &PerlResource::SetArrayUInt64, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_STRING")) {
							ai->InitArrayString(name, &PerlResource::GetArrayString, &PerlResource::SetArrayString, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_IP")) {
							ai->InitArrayIpAddr(name, &PerlResource::GetArrayIpAddr, &PerlResource::SetArrayIpAddr, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "HASH_INT32")) {
							ai->InitHashInt32(name, &PerlResource::GetHashInt32, &PerlResource::SetHashInt32, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::Int32Values);
						} else if (!strcmp(s, "HASH_UINT32")) {
							ai->InitHashUInt32(name, &PerlResource::GetHashUInt32, &PerlResource::SetHashUInt32, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::UInt32Values);
						} else if (!strcmp(s, "HASH_INT64")) {
							ai->InitHashInt64(name, &PerlResource::GetHashInt64, &PerlResource::SetHashInt64, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::Int64Values);
						} else if (!strcmp(s, "HASH_UINT64")) {
							ai->InitHashUInt64(name, &PerlResource::GetHashUInt64, &PerlResource::SetHashUInt64, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::UInt64Values);
						} else if (!strcmp(s, "HASH_STRING")) {
							ai->InitHashString(name, &PerlResource::GetHashString, &PerlResource::SetHashString, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::StringValues);
						} else if (!strcmp(s, "HASH_IP")) {
							ai->InitHashIpAddr(name, &PerlResource::GetHashIpAddr, &PerlResource::SetHashIpAddr, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::IpAddrValues);
						} else {
							LOG4CXX_ERROR(logger, "Unknown attribute type: " << s);
							error = true;
							break;
						}
						free(name);
						attrInfoList->push_back(ai);
					} else {
						LOG4CXX_ERROR(logger, "Invalid array item in GetResourceInfo");
						error = true;
						break;
					}
				} else {
					LOG4CXX_ERROR(logger, "Invalid array item in GetResourceInfo (expected reference to array of arrays)");
					error = true;
					break;
				}
			}
			result->SetAttrInfoList(attrInfoList);
			break;
		}
		// error occured
		if (error) {
			delete result;
			result = NULL;
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	if (result) {
		typeId = result->GetTypeId();
		infoMap.SetResourceInfo(result);
	}
	infoMap.Unlock();

	return result;
}

#if 0
vector<ResourceAttrInfo*> *PerlResource::GetAttrInfoList() {
	vector<ResourceAttrInfo*> *result = new vector<ResourceAttrInfo*>();

	int tid = GetTypeId();

	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("GetAttrInfoList", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling GetAttrInfoList (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling GetAttrInfoList (no result)");
	} else {
		SV *resultSV = POPs;
		if (SvTYPE(SvRV(resultSV)) == SVt_PVAV) {
			AV *a = (AV*)SvRV(resultSV);
			SV *sv;
			while ((sv = av_shift(a)) != &PL_sv_undef) {
				if (SvTYPE(SvRV(sv)) == SVt_PVAV) {
					AV *item = (AV*)SvRV(sv);
					SV *second = av_pop(item);
					SV *first = av_pop(item);
					if (SvPOK(first) && SvPOK(second)) {
						ResourceAttrInfoP<PerlResource> *ai = new ResourceAttrInfoP<PerlResource>(tid);
						STRLEN len;
						char *s = SvPV(first, len);
						char *name = strndup(s, len);
						s = SvPV(second, len);
						if (!strcmp(s, "STRING")) {
							ai->InitString(name, &PerlResource::GetString, &PerlResource::SetString);
						} else if (!strcmp(s, "INT")) {
							ai->InitInt(name, &PerlResource::GetInt, &PerlResource::SetInt);
						} else if (!strcmp(s, "LONG")) {
							ai->InitLong(name, &PerlResource::GetLong, &PerlResource::SetLong);
						} else if (!strcmp(s, "IP")) {
							ai->InitIpAddr(name, &PerlResource::GetIpAddr, &PerlResource::SetIpAddr);
						} else if (!strcmp(s, "ARRAY_STRING")) {
							ai->InitArrayString(name, &PerlResource::GetArrayString, &PerlResource::SetArrayString, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_INT")) {
							ai->InitArrayInt(name, &PerlResource::GetArrayInt, &PerlResource::SetArrayInt, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_LONG")) {
							ai->InitArrayLong(name, &PerlResource::GetArrayLong, &PerlResource::SetArrayLong, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "ARRAY_IP")) {
							ai->InitArrayIpAddr(name, &PerlResource::GetArrayIpAddr, &PerlResource::SetArrayIpAddr, &PerlResource::Clear, &PerlResource::Count);
						} else if (!strcmp(s, "HASH_STRING")) {
							ai->InitHashString(name, &PerlResource::GetHashString, &PerlResource::SetHashString, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::StringValues);
						} else if (!strcmp(s, "HASH_INT")) {
							ai->InitHashInt(name, &PerlResource::GetHashInt, &PerlResource::SetHashInt, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::IntValues);
						} else if (!strcmp(s, "HASH_LONG")) {
							ai->InitHashLong(name, &PerlResource::GetHashLong, &PerlResource::SetHashLong, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::LongValues);
						} else if (!strcmp(s, "HASH_IP")) {
							ai->InitHashIpAddr(name, &PerlResource::GetHashIpAddr, &PerlResource::SetHashIpAddr, &PerlResource::Clear, &PerlResource::DeleteItem, &PerlResource::Count, &PerlResource::Keys, &PerlResource::IpAddrValues);
						} else {
							LOG4CXX_ERROR(logger, "Unknown attribute type: " << s);
						}
						free(name);
						result->push_back(ai);
					} else {
						LOG4CXX_ERROR(logger, "Invalid array item in GetAttrInfoList");
					}
				} else {
					LOG4CXX_ERROR(logger, "Invalid array item in GetAttrInfoList (expected reference to array of arrays)");
				}
			}
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetAttrInfoList (expected array refrence)");
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	return result;
}

int PerlResource::GetTypeId() {
	int result = -1;
	typeLock.Lock();
	if (typeIdSet) {
		result = typeId;
		typeLock.Unlock();
		return result;
	}

	// call Perl
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("GetTypeId", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling GetTypeId (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling GetTypeId (no result)");
	} else {
		SV *sv = POPs;
		if (SvIOK(sv)) {
			result = (int)SvIV(sv);
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetTypeId (invalid result type)");
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	if (result != -1) {
		typeId = result;
		typeIdSet = true;
	}

	typeLock.Unlock();
	return result;
}

const char *PerlResource::GetTypeString(bool terse) {
	char *result = NULL;
	typeLock.Lock();
	if (terse && typeStringShortSet) {
		result = typeStringShort;
		typeLock.Unlock();
		return result;
	} else if (!terse && typeStringSet) {
		result = typeString;
		typeLock.Unlock();
		return result;
	}

	// call Perl
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	int count;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSViv(terse ? 1 : 0)));
        PUTBACK;
	count = call_method("GetTypeString", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling GetTypeString (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling GetTypeString (no result)");
	} else {
		SV *sv = POPs;
		if (SvPOK(sv)) {
			STRLEN len;
			char *s = SvPV(sv, len);
			result = strndup(s, len);
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetTypeString (invalid result type)");
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	if (result) {
		if (!terse) {
			typeString = result;
			typeStringSet = true;
		} else {
			typeStringShort = result;
			typeStringShortSet = true;
		}
	}
	typeLock.Unlock();
	return result;
}
#endif

int PerlResource::GetSize() {
	int result = 0;

	// call Perl
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("GetSize", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling GetSize (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling GetSize (no result)");
	} else {
		SV *sv = POPs;
		if (SvIOK(sv)) {
			result = (int)SvIV(sv);
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetSize (invalid result type)");
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	return result;
}

string PerlResource::ToString(Object::LogLevel logLevel) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	string result;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	int count = call_method("ToString", G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling ToString (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling ToString (no result)");
	} else {
		SV *sv = POPs;
		if (SvPOK(sv)) {
			STRLEN len;
			result = SvPV(sv, len);
		} else {
			LOG4CXX_ERROR(logger, "Error calling ToString (invalid result type)");
		}
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
	perl->GetPerl()->SetContext(old);
	perl->Unlock();

	return result;
}

SV *PerlResource::GetValue(const char *prefix, const char *name) {
	SV *result = NULL;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "%s_%s", prefix, name);
	int count = call_method(method, G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling " << prefix << "_" << name << " (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling " << prefix << "_" << name << " (no result)");
	} else {
		result = SvREFCNT_inc(POPs);
	}
	PUTBACK;
	FREETMPS;
	LEAVE;

	return result;
}

SV *PerlResource::GetArrayValue(const char *name, int index) {
	SV *result = NULL;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSViv(index)));
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "Get_%s", name);
	int count = call_method(method, G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling Get_" << name << " (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling Get_" << name << " (no result)");
	} else {
		result = SvREFCNT_inc(POPs);
	}
	PUTBACK;
	FREETMPS;
	LEAVE;

	return result;
}

SV *PerlResource::GetHashValue(const char *name, const string &index) {
	SV *result = NULL;
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(index.data(), index.length())));
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "Get_%s", name);
	int count = call_method(method, G_SCALAR|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV)) {
		LOG4CXX_ERROR(logger, "Error calling Get_" << name << " (" << SvPV_nolen(ERRSV) << ")");
	} else if (count != 1) {
		LOG4CXX_ERROR(logger, "Error calling Get_" << name << " (no result)");
	} else {
		result = SvREFCNT_inc(POPs);
	}
	PUTBACK;
	FREETMPS;
	LEAVE;

	return result;
}

void PerlResource::SetValue(const char *name, SV *sv) {
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(sv));
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "Set_%s", name);
	call_method(method, G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG4CXX_ERROR(logger, "Error calling Set_" << name << " (" << SvPV_nolen(ERRSV) << ")");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

void PerlResource::SetArrayValue(const char *name, int index, SV *sv) {
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSViv(index)));
        XPUSHs(sv_2mortal(sv));
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "Set_%s", name);
	call_method(method, G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG4CXX_ERROR(logger, "Error calling Set_" << name << " (" << SvPV_nolen(ERRSV) << ")");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

void PerlResource::SetHashValue(const char *name, const string &index, SV *sv) {
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        XPUSHs(sv_2mortal(newSVpv(index.data(), index.length())));
        XPUSHs(sv_2mortal(sv));
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "Set_%s", name);
	call_method(method, G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG4CXX_ERROR(logger, "Error calling Set_" << name << " (" << SvPV_nolen(ERRSV) << ")");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

long PerlResource::GetLong(const string &name) {
	long result = 0;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("Get", name.c_str());
	if (SvIOK(sv)) {
		result = SvIV(sv);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetLong (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

int32_t PerlResource::GetInt32(const string &name) {
	return PerlResource::GetLong(name);
}

uint32_t PerlResource::GetUInt32(const string &name) {
	return PerlResource::GetLong(name);
}

int64_t PerlResource::GetInt64(const string &name) {
	return PerlResource::GetLong(name);
}

uint64_t PerlResource::GetUInt64(const string &name) {
	return PerlResource::GetLong(name);
}

double PerlResource::GetDouble(const string &name) {
	double result = 0;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("Get", name.c_str());
	if (SvNOK(sv)) {
		result = SvNV(sv);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetDouble (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

string PerlResource::GetString(const string &name) {
	string result;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("Get", name.c_str());
	if (SvPOK(sv)) {
		STRLEN len;
		result = SvPV(sv, len);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetString (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

IpAddr PerlResource::GetIpAddr(const string &name) {
	IpAddr result;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("Get", name.c_str());
	if (SvIOK(sv)) {
		void *r = NULL;
		if (perl->GetPerl()->ConvertPtr(sv, &r, "IpAddr *", 0x01) >= 0) {
			IpAddr *addr = reinterpret_cast<IpAddr*>(r);
			result = *addr;
			delete addr;
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetIpAddr (cannot covert result to IpAddr*)");
		}
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetIpAddr (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

long PerlResource::GetArrayLong(const string &name, int index) {
	long result = 0;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetArrayValue(name.c_str(), index);
	if (SvIOK(sv)) {
		result = SvIV(sv);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetArrayLong (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

int32_t PerlResource::GetArrayInt32(const string &name, int index) {
	return GetArrayLong(name, index);
}

uint32_t PerlResource::GetArrayUInt32(const string &name, int index) {
	return GetArrayLong(name, index);
}

int64_t PerlResource::GetArrayInt64(const string &name, int index) {
	return GetArrayLong(name, index);
}

uint64_t PerlResource::GetArrayUInt64(const string &name, int index) {
	return GetArrayLong(name, index);
}

double PerlResource::GetArrayDouble(const string &name, int index) {
	double result = 0;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetArrayValue(name.c_str(), index);
	if (SvNOK(sv)) {
		result = SvNV(sv);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetArrayDouble (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

string PerlResource::GetArrayString(const string &name, int index) {
	string result;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetArrayValue(name.c_str(), index);
	if (SvPOK(sv)) {
		STRLEN len;
		result = SvPV(sv, len);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetArrayString (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

IpAddr PerlResource::GetArrayIpAddr(const string &name, int index) {
	IpAddr result;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetArrayValue(name.c_str(), index);
	if (SvIOK(sv)) {
		void *r = NULL;
		if (perl->GetPerl()->ConvertPtr(sv, &r, "IpAddr *", 0x01) >= 0) {
			IpAddr *addr = reinterpret_cast<IpAddr*>(r);
			result = *addr;
			delete addr;
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetArrayIpAddr (cannot covert result to IpAddr*)");
		}
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetArrayIpAddr (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

long PerlResource::GetHashLong(const string &name, const string &index) {
	long result = 0;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetHashValue(name.c_str(), index);
	if (SvIOK(sv)) {
		result = SvIV(sv);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetHashLong (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

int32_t PerlResource::GetHashInt32(const string &name, const string &index) {
	return GetHashLong(name, index);
}

uint32_t PerlResource::GetHashUInt32(const string &name, const string &index) {
	return GetHashLong(name, index);
}

int64_t PerlResource::GetHashInt64(const string &name, const string &index) {
	return GetHashLong(name, index);
}

uint64_t PerlResource::GetHashUInt64(const string &name, const string &index) {
	return GetHashLong(name, index);
}

double PerlResource::GetHashDouble(const string &name, const string &index) {
	double result = 0;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetHashValue(name.c_str(), index);
	if (SvNOK(sv)) {
		result = SvNV(sv);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetHashDouble (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

string PerlResource::GetHashString(const string &name, const string &index) {
	string result;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetHashValue(name.c_str(), index);
	if (SvPOK(sv)) {
		STRLEN len;
		result = SvPV(sv, len);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetHashString (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

IpAddr PerlResource::GetHashIpAddr(const string &name, const string &index) {
	IpAddr result;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetHashValue(name.c_str(), index);
	if (SvIOK(sv)) {
		void *r = NULL;
		if (perl->GetPerl()->ConvertPtr(sv, &r, "IpAddr *", 0x01) >= 0) {
			IpAddr *addr = reinterpret_cast<IpAddr*>(r);
			result = *addr;
			delete addr;
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetHashIpAddr (cannot covert result to IpAddr*)");
		}
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetIpAddr (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

void PerlResource::SetLong(const string &name, long value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSViv(value);
	SetValue(name.c_str(), sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetInt32(const string &name, int32_t value) {
	SetLong(name, value);
}

void PerlResource::SetUInt32(const string &name, uint32_t value) {
	SetLong(name, value);
}

void PerlResource::SetInt64(const string &name, int64_t value) {
	SetLong(name, value);
}

void PerlResource::SetUInt64(const string &name, uint64_t value) {
	SetLong(name, value);
}

void PerlResource::SetDouble(const string &name, double value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSVnv(value);
	SetValue(name.c_str(), sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetString(const string &name, const string &value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSVpv(value.c_str(), 0);
	SetValue(name.c_str(), sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetIpAddr(const string &name, IpAddr &value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = perl->GetPerl()->NewPointerObj(const_cast<void*>(static_cast<const void*>(&value)), "IpAddr *", 0x01);
	if (!sv)
		return;
	SetValue(name.c_str(), sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetArrayLong(const string &name, int index, long value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSViv(value);
	SetArrayValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetArrayInt32(const string &name, int index, int32_t value) {
	SetArrayLong(name, index, value);
}

void PerlResource::SetArrayUInt32(const string &name, int index, uint32_t value) {
	SetArrayLong(name, index, value);
}

void PerlResource::SetArrayInt64(const string &name, int index, int64_t value) {
	SetArrayLong(name, index, value);
}

void PerlResource::SetArrayDouble(const string &name, int index, double value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSVnv(value);
	SetArrayValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetArrayUInt64(const string &name, int index, uint64_t value) {
	SetArrayLong(name, index, value);
}

void PerlResource::SetArrayString(const string &name, int index, const string &value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSVpv(value.data(), value.length());
	SetArrayValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetArrayIpAddr(const string &name, int index, IpAddr &value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = perl->GetPerl()->NewPointerObj(const_cast<void*>(static_cast<const void*>(&value)), "IpAddr *", 0x01);
	if (!sv)
		return;
	SetArrayValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetHashLong(const string &name, const string &index, long value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSViv(value);
	SetHashValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetHashInt32(const string &name, const string &index, int32_t value) {
	SetHashLong(name, index, value);
}

void PerlResource::SetHashUInt32(const string &name, const string &index, uint32_t value) {
	SetHashLong(name, index, value);
}

void PerlResource::SetHashInt64(const string &name, const string &index, int64_t value) {
	SetHashLong(name, index, value);
}

void PerlResource::SetHashUInt64(const string &name, const string &index, uint64_t value) {
	SetHashLong(name, index, value);
}

void PerlResource::SetHashDouble(const string &name, const string &index, double value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSVnv(value);
	SetHashValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetHashString(const string &name, const string &index, const string &value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = newSVpv(value.data(), value.length());
	SetHashValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::SetHashIpAddr(const string &name, const string &index, IpAddr &value) {
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = perl->GetPerl()->NewPointerObj(const_cast<void*>(static_cast<const void*>(&value)), "IpAddr *", 0x01);
	if (!sv)
		return;
	SetHashValue(name.c_str(), index, sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
}

void PerlResource::Clear(const string &name) {
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
        XPUSHs(ref);
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "Clear%s", name.c_str());
	call_method(method, G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG4CXX_ERROR(logger, "Error calling Clear" << name << " (" << SvPV_nolen(ERRSV) << ")");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

int PerlResource::Count(const string &name) {
	int result = 0;
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("Count", name.c_str());
	if (SvIOK(sv)) {
		result = SvIV(sv);
	} else {
		LOG4CXX_ERROR(logger, "Error calling GetInt (invalid result type)");
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

void PerlResource::DeleteItem(const string &name, const string &index) {
	dSP;
	ENTER;
	SAVETMPS;
        PUSHMARK(SP);
	XPUSHs(newSVpv(index.data(), index.length()));
        XPUSHs(ref);
        PUTBACK;
	char method[100];
	snprintf(method, sizeof(method), "DeleteItem%s", name.c_str());
	call_method(method, G_DISCARD|G_EVAL);
	SPAGAIN;
	if (SvTRUE(ERRSV))
		LOG4CXX_ERROR(logger, "Error calling DeleteItem" << name << " (" << SvPV_nolen(ERRSV) << ")");
	PUTBACK;
	FREETMPS;
	LEAVE;
}

vector<string> *PerlResource::Keys(const string &name) {
	vector<string> *result = new vector<string>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetKeys%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvPOK(i)) {
			STRLEN len;
			char *s = SvPV(i, len);
			string str(s, len);
			result->push_back(str);
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetKeys" << name << " (invalid array item type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

vector<int32_t> *PerlResource::Int32Values(const string &name) {
	vector<int32_t> *result = new vector<int32_t>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetValues%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvIOK(i)) {
			result->push_back(SvIV(i));
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (invalid array item type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

vector<uint32_t> *PerlResource::UInt32Values(const string &name) {
	vector<uint32_t> *result = new vector<uint32_t>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetValues%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvIOK(i)) {
			result->push_back(SvIV(i));
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (invalid array item type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

vector<int64_t> *PerlResource::Int64Values(const string &name) {
	vector<int64_t> *result = new vector<int64_t>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetValues%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvIOK(i)) {
			result->push_back(SvIV(i));
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (invalid array item type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

vector<uint64_t> *PerlResource::UInt64Values(const string &name) {
	vector<uint64_t> *result = new vector<uint64_t>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetValues%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvIOK(i)) {
			result->push_back(SvIV(i));
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (invalid array item type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

vector<double> *PerlResource::DoubleValues(const string &name) {
	vector<double> *result = new vector<double>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetValues%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvNOK(i)) {
			result->push_back(SvNV(i));
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (invalid array item type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

vector<string> *PerlResource::StringValues(const string &name) {
	vector<string> *result = new vector<string>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetValues%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvPOK(i)) {
			STRLEN len;
			char *s = SvPV(i, len);
			string str(s, len);
			result->push_back(str);
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (invalid array item type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}

vector<IpAddr> *PerlResource::IpAddrValues(const string &name) {
	vector<IpAddr> *result = new vector<IpAddr>();
	perl->Lock();
	void *old = perl->GetPerl()->GetContext();
	perl->GetPerl()->SetContext();
	SV *sv = GetValue("GetValues%s", name.c_str());
	AV *a = (AV*)SvRV(sv);
	SV *i;
	while ((i = av_shift(a)) != &PL_sv_undef) {
		if (SvIOK(i)) {
			void *r = NULL;
			if (perl->GetPerl()->ConvertPtr(sv, &r, "IpAddr *", 0x01) >= 0) {
				IpAddr *addr = reinterpret_cast<IpAddr*>(r);
				result->push_back(*addr);
				delete addr;
			} else {
				LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (cannot covert result to IpAddr*)");
			}
		} else {
			LOG4CXX_ERROR(logger, "Error calling GetValues" << name << " (invalid result type)");
		}
	}
	SvREFCNT_dec(sv);
	perl->GetPerl()->SetContext(old);
	perl->Unlock();
	return result;
}
