#ifndef _CONNECTION_H_
#define _CONNECTION_H_

class Connection {
public:
	Connection() {};
	~Connection() {};

	virtual bool Init() { return true; };
	virtual int ReadWrite(const char *writeBuffer, int writeBufferLen, char *readBuffer, int readBufferLen, bool waitForRead = true) { return 0; };
};

#endif
