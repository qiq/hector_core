/**
 */

#include <assert.h>
#include <string.h>
#include "common.h"
#include "PerlResourceInterpreter.h"
#include "EmbeddedPerl.h"

using namespace std;

PerlResourceInterpreter::PerlResourceInterpreter(): count(0), position(0), perl(NULL) {
	lock = new PlainLock(true);
}

bool PerlResourceInterpreter::Init() {
	perl = new EmbeddedPerl();
	return perl->Init();
}

PerlResourceInterpreter::~PerlResourceInterpreter() {
	delete lock;
	delete perl;
}

