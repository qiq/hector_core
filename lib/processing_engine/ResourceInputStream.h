/**
 * Wrapping PB CodedInputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_H_

#include <config.h>

#include <limits>
#include <string>
#include <google/protobuf/message.h>
#include <log4cxx/logger.h>
#include "Resource.h"

class ResourceInputStream {
public:
	ResourceInputStream() {};
	virtual ~ResourceInputStream() {};

	virtual bool ReadString(std::string *buffer) = 0;
	virtual bool ReadLittleEndian32(uint32_t *value) = 0;
	virtual bool ReadVarint32(uint32_t *value) = 0;
	virtual bool ReadLittleEndian64(uint64_t *value) = 0;
	virtual bool ReadVarint64(uint64_t *value) = 0;
	virtual bool ReadRaw(char *data, int size) = 0;
	virtual bool Skip(int count) = 0;
	virtual int ByteCount() = 0;

	virtual bool ParseMessage(google::protobuf::Message &msg, uint32_t size = 0, bool skip = false) = 0;
};

#endif
