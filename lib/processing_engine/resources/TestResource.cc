
#include "TestResource.h"
#include "TestResource.pb.h"

using namespace std;

log4cxx::LoggerPtr TestResource::logger(log4cxx::Logger::getLogger("lib.processing_engine.TestResource"));

TestResource::TestResource() {
	// check library vs header file versions
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

Resource *TestResource::Clone() {
	return new TestResource(*this);
}

int TestResource::getSize() {
	return 1; //FIXME
}

string *TestResource::serialize() {
	string *result = new string();
	if (!r.SerializeToString(result)) {
		LOG4CXX_ERROR(logger, "Cannot serialize Resource");
		delete result;
		return NULL;
	}
	return result;
}

bool TestResource::deserialize(string *s) {
	r.ParseFromString(*s);
	return true;
}
