/**
 * N.B.: Python interpreter calls must be guarged by a GIL, so we call python->Lock() and python->Unlock()
 */

#include <assert.h>
#include <string.h>
#include "common.h"
#include "EmbeddedPython.h"
#include "PythonModule.h"

using namespace std;

PythonModule::PythonModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name): Module(objects, id, threadIndex) {
	this->name = strdup(name);
	char *dot = strrchr(this->name, '.');
	if (dot)
		*dot = '\0';
	this->threadIndex = threadIndex;
	python = NULL;
	module = NULL;
	obj = NULL;
}

PythonModule::~PythonModule() {
	free(name);

	PyGILState_STATE gstate = python->Lock();
	if (module)
		Py_DECREF(module);
	if (obj)
		Py_DECREF(obj);
	python->Unlock(gstate);
}

bool PythonModule::Init(vector<pair<string, string> > *c) {
	PyObject *args;
	PyObject *r = NULL;
	if (!python) {
		python = EmbeddedPython::GetPython();
		if (!python) {
			LOG_ERROR(this, "Cannot instantiate Python");
			return false;
		}
	}
	PyGILState_STATE gstate = python->Lock();
	// first stage?
	if (c) {
		// load python module
		obj = python->NewPointerObj(const_cast<void*>(static_cast<const void*>(this)), "Object *", 0);
		module = python->LoadModule(name, "(Osi)", obj, GetId(), threadIndex);
		if (module) {
			// create c vector (list of pairs) in Python	
			args = PyList_New(c->size());
			int i = 0;
			for (vector<pair<string, string> >::iterator iter = c->begin(); iter != c->end(); ++iter) {
				PyList_SetItem(args, i++, PyTuple_Pack(2, PyString_FromString(iter->first.c_str()), PyString_FromString(iter->second.c_str())));
			}
			// call Init(c)
			r = python->CallMethod(module, "Init", "(O)", args);
			Py_DECREF(args);
		}
	} else {
		// call Init()
		Py_INCREF(Py_None);
		r = python->CallMethod(module, "Init", "(O)", Py_None);
		Py_DECREF(Py_None);
	}
	bool result = false;
	if (!r) {
		LOG_ERROR(this, "Error initialize module " << name);
	} else {
		if (PyBool_Check(r) && r == Py_True)
			result = true;
		Py_DECREF(r);
	}
	python->Unlock(gstate);
	return result;
}

Module::Type PythonModule::GetType() {
	PyGILState_STATE gstate = python->Lock();
	Module::Type result = Module::INVALID;

	PyObject *r = python->CallMethod(module, "GetType", "()");
	if (!r) {
		LOG_ERROR(this, "Error calling GetType");
	} else if (!PyInt_Check(r)) {
		LOG_ERROR(this, "Error calling GetType: invalid result");
		Py_DECREF(r);
	} else {
		result = (Module::Type)PyLong_AsLong(r);
		Py_DECREF(r);
	}

	python->Unlock(gstate);
	return result;
}

