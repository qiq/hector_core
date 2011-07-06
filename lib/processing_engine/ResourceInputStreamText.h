/**
 * Wrapping PB CodedInputStream, so that it is simply usable by both native and
 * SWIG modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_TEXT_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_INPUT_STREAM_TEXT_H_

#include <config.h>
#include <stdio.h>
#include <limits>
#include <iostream>
#include <sstream>
#include <string>
#include <log4cxx/logger.h>
#include <google/protobuf/text_format.h>
#include "Resource.h"
#include "ResourceInputStream.h"

class ResourceInputStreamText : public ResourceInputStream {
public:
	ResourceInputStreamText(std::istream *is);
	ResourceInputStreamText(std::string &s);
	~ResourceInputStreamText();

	bool ReadString(std::string *buffer);
	bool ReadLittleEndian32(uint32_t *value);
	bool ReadVarint32(uint32_t *value);
	bool ReadLittleEndian64(uint64_t *value);
	bool ReadVarint64(uint64_t *value);
	bool ReadRaw(char *data, int size);
	bool Skip(int count);
	int ByteCount();

	bool ParseMessage(google::protobuf::Message &msg, uint32_t size = 0, bool skip = false);

private:
	std::istream *is;
	std::istringstream *strstr;

	google::protobuf::TextFormat::Parser parser;
};

inline ResourceInputStreamText::ResourceInputStreamText(std::istream *is) {
	this->is = is;
	strstr = NULL;
}

inline ResourceInputStreamText::ResourceInputStreamText(std::string &s) {
	is = strstr = new std::istringstream(s, std::istringstream::in);
}

inline ResourceInputStreamText::~ResourceInputStreamText() {
	if (strstr)
		delete strstr;
}

inline bool ResourceInputStreamText::ReadString(std::string *value) {
	getline(*is, *value, '\n');
	if (is->fail())
		return false;

	// replace \n by real newlines
	replace_all(*value, "\\n", "\n");
	// replace all double-backslashes by backslashes
	replace_all(*value, "\\\\", "\\");
	
	return true;
}

inline bool ResourceInputStreamText::ReadLittleEndian32(uint32_t *value) {
	*is >> *value;
	if (is->fail())
		return false;
	std::string s;
	getline(*is, s, '\n');
	return true;
}

inline bool ResourceInputStreamText::ReadVarint32(uint32_t *value) {
	*is >> *value;
	if (is->fail())
		return false;
	std::string s;
	getline(*is, s, '\n');
	return true;
}

inline bool ResourceInputStreamText::ReadLittleEndian64(uint64_t *value) {
	*is >> *value;
	if (is->fail())
		return false;
	std::string s;
	getline(*is, s, '\n');
	return true;
}

inline bool ResourceInputStreamText::ReadVarint64(uint64_t *value) {
	*is >> *value;
	if (is->fail())
		return false;
	std::string s;
	getline(*is, s, '\n');
	return true;
}

inline bool ResourceInputStreamText::ReadRaw(char *data, int size) {
	is->read(data, size);
	if (is->bad())
		return false;
	std::string s;
	getline(*is, s, '\n');
	return true;
}

inline bool ResourceInputStreamText::Skip(int count) {
	is->ignore(count);
	if (is->fail())
		return false;
	std::string s;
	getline(*is, s, '\n');
	return true;
}

inline int ResourceInputStreamText::ByteCount() {
	return is->tellg();
}

inline bool ResourceInputStreamText::ParseMessage(google::protobuf::Message &msg, uint32_t size, bool skip) {
	std::string s;
	getline(*is, s, '\n');
	if (is->fail())
		return false;
	if (skip)
		return true;
	return parser.ParseFromString(s, &msg);
}

#endif
