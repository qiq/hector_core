/**
 */

#include <Python.h>		// must be first due to warnings

#include <assert.h>
#include <string.h>
#include <traceback.h>		// Python
#include <frameobject.h>	// Python
#include "common.h"
#include "EmbeddedPython.h"
#include "LibraryLoader.h"

using namespace std;

PlainLock EmbeddedPython::lock;
EmbeddedPython *EmbeddedPython::python = NULL;
swig_type_info *(*EmbeddedPython::Python_TypeQuery)(const char *type);
PyObject *(*EmbeddedPython::Python_NewPointerObj)(void *ptr, swig_type_info *type, int flags);
int (*EmbeddedPython::Python_ConvertPtr)(PyObject *obj, void **ptr, swig_type_info *ty, int flags);
tr1::unordered_map<string, swig_type_info*> EmbeddedPython::typeInfoCache;
log4cxx::LoggerPtr EmbeddedPython::logger(log4cxx::Logger::getLogger("lib.EmbeddedPython"));

EmbeddedPython::EmbeddedPython() {
	Py_Initialize();
	PyEval_InitThreads();

//PyThreadState* tcur = PyThreadState_Get() ;
//
//PyThreadState_Swap(NULL);
//PyThreadState_Clear(tcur);
//PyThreadState_Delete(tcur);
//
// release the GIL as PyEval_InitThreads 
// implicitly acquires the GIL
//PyEval_ReleaseLock();

//PyThreadState *pts = PyGILState_GetThisThreadState();
//PyEval_ReleaseThread(pts);

	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
}

EmbeddedPython::~EmbeddedPython() {
	Py_Finalize();
}

EmbeddedPython *EmbeddedPython::GetPython() {
	lock.Lock();
	if (!python) {
		python = new EmbeddedPython();
		PyGILState_STATE gstate = python->Lock();
		PyObject *module = PyImport_ImportModule("Hector");
		if (!module) {
			python->LogError();
			LOG4CXX_ERROR(logger, "Cannot import Hector module");
			return NULL;
		}
		python->Unlock(gstate);
		Python_TypeQuery = (swig_type_info*(*)(const char*))LibraryLoader::LoadLibrary("_Hector.so", "Python_TypeQuery_Wrapper");
		if (!Python_TypeQuery) {
			LOG4CXX_ERROR(logger, "Cannot find Python_TypeQuery_Wrapper");
			return NULL;
		}
		Python_NewPointerObj = (PyObject*(*)(void*, swig_type_info*, int))LibraryLoader::LoadLibrary("_Hector.so", "Python_NewPointerObj_Wrapper");
		if (!Python_NewPointerObj) {
			LOG4CXX_ERROR(logger, "Cannot find Python_NewPointerObj_Wrapper");
			return NULL;
		}
		Python_ConvertPtr = (int(*)(PyObject*, void**, swig_type_info *ty, int))LibraryLoader::LoadLibrary("_Hector.so", "Python_ConvertPtr_Wrapper");
		if (!Python_ConvertPtr) {
			LOG4CXX_ERROR(logger, "Cannot find Python_ConvertPtr_Wrapper");
			return NULL;
		}
	}
	EmbeddedPython *result = python;
	lock.Unlock();
	return result;
}

void EmbeddedPython::LogError() {
	PyObject *exception, *value, *trace;

	PyErr_Fetch(&exception, &value, &trace);
	if (exception == NULL)
		return;
	PyErr_NormalizeException(&exception, &value, &trace);
	if (exception == NULL)
		return;

	PyObject *str = NULL;
	char *error_type;
	if (exception && (str = PyObject_Str(exception)) && PyString_Check(str))
		error_type = strdup(PyString_AsString(str));
	else 
		error_type = strdup("<unknown>");
	Py_XDECREF(str);

	str = NULL;
	char *error_info;
	if (value && (str = PyObject_Str(value)) && PyString_Check(str))
		error_info = strdup(PyString_AsString(str));
	else 
		error_info = strdup("<unknown>");
	Py_XDECREF(str);

	LOG4CXX_ERROR(logger, "Python: " << error_type << ": " << error_info);

	PyTracebackObject *o = (PyTracebackObject*)trace;
	while (o) {
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "    %.500s:%d, in %.500s\n",
			PyString_AsString(o->tb_frame->f_code->co_filename),
			o->tb_lineno,
			PyString_AsString(o->tb_frame->f_code->co_name));
		LOG4CXX_ERROR(logger, buffer);
        	o = o->tb_next;
	}

	Py_XDECREF(exception);
	Py_XDECREF(value);
	Py_XDECREF(trace);
}