void PythonModule::StartSync() {
	PyGILState_STATE gstate = python->Lock();

	PyObject *out = python->CallMethod(module, "Start", "()");
	if (out) {
		if (PyInt_Check(out) >= 0) {
			if (PyInt_AsLong(out) == 0)
				LOG_ERROR(this, "Start returned error");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling Start");
	}

	python->Unlock(gstate);
}

void PythonModule::StopSync() {
	PyGILState_STATE gstate = python->Lock();

	PyObject *out = python->CallMethod(module, "Stop", "()");
	if (out) {
		if (PyInt_Check(out) >= 0) {
			if (PyInt_AsLong(out) == 0)
				LOG_ERROR(this, "Stop returned error");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling Stop");
	}

	python->Unlock(gstate);
}

void PythonModule::PauseSync() {
	PyGILState_STATE gstate = python->Lock();

	PyObject *out = python->CallMethod(module, "Pause", "()");
	if (out) {
		if (PyInt_Check(out) >= 0) {
			if (PyInt_AsLong(out) == 0)
				LOG_ERROR(this, "Pause returned error");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling Pause");
	}

	python->Unlock(gstate);
}

void PythonModule::ResumeSync() {
	PyGILState_STATE gstate = python->Lock();

	PyObject *out = python->CallMethod(module, "Resume", "()");
	if (out) {
		if (PyInt_Check(out) >= 0) {
			if (PyInt_AsLong(out) == 0)
				LOG_ERROR(this, "Resume returned error");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling Resume");
	}

	python->Unlock(gstate);
}


Resource *PythonModule::ProcessInputSync(bool sleep) {
	PyGILState_STATE gstate = python->Lock();
	Resource *result = NULL;

	PyObject *out = python->CallMethod(module, "ProcessInput", "()");
	if (out) {
		void *r = NULL;
		if (python->ConvertPtr(out, &r, "Resource *", 0x01) >= 0) {
			result = reinterpret_cast<Resource*>(r);
		} else {
			LOG_ERROR(this, "Error calling ProcessInput");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling ProcessInput");
	}

	python->Unlock(gstate);
	return result;
}

Resource *PythonModule::ProcessOutputSync(Resource *resource) {
	PyGILState_STATE gstate = python->Lock();
	Resource *result = NULL;

	PyObject *in;
	if (resource) {
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "%s *", resource->GetObjectName());
		in = python->NewPointerObj(const_cast<void*>(static_cast<const void*>(resource)), buffer, 0x01);
	} else {
		Py_INCREF(Py_None);
		in = Py_None;
	}
	PyObject *out = python->CallMethod(module, "ProcessOutput", "(O)", in);
	Py_DECREF(in);
	if (out) {
		void *r = NULL;
		if (python->ConvertPtr(out, &r, "Resource *", 0x01) >= 0) {
			result = reinterpret_cast<Resource*>(r);
		} else {
			LOG_ERROR(this, "Error calling ProcessOutput");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling ProcessOutput: no result");
	}

	python->Unlock(gstate);
	return result;
}

Resource *PythonModule::ProcessSimpleSync(Resource *resource) {
	PyGILState_STATE gstate = python->Lock();
	Resource *result = NULL;

	PyObject *in;
	if (resource) {
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "%s *", resource->GetObjectName());
		in = python->NewPointerObj(const_cast<void*>(static_cast<const void*>(resource)), buffer, 0x01);
	} else {
		Py_INCREF(Py_None);
		in = Py_None;
	}
	PyObject *out = python->CallMethod(module, "ProcessSimple", "(O)", in);
	Py_DECREF(in);
	if (out) {
		void *r = NULL;
		if (python->ConvertPtr(out, &r, "Resource *", 0x01) >= 0) {
			result = reinterpret_cast<Resource*>(r);
		} else {
			LOG_ERROR(this, "Error calling ProcessSimple");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling ProcessSimple: no result");
	}

	python->Unlock(gstate);
	return result;
}

int PythonModule::ProcessMultiSync(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectingResources) {
	PyGILState_STATE gstate = python->Lock();

	int result = 0;
	PyObject *inRes = PyList_New(inputResources->size());
	int i = 0;
	while (inputResources->size() > 0) {
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "%s *", inputResources->front()->GetObjectName());
		PyList_SetItem(inRes, i, python->NewPointerObj(const_cast<void*>(static_cast<const void*>(inputResources->front())), buffer, 0x01));
		inputResources->pop();
		i++;
	}
	PyObject *outRes = PyList_New(outputResources->size());
	i = 0;
	while (outputResources->size() > 0) {
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "%s *", outputResources->front()->GetObjectName());
		PyList_SetItem(outRes, i, python->NewPointerObj(const_cast<void*>(static_cast<const void*>(outputResources->front())), buffer, 0x01));
		outputResources->pop();
		i++;
	}

	PyObject *out = python->CallMethod(module, "ProcessMulti", "(OO)", inRes, outRes);

	if (PyList_Check(inRes) >= 0 && PyList_Check(outRes) >= 0) {
		for (int i = 0; i < PyList_Size(inRes); i++) {
			PyObject *item = PyList_GetItem(inRes, i);
			void *r = NULL;
			if (python->ConvertPtr(item, &r, "Resource *", 0x01) >= 0) {
				inputResources->push(reinterpret_cast<Resource*>(r));
			} else {
				LOG_ERROR(this, "Invalid type of inputResources resource");
			}
		}
		for (int i = 0; i < PyList_Size(outRes); i++) {
			PyObject *item = PyList_GetItem(outRes, i);
			void *r = NULL;
			if (python->ConvertPtr(item, &r, "Resource *", 0x01) >= 0) {
				outputResources->push(reinterpret_cast<Resource*>(r));
			} else {
				LOG_ERROR(this, "Invalid type of inputResources resource");
			}
		}
	} else {
		LOG_ERROR(this, "Error calling ProcessMulti (input/output resources is not a list)");
	}
	Py_DECREF(inRes);
	Py_DECREF(outRes);
	if (out) {
		if (PyTuple_Check(out) >= 0 || PyTuple_Size(out) != 2) {
			PyObject *a = PyTuple_GetItem(out, 0);
			if (PyLong_Check(a) >= 0) {
				result = PyLong_AsLong(a);
			} else {
				LOG_ERROR(this, "Error calling ProcessMulti: expected integer as result[0]");
			}
			PyObject *b = PyTuple_GetItem(out, 1);
			if (PyLong_Check(b) >= 0) {
				if (expectingResources)
					*expectingResources = PyLong_AsLong(b);
			} else {
				LOG_ERROR(this, "Error calling ProcessMulti: expected integer as result[1]");
			}
		} else {
			LOG_ERROR(this, "Error calling ProcessMulti: expected tuple(2)");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling ProcessMulti: no output");
	}

	python->Unlock(gstate);
	return result;
}

char *PythonModule::GetPropertySync(const char *name) {
	PyGILState_STATE gstate = python->Lock();
	assert(name != NULL);

	char *result = NULL;
	PyObject *out = python->CallMethod(module, "GetProperty", "(s)", name);
	if (out) {
		if (PyString_Check(out) >= 0) {
			result = strdup(PyString_AsString(out));
		} else {
			LOG_ERROR(this, "Error calling GetProperty");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling GetProperty");
	}

	python->Unlock(gstate);
	return result;
}

bool PythonModule::SetPropertySync(const char *name, const char *value) {
	PyGILState_STATE gstate = python->Lock();
	assert(name != NULL);

	bool result = false;
	PyObject *out = python->CallMethod(module, "SetProperty", "(ss)", name, value);
	if (out) {
		if (PyInt_Check(out) >= 0) {
			result = PyInt_AsLong(out) != 0;
		} else {
			LOG_ERROR(this, "Error calling SetProperty");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling SetProperty");
	}

	python->Unlock(gstate);
	return result;
}

vector<string> *PythonModule::ListPropertiesSync() {
	PyGILState_STATE gstate = python->Lock();

	vector<string> *result = new vector<string>();
	PyObject *out = python->CallMethod(module, "ListProperties", "()");
	if (out) {
		for (int i = 0; i < PyList_Size(out); i++) {
			PyObject *item = PyList_GetItem(out, i);
			if (PyString_Check(item) >= 0) {
				result->push_back(PyString_AsString(item));
			} else {
				LOG_ERROR(this, "Invalid name");
			}
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling ListProperties");
	}

	python->Unlock(gstate);
	return result;
}

void PythonModule::SaveCheckpointSync(const char *path, const char *id) {
	PyGILState_STATE gstate = python->Lock();

	PyObject *out = python->CallMethod(module, "SaveCheckpoint", "(ss)", path, id);
	if (out) {
		if (PyInt_Check(out) >= 0) {
			if (PyInt_AsLong(out) == 0)
				LOG_ERROR(this, "SaveCheckpoint returned error");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling SaveCheckpoint");
	}

	python->Unlock(gstate);
}

void PythonModule::RestoreCheckpointSync(const char *path, const char *id) {
	PyGILState_STATE gstate = python->Lock();

	PyObject *out = python->CallMethod(module, "RestoreCheckpoint", "(ss)", path, id);
	if (out) {
		if (PyInt_Check(out) >= 0) {
			if (PyInt_AsLong(out) == 0)
				LOG_ERROR(this, "RestoreCheckpoint returned error");
		}
		Py_DECREF(out);
	} else {
		LOG_ERROR(this, "Error calling RestoreCheckpoint");
	}

	python->Unlock(gstate);
}
