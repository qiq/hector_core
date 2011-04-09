/**
 * N.B.: Only one thread can use Perl interpreter at the time, so we had to lock it for write.
 */

#include <assert.h>
#include <string.h>
#include "heapplus.h"
#include "common.h"
#include "PerlInterpreters.h"
#include "EmbeddedPerl.h"

using namespace std;

//TODO: finish & destroy interpreters: called from ResourceRegistry
log4cxx::LoggerPtr PerlInterpreters::logger(log4cxx::Logger::getLogger("PerlInterpreters"));

#define MAX_PERL_INTERPRETERS 100

PerlInterpreters::~PerlInterpreters() {
	for (vector<PerlResourceInterpreter*>::iterator iter = interpreters.begin(); iter != interpreters.end(); ++iter)
		delete *iter;
}

PerlResourceInterpreter *PerlInterpreters::AcquirePerl() {
	lock.Lock();
	PerlResourceInterpreter *pi;
	if (interpreters.size() < MAX_PERL_INTERPRETERS) {
		// create new Perl interpreter
		pi = new PerlResourceInterpreter(new EmbeddedPerl());
		if (!pi->GetPerl()->Init()) {
			lock.Unlock();
			return NULL;
		}
		interpreters.push_back(pi);
		push_heap(interpreters.begin(), interpreters.end(), PerlResourceInterpreter_Compare(), PerlResourceInterpreter_Update());
	} else {
		pi = interpreters[0];
	}
	pi->IncCount();
	update_heap_pos(interpreters.begin(), interpreters.end(), interpreters.begin() + pi->GetHeapPosition(), PerlResourceInterpreter_Compare(), PerlResourceInterpreter_Update());
	lock.Unlock();
	return pi;
}

void PerlInterpreters::ReleasePerl(PerlResourceInterpreter *pi) {
	lock.Lock();
	pi->DecCount();
	update_heap_pos(interpreters.begin(), interpreters.end(), interpreters.begin() + pi->GetHeapPosition(), PerlResourceInterpreter_Compare(), PerlResourceInterpreter_Update());
	assert(pi->GetCount() >= 0);
	lock.Unlock();
}
