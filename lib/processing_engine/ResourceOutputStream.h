/**
 * Wrapping PB CodedOutputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_H_

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
			class CodedOutputStream;
		}
	}
}


class ResourceOutputStream {
public:
	ResourceOutputStream(int fd);
	ResourceOutputStream(std::string *s);
	~ResourceOutputStream();

	// generic methods
	void WriteRaw(const void *buffer, int size);
	void WriteString(const std::string &buffer);
	void WriteLittleEndian32(uint32_t value);
	void WriteVarint32(uint32_t value);
	void WriteLittleEndian64(uint64_t value);
	void WriteVarint64(uint64_t value);
	bool Skip(int count);

	// protobuf methods
	google::protobuf::io::CodedOutputStream *GetCodedOutputStream();

private:
	google::protobuf::io::ZeroCopyOutputStream *file;
	google::protobuf::io::CodedOutputStream *stream;
};

inline ResourceOutputStream::ResourceOutputStream(int fd) {
	file = new google::protobuf::io::FileOutputStream(fd);
	stream = new google::protobuf::io::CodedOutputStream(file);
}

inline ResourceOutputStream::ResourceOutputStream(std::string *s) {
	file = new google::protobuf::io::StringOutputStream(s);
	stream = new google::protobuf::io::CodedOutputStream(file);
}

inline ResourceOutputStream::~ResourceOutputStream() {
	delete stream;
	delete file;
}

inline void ResourceOutputStream::WriteRaw(const void *buffer, int size) {
	stream->WriteRaw(buffer, size);
}

inline void ResourceOutputStream::WriteString(const std::string &buffer) {
	stream->WriteString(buffer);
}

inline void ResourceOutputStream::WriteLittleEndian32(uint32_t value) {
	stream->WriteLittleEndian32(value);
}

inline void ResourceOutputStream::WriteVarint32(uint32_t value) {
	stream->WriteVarint32(value);
}

inline void ResourceOutputStream::WriteLittleEndian64(uint64_t value) {
	stream->WriteLittleEndian64(value);
}

inline void ResourceOutputStream::WriteVarint64(uint64_t value) {
	stream->WriteVarint64(value);
}

inline bool ResourceOutputStream::Skip(int count) {
	return stream->Skip(count);
}

inline google::protobuf::io::CodedOutputStream *ResourceOutputStream::GetCodedOutputStream() {
	return stream;
}

#endif
