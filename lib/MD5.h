/*
 *	This file is in public domain (see md5.c).
 */

#ifndef _LIB_MD5_H
#define _LIB_MD5_H

#include <config.h>
#include <stdint.h>

class MD5 {
public:
	/** Initialize the MD5 hashing algorithm in @context. **/
	MD5();
	~MD5() {};

	/**
	 * Push another @len bytes of data from @buf to the MD5 hash
	 * represented by @context. You can call it multiple time on the same
	 * @context without reinitializing it and the result will be the same
	 * as if you concatenated all the data together and fed them here all at
	 * once.
	 */
	void Update(const char *buf, unsigned len);
	/**
	 * Call this after the last @md5_update(). It will terminate the
	 * algorithm and return a pointer to the result.
	 *
	 * Note that the data it points to are stored inside the @context, so
	 * if you use it to compute another hash or it ceases to exist, the
	 * pointer becomes invalid.
	 *
	 * To convert the hash to its usual hexadecimal representation, see
	 * <<string:mem_to_hex()>>.
	 */
	char *Final();
	/**
	 * This is the core routine of the MD5 algorithm. It takes 16 longwords of
	 * data in @in and transforms the hash in @buf according to them.
	 *
	 * You probably do not want to call this one directly.
	 */
	void Transform(uint32_t buf[4], const uint32_t in[16]);
	/**
	 * MD5 one-shot convenience method. It takes @length bytes from
	 * @buffer, creates the hash from them and returns it in @output.
	 *
	 * It is equivalent to this code:
	 *
	 *  md5_context c;
	 *  md5_init(&c);
	 *  md5_update(&c, buffer, length);
	 *  memcpy(outbuf, md5_final(&c), MD5_SIZE);
	 */
	static void HashBuffer(char *outbuf, const char *buffer, unsigned length);
private:
	uint32_t buf[4];
	uint32_t bits[2];
	char in[64];
};

#define MD5_HEX_SIZE 33 /** How many bytes a string buffer for MD5 in hexadecimal format should have. **/
#define MD5_SIZE 16 /** Number of bytes the MD5 hash takes in the binary form. **/

#endif