PyObject *EmbeddedPython::NewPointerObj(void *ptr, const char *type, int flags) {
	// typeInfoCache is used only with GIL lock held, so no need to apply further lock
	tr1::unordered_map<string, swig_type_info*>::iterator iter = typeInfoCache.find(type);
	swig_type_info *info;
	if (iter != typeInfoCache.end()) {
		info = iter->second;
	} else {
		info = (*Python_TypeQuery)(type);
		typeInfoCache[type] = info;
	}
	return (*Python_NewPointerObj)(ptr, info, flags);
}

int EmbeddedPython::ConvertPtr(PyObject *obj, void **ptr, const char *type, int flags) {
	// typeInfoCache is used only with GIL lock held, so no need to apply further lock
	tr1::unordered_map<string, swig_type_info*>::iterator iter = typeInfoCache.find(type);
	swig_type_info *info;
	if (iter != typeInfoCache.end()) {
		info = iter->second;
	} else {
		info = (*Python_TypeQuery)(type);
		typeInfoCache[type] = info;
	}
	return (*Python_ConvertPtr)(obj, ptr, info, flags);
}

PyObject *EmbeddedPython::LoadModule(const char *name, const char *format, ...) {
	PyObject *module = PyImport_ImportModule(name);
	if (!module) {
		LogError();
		LOG4CXX_ERROR(logger, "Cannot load module: " << name);
		return NULL;
	}
	PyObject *cl = PyObject_GetAttrString(module, name);
	Py_DECREF(module);
	if (!cl) {
		LogError();
		LOG4CXX_ERROR(logger, "Cannot load module: " << name);
		return NULL;
	}
	va_list var;
	va_start(var, format);
	PyObject *args = Py_VaBuildValue(format, var);
	va_end(var);
	if (!args) {
		Py_DECREF(cl);
		LogError();
		LOG4CXX_ERROR(logger, "Cannot construct constructor arguments: " << format);
		return NULL;
	}

	PyObject *object = PyEval_CallObject(cl, args);
	Py_DECREF(cl);
	Py_DECREF(args);
	if (!object) {
		LogError();
		LOG4CXX_ERROR(logger, "Cannot call constructor: " << name);
		return NULL;
	}
	return object;
}

PyObject *EmbeddedPython::_CallMethod(PyObject *module, const char *name, PyObject *args) {
	PyObject *method = PyObject_GetAttrString(module, name);
	if (!method) {
		LogError();
		LOG4CXX_ERROR(logger, "Cannot get method: " << name);
		return NULL;
	}

	PyObject *result = PyEval_CallObject(method, args);
	Py_DECREF(method);
	if (!result) {
		LogError();
		LOG4CXX_ERROR(logger, "Cannot call method: " << name);
		return NULL;
	}

	return result;
}

PyObject *EmbeddedPython::CallMethod(PyObject *module, const char *name, const char *format, ...) {
	va_list var;
	va_start(var, format);
	PyObject *args = Py_VaBuildValue(format, var);
	va_end(var);
	if (!args) {
		LogError();
		LOG4CXX_ERROR(logger, "Cannot construct arguments: " << format);
		return NULL;
	}
	PyObject *result = _CallMethod(module, name, args);
	Py_DECREF(args);
	return result;
}

PyObject *EmbeddedPython::CallMethod(PyObject *module, const char *name, const char *format, va_list varargs) {
	PyObject *args = Py_VaBuildValue(format, varargs);
	if (!args) {
		LogError();
		LOG4CXX_ERROR(logger, "Cannot construct arguments: " << format);
		return NULL;
	}
	PyObject *result = _CallMethod(module, name, args);
	Py_DECREF(args);
	return result;
}

/*
int main(int argc, char *argv[]) {
	EmbeddedPython *python = new EmbeddedPython();

	Py_DECREF(args);
	PyObject *method = PyObject_GetAttrString(object, "ProcessInput");
	if (!method) {
		PyErr_Print();
		fprintf(stderr, "Error");
		return 1;
	}
	Py_DECREF(object);

	args = Py_BuildValue("(s)", " ABC");
	if (!args) {
		PyErr_Print();
		fprintf(stderr, "Error");
		return 1;
	}
	PyObject *ret = PyEval_CallObject(method, args);
	if (!ret) {
		PyErr_Print();
		fprintf(stderr, "Error");
		return 1;
	}
	Py_DECREF(ret);

	Release the thread. No Python API allowed beyond this point.
	PyGILState_Release(gstate);

	delete python;
}
*/
