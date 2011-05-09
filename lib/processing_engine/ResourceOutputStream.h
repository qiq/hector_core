/**
 * Wrapping PB CodedOutputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_H_

#include <config.h>

#include <limits>
#include <string>
#include <google/protobuf/message.h>
#include <log4cxx/logger.h>
#include "Resource.h"

class ResourceOutputStream {
public:
	ResourceOutputStream() {};
	virtual ~ResourceOutputStream() {};

	virtual void WriteString(const std::string &buffer) = 0;
	virtual void WriteLittleEndian32(uint32_t value) = 0;
	virtual void WriteVarint32(uint32_t value) = 0;
	virtual void WriteLittleEndian64(uint64_t value) = 0;
	virtual void WriteVarint64(uint64_t value) = 0;

	virtual void SerializeMessage(google::protobuf::Message &msg, bool saveSize = true) = 0;
};

#endif
