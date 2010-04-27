
#include "TestProtobufResourceWrapper.h"
#include "TestProtobufResource.h"

using namespace std;

// the class factories

extern "C" Resource* create() {
	return (Resource*)new TestProtobufResource();
}
