/**
 * Wrapping PB CodedOutputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_TEXT_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_OUTPUT_STREAM_TEXT_H_

#include <config.h>

#include <limits>
#include <string>
#include <google/protobuf/text_format.h>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "ResourceOutputStream.h"

class ResourceOutputStreamText : public ResourceOutputStream {
public:
	ResourceOutputStreamText(std::ostream *os);
	~ResourceOutputStreamText() {};

	void WriteString(const std::string &buffer);
	void WriteLittleEndian32(uint32_t value);
	void WriteVarint32(uint32_t value);
	void WriteLittleEndian64(uint64_t value);
	void WriteVarint64(uint64_t value);
	void WriteRaw(const char *data, int size);

	void SerializeMessage(google::protobuf::Message &msg, bool saveSize = true);

private:
	std::ostream *os;

	google::protobuf::TextFormat::Printer printer;
};

inline ResourceOutputStreamText::ResourceOutputStreamText(std::ostream *os) {
	this->os = os;
	printer.SetSingleLineMode(true);
	printer.SetUseShortRepeatedPrimitives(true);
}

inline void ResourceOutputStreamText::WriteString(const std::string &buffer) {
	std::string s(buffer);
	// replace all backslashes by double-backslashes
	replace_all(s, "\\", "\\\\");
	// replace newlines by \n
	replace_all(s, "\n", "\\n");
	*os << s << "\n";
}

inline void ResourceOutputStreamText::WriteLittleEndian32(uint32_t value) {
	*os << value << "\n";
}

inline void ResourceOutputStreamText::WriteVarint32(uint32_t value) {
	*os << value << "\n";
}

inline void ResourceOutputStreamText::WriteLittleEndian64(uint64_t value) {
	*os << value << "\n";
}

inline void ResourceOutputStreamText::WriteVarint64(uint64_t value) {
	*os << value << "\n";
}

inline void ResourceOutputStreamText::WriteRaw(const char *data, int size) {
	os->write(data, size);
	*os << "\n";
}

inline void ResourceOutputStreamText::SerializeMessage(google::protobuf::Message &msg, bool saveSize) {
	std::string s;
	printer.PrintToString(msg, &s);
	*os << s << "\n";
}

#endif
