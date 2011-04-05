
#include "MarkerResourceWrapper.h"
#include "MarkerResource.h"

using namespace std;

// the class factories

extern "C" Resource* create() {
	return (Resource*)new MarkerResource();
}
