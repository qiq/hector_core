/**
 * Wrapping PB CodedOutputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_BINARY_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_BINARY_H_

#include <config.h>

#include <limits>
#include <string>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "ResourceOutputStream.h"

namespace google {
	namespace protobuf {
		namespace io {
			class CodedOutputStream;
		}
	}
}

class ResourceOutputStreamBinary : public ResourceOutputStream {
public:
	ResourceOutputStreamBinary(int fd, bool compress = false);
	ResourceOutputStreamBinary(std::string *s, bool compress = false);
	~ResourceOutputStreamBinary();

	void WriteString(const std::string &buffer);
	void WriteLittleEndian32(uint32_t value);
	void WriteVarint32(uint32_t value);
	void WriteLittleEndian64(uint64_t value);
	void WriteVarint64(uint64_t value);
	void WriteRaw(const char *data, int size);

	void SerializeMessage(google::protobuf::Message &msg, bool saveSize = true);

private:
	google::protobuf::io::ZeroCopyOutputStream *file;
	google::protobuf::io::ZeroCopyOutputStream *gzip;
	google::protobuf::io::CodedOutputStream *stream;
};

inline ResourceOutputStreamBinary::ResourceOutputStreamBinary(int fd, bool compress) {
	file = new google::protobuf::io::FileOutputStream(fd);
	if (compress) {
		gzip = new google::protobuf::io::GzipOutputStream(file);
		stream = new google::protobuf::io::CodedOutputStream(gzip);
	} else {
		gzip = NULL;
		stream = new google::protobuf::io::CodedOutputStream(file);
	}
}

inline ResourceOutputStreamBinary::ResourceOutputStreamBinary(std::string *s, bool compress) {
	file = new google::protobuf::io::StringOutputStream(s);
	if (compress) {
		gzip = new google::protobuf::io::GzipOutputStream(file);
		stream = new google::protobuf::io::CodedOutputStream(gzip);
	} else {
		gzip = NULL;
		stream = new google::protobuf::io::CodedOutputStream(file);
	}
}

inline ResourceOutputStreamBinary::~ResourceOutputStreamBinary() {
	delete stream;
	delete gzip;
	delete file;
}

inline void ResourceOutputStreamBinary::WriteString(const std::string &buffer) {
	uint32_t size = buffer.length();
	stream->WriteVarint32(size);
	return stream->WriteString(buffer);
}

inline void ResourceOutputStreamBinary::WriteLittleEndian32(uint32_t value) {
	stream->WriteLittleEndian32(value);
}

inline void ResourceOutputStreamBinary::WriteVarint32(uint32_t value) {
	stream->WriteVarint32(value);
}

inline void ResourceOutputStreamBinary::WriteLittleEndian64(uint64_t value) {
	stream->WriteLittleEndian64(value);
}

inline void ResourceOutputStreamBinary::WriteVarint64(uint64_t value) {
	stream->WriteVarint64(value);
}

inline void ResourceOutputStreamBinary::WriteRaw(const char *data, int size) {
	stream->WriteRaw(data, size);
}

inline void ResourceOutputStreamBinary::SerializeMessage(google::protobuf::Message &msg, bool saveSize) {
	if (saveSize) {
		stream->WriteVarint32(msg.ByteSize());
		msg.SerializeWithCachedSizes(stream);
	} else {
		msg.SerializeToCodedStream(stream);
	}
}

#endif
