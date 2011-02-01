/**
 * Embedding Python, only one instance of Python interpreter is supported (as
 * it is problematic to have more than one Python instance running in the same
 * time due to per-process-GIL).
 */

#ifndef _LIB_PROCESSING_ENGINE_EMBEDDED_PYTHON_H_
#define _LIB_PROCESSING_ENGINE_EMBEDDED_PYTHON_H_

#include <config.h>
#include <cstdarg>
// prevent warnings from Python.h
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif
#include <Python.h>
#include <log4cxx/logger.h>
#include "PlainLock.h"

struct swig_type_info;

class EmbeddedPython {
public:
	EmbeddedPython();
	~EmbeddedPython();

	static EmbeddedPython *GetPython();

	PyGILState_STATE Lock();
	void Unlock(PyGILState_STATE gstate);
	PyObject *LoadModule(const char *module, const char *format, ...);
	PyObject *CallMethod(PyObject *module, const char *name, const char *format, ...);
	PyObject *CallMethod(PyObject *module, const char *name, const char *format, va_list args);
	void LogError();

	PyObject *NewPointerObj(void *ptr, const char *type, int flags);
	int ConvertPtr(PyObject *obj, void **ptr, const char *type, int flags);

private:
	static PlainLock lock;
	static EmbeddedPython *python;
	static swig_type_info *(*Python_TypeQuery)(const char *type);
	static PyObject *(*Python_NewPointerObj)(void *ptr, swig_type_info *type, int flags);
	static int (*Python_ConvertPtr)(PyObject *obj, void **ptr, swig_type_info *ty, int flags);
	static std::tr1::unordered_map<std::string, swig_type_info*> typeInfoCache;

	PyObject *_CallMethod(PyObject *module, const char *name, PyObject *args);

	static log4cxx::LoggerPtr logger;
};

inline PyGILState_STATE EmbeddedPython::Lock() {
	return PyGILState_Ensure();
}

inline void EmbeddedPython::Unlock(PyGILState_STATE gstate) {
	PyGILState_Release(gstate);
}

#endif
