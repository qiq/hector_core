/**
 * 
 */

#ifndef _LIB_PROCESSING_ENGINE_PERL_RESOURCE_INTERPTERER_H_
#define _LIB_PROCESSING_ENGINE_PERL_RESOURCE_INTERPTERER_H_

#include <config.h>

#include "PlainLock.h"

class EmbeddedPerl;
class PerlResourceInterpreter {
public:
	PerlResourceInterpreter();
	~PerlResourceInterpreter();
	bool Init();

	void Lock();
	void Unlock();
	EmbeddedPerl *GetPerl();
	int GetCount() const;
	void IncCount();
	void DecCount();
	int GetHeapPosition();
	void SetHeapPosition(int position);

private:
	int count;
	int position;
	PlainLock *lock;		// recursive lock
	EmbeddedPerl *perl;
};

inline void PerlResourceInterpreter::Lock() {
	lock->Lock();
}

inline void PerlResourceInterpreter::Unlock() {
	lock->Unlock();
}

inline EmbeddedPerl *PerlResourceInterpreter::GetPerl() {
	return perl;
}

inline int PerlResourceInterpreter::GetCount() const {
	return count;
}

inline void PerlResourceInterpreter::IncCount() {
	count++;
}

inline void PerlResourceInterpreter::DecCount() {
	count--;
}

inline int PerlResourceInterpreter::GetHeapPosition() {
	return position;
}

inline void PerlResourceInterpreter::SetHeapPosition(int position) {
	this->position = position;
}

struct PerlResourceInterpreter_Compare {
	bool operator()(PerlResourceInterpreter const *a, PerlResourceInterpreter const *b) const {
		return a->GetCount() < b->GetCount();
	}
};

struct PerlResourceInterpreter_Update {
	void operator()(PerlResourceInterpreter *a, int pos) {
		a->SetHeapPosition(pos);
	}
};

#endif
