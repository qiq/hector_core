/**
 */

#include <assert.h>
#include <string.h>
#include "binaryheap.h"
#include "common.h"
#include "PerlResourceInterpreter.h"
#include "EmbeddedPerl.h"

using namespace std;

PerlResourceInterpreter::~PerlResourceInterpreter() {
	delete perl;
}

