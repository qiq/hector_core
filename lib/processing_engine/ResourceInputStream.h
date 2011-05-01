/**
 * Wrapping PB CodedInputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_H_

#include <config.h>

#include <limits>
#include <string>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <log4cxx/logger.h>
#include "Resource.h"

namespace google {
	namespace protobuf {
		namespace io {
			class CodedInputStream;
		}
	}
}


class ResourceInputStream {
public:
	ResourceInputStream(int fd);
	ResourceInputStream(std::string &s);
	~ResourceInputStream();

	// generic methods
	bool ReadRaw(void *buffer, int size);
	bool ReadString(std::string *buffer, int size);
	bool ReadLittleEndian32(uint32_t *value);
	bool ReadVarint32(uint32_t *value);
	bool ReadLittleEndian64(uint64_t *value);
	bool ReadVarint64(uint64_t *value);
	bool Skip(int count);
	int ByteCount();

	// protobuf methods
	google::protobuf::io::CodedInputStream::Limit PushLimit(int byte_limit);
	void PopLimit(google::protobuf::io::CodedInputStream::Limit limit);
	google::protobuf::io::CodedInputStream *GetCodedInputStream();

private:
	google::protobuf::io::ZeroCopyInputStream *file;
	google::protobuf::io::CodedInputStream *stream;
};

inline ResourceInputStream::ResourceInputStream(int fd) {
	file = new google::protobuf::io::FileInputStream(fd);
	stream = new google::protobuf::io::CodedInputStream(file);
}

inline ResourceInputStream::ResourceInputStream(std::string &s) {
	file = new google::protobuf::io::ArrayInputStream(s.data(), s.size());
	stream = new google::protobuf::io::CodedInputStream(file);
}

inline ResourceInputStream::~ResourceInputStream() {
	delete stream;
	delete file;
}

inline bool ResourceInputStream::ReadRaw(void *buffer, int size) {
	return stream->ReadRaw(buffer, size);
}

inline bool ResourceInputStream::ReadString(std::string *buffer, int size) {
	return stream->ReadString(buffer, size);
}

inline bool ResourceInputStream::ReadLittleEndian32(uint32_t *value) {
	return stream->ReadLittleEndian32(value);
}

inline bool ResourceInputStream::ReadVarint32(uint32_t *value) {
	return stream->ReadVarint32(value);
}

inline bool ResourceInputStream::ReadLittleEndian64(uint64_t *value) {
	return stream->ReadLittleEndian64(value);
}

inline bool ResourceInputStream::ReadVarint64(uint64_t *value) {
	return stream->ReadVarint64(value);
}

inline bool ResourceInputStream::Skip(int count) {
	return stream->Skip(count);
}

inline int ResourceInputStream::ByteCount() {
	return file->ByteCount();
}

inline google::protobuf::io::CodedInputStream::Limit ResourceInputStream::PushLimit(int byte_limit) {
	return stream->PushLimit(byte_limit);
}

inline void ResourceInputStream::PopLimit(google::protobuf::io::CodedInputStream::Limit limit) {
	stream->PopLimit(limit);
}

inline google::protobuf::io::CodedInputStream *ResourceInputStream::GetCodedInputStream() {
	return stream;
}

#endif
