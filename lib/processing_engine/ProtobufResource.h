/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_PROTOBUF_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_PROTOBUF_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include "Resource.h"
#include "Object.h"	// for LOG_*

class ProtobufResource : public Resource {
public:
	ProtobufResource() {};
	virtual ~ProtobufResource() {};
	bool IsProtobufResource();

	// save and restore resource
	virtual int GetSerializedSize() = 0;
	virtual bool SerializeWithCachedSize(google::protobuf::io::CodedOutputStream *output) = 0;
	virtual bool Deserialize(google::protobuf::io::CodedInputStream *input) = 0;

private:
	static log4cxx::LoggerPtr logger;

};

inline bool ProtobufResource::IsProtobufResource() {
	return true;
}

#endif
