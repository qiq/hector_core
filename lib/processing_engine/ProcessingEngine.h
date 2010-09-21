/**
 * Processing engine: call several modules
 */

#ifndef _LIB_PROCESSING_ENGINE_PROCESSING_ENGINE_H_
#define _LIB_PROCESSING_ENGINE_PROCESSING_ENGINE_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include <tr1/unordered_map>
#include "common.h"
#include "Object.h"
#include "ObjectRegistry.h"
#include "Processor.h"

class ProcessingEngine : public Object {
public:
	ProcessingEngine(ObjectRegistry *objects, const char *id);
	~ProcessingEngine();
	bool Init(Config *config);
	void Start();
	void Stop();
	void Pause();
	void Resume();

protected:
	std::vector<Processor*> processors;

	bool propRun;
	bool propPause;

	ObjectValues<ProcessingEngine> *values;

	char *getRun(const char *name);
	void setRun(const char *name, const char *value);
	char *getPause(const char *name);
	void setPause(const char *name, const char *value);

	void StartSync();
	void StopSync();
	void PauseSync();
	void ResumeSync();

	void doPause();
	void doResume();

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();

	static log4cxx::LoggerPtr logger;
};

inline void ProcessingEngine::Start() {
	ObjectLockWrite();
	StartSync();
	ObjectUnlock();
}

inline void ProcessingEngine::Stop() {
	ObjectLockWrite();
	StopSync();
	ObjectUnlock();
}

inline void ProcessingEngine::Pause() {
	ObjectLockWrite();
	PauseSync();
	ObjectUnlock();
}

inline void ProcessingEngine::Resume() {
	ObjectLockWrite();
	ResumeSync();
	ObjectUnlock();
}

#endif
