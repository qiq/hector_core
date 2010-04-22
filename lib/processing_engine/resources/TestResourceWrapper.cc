
#include "TestResourceWrapper.h"
#include "TestResource.h"

using namespace std;

// the class factories

extern "C" Resource* create() {
	return (Resource*)new TestResource();
}
