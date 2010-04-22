
#include <stdio.h>
#include "TestResource.h"

using namespace std;

log4cxx::LoggerPtr TestResource::logger(log4cxx::Logger::getLogger("lib.processing_engine.TestResource"));

TestResource::TestResource() {
	id = 0;
	status = 0;
}

Resource *TestResource::Clone() {
	return new TestResource(*this);
}

int TestResource::getSize() {
	return 1;
}

string *TestResource::serialize() {
	string *strcopy = new string(str);
	size_t idx;
	while ((idx = strcopy->find('\n')) != string::npos) {
		strcopy->replace(idx, 1, " ");
	}
	char s[1024];
	snprintf(s, sizeof(s), "%d\n%d\n%s\n", id, status, strcopy->c_str());
	delete strcopy;
	return new string(s);
}

bool TestResource::deserialize(string *s) {
	char buf[1024];
	if (sscanf(s->c_str(), "%d\n%d\n%1023s\n", &id, &status, (char*)&buf) != 3) {
		LOG4CXX_ERROR(logger, "Cannot deserialize TestResource: " << s);
		return false;
	}
	str.assign(buf);
	return true;
}
