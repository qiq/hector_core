/**
 * Wrapping PB CodedInputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_BINARY_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_BINARY_H_

#include <config.h>

#include <limits>
#include <string>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "ResourceInputStream.h"

namespace google {
	namespace protobuf {
		namespace io {
			class CodedInputStream;
		}
	}
}

class ResourceInputStreamBinary : public ResourceInputStream {
public:
	ResourceInputStreamBinary(int fd, bool gzip = false);
	ResourceInputStreamBinary(std::string &s, bool gzip = false);
	~ResourceInputStreamBinary();

	bool ReadString(std::string *buffer);
	bool ReadLittleEndian32(uint32_t *value);
	bool ReadVarint32(uint32_t *value);
	bool ReadLittleEndian64(uint64_t *value);
	bool ReadVarint64(uint64_t *value);
	bool ReadRaw(char *data, int size);
	bool Skip(int count);
	int ByteCount();
	void Refresh();

	bool ParseMessage(google::protobuf::Message &msg, uint32_t size = 0, bool skip = false);

private:
	bool compress;
	google::protobuf::io::ZeroCopyInputStream *file;
	google::protobuf::io::ZeroCopyInputStream *gzip;
	google::protobuf::io::CodedInputStream *stream;
};

inline ResourceInputStreamBinary::ResourceInputStreamBinary(int fd, bool compress) {
	this->compress = compress;
	file = new google::protobuf::io::FileInputStream(fd);
	if (compress) {
		gzip = new google::protobuf::io::GzipInputStream(file);
		stream = new google::protobuf::io::CodedInputStream(gzip);
	} else {
		gzip = NULL;
		stream = new google::protobuf::io::CodedInputStream(file);
	}
	stream->SetTotalBytesLimit(512*1024*1024, 256*1024*1024);
}

inline ResourceInputStreamBinary::ResourceInputStreamBinary(std::string &s, bool compress) {
	this->compress = compress;
	file = new google::protobuf::io::ArrayInputStream(s.data(), s.size());
	if (compress) {
		gzip = new google::protobuf::io::GzipInputStream(file);
		stream = new google::protobuf::io::CodedInputStream(gzip);
	} else {
		gzip = NULL;
		stream = new google::protobuf::io::CodedInputStream(file);
	}
	stream->SetTotalBytesLimit(512*1024*1024, 256*1024*1024);
}

inline ResourceInputStreamBinary::~ResourceInputStreamBinary() {
	delete stream;
	delete gzip;
	delete file;
}

inline bool ResourceInputStreamBinary::ReadString(std::string *buffer) {
	uint32_t size;
	if (!stream->ReadVarint32(&size))
		return false;
	return stream->ReadString(buffer, size);
}

inline bool ResourceInputStreamBinary::ReadLittleEndian32(uint32_t *value) {
	return stream->ReadLittleEndian32(value);
}

inline bool ResourceInputStreamBinary::ReadVarint32(uint32_t *value) {
	return stream->ReadVarint32(value);
}

inline bool ResourceInputStreamBinary::ReadLittleEndian64(uint64_t *value) {
	return stream->ReadLittleEndian64(value);
}

inline bool ResourceInputStreamBinary::ReadVarint64(uint64_t *value) {
	return stream->ReadVarint64(value);
}

inline bool ResourceInputStreamBinary::ReadRaw(char *data, int size) {
	return stream->ReadRaw(data, size);
}

inline bool ResourceInputStreamBinary::Skip(int count) {
	return stream->Skip(count);
}

inline int ResourceInputStreamBinary::ByteCount() {
	return file->ByteCount();
}

inline void ResourceInputStreamBinary::Refresh() {
	// re-create CodedInputStream
	stream->~CodedInputStream();
	stream = compress ? new(stream) google::protobuf::io::CodedInputStream(gzip) : new(stream) google::protobuf::io::CodedInputStream(file);
	stream->SetTotalBytesLimit(512*1024*1024, 256*1024*1024);
}

inline bool ResourceInputStreamBinary::ParseMessage(google::protobuf::Message &msg, uint32_t size, bool skip) {
	bool result;
	if (!size) {
	        if (!stream->ReadVarint32(&size))
	                return false;
        }
	if (skip)
		return stream->Skip(size);
	google::protobuf::io::CodedInputStream::Limit l = stream->PushLimit(size);
	result = msg.ParseFromCodedStream(stream);
	stream->PopLimit(l);
	return result;
}

#endif
