/**
 * Processing chain: call several modules
 */

#ifndef _PROCESSING_CHAIN_H_
#define _PROCESSING_CHAIN_H_

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
	vector<Processor*> processors;

	bool propRun;
	bool propPause;

	std::tr1::unordered_map<string, char*(ProcessingEngine::*)(const char*)> getters;
	std::tr1::unordered_map<string, void(ProcessingEngine::*)(const char*, const char*)> setters;

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
	vector<string> *listNamesSync();

	static log4cxx::LoggerPtr logger;
};

inline void ProcessingEngine::Start() {
	DoLock();
	StartSync();
	DoUnlock();
}

inline void ProcessingEngine::Stop() {
	DoLock();
	StopSync();
	DoUnlock();
}

inline void ProcessingEngine::Pause() {
	DoLock();
	PauseSync();
	DoUnlock();
}

inline void ProcessingEngine::Resume() {
	DoLock();
	ResumeSync();
	DoUnlock();
}

#endif
