/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <string>

class Resource {
private:
	int something;
public:
	Resource() { something = 1; };
	~Resource() {};
	//void serialize(FILE *f);
	//void deserialize(FILE *f);
//	bool operator==(Resource &r2) { return this->something == r2.something; }
};

#endif
