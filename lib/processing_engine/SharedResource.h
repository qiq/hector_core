/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_SHARED_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_SHARED_RESOURCE_H_

#include <config.h>

#include <limits>
#include "Object.h"
#include "PlainLock.h"

class SharedResource : public Resource {
public:
	SharedResource() {};
	virtual ~SharedResource() {};

	// Clear current resource (as delete + new would do, except id is set to 0)
	virtual void Clear();
	virtual Resource *Clone() = 0;

	// id should be unique across all in-memory resources
	virtual int GetId();
	virtual void SetId(int id);
	// status may be tested in Processor to select target queue
	virtual int GetStatus();
	virtual void SetStatus(int status);

	// flags (only used in Processor)
	virtual void SetFlag(Resource::Flag flag);
	virtual void ResetFlag(Resource::Flag flag);
	virtual bool IsSetFlag(Resource::Flag flag);

	// resource may contain link to other resource, it is only kept only in the memory
	virtual Resource *GetAttachedResource();
	virtual void SetAttachedResource(Resource *attachedResource);
	virtual void ClearAttachedResource();

protected:
	void LockRead();
	void LockWrite();
	void Unlock();

private:
	// this is a shared resource: all methods need to take the lock
	RWLock lock;
};

inline void SharedResource::Clear() {
	lock.LockWrite();
	Resource::Clear();
	lock.Unlock();
}

inline int SharedResource::GetId() {
	lock.LockRead();
	int result = Resource::GetId();
	lock.Unlock();
	return result;
}

inline void SharedResource::SetId(int id) {
	lock.LockWrite();
	Resource::SetId(id);
	lock.Unlock();
}

inline int SharedResource::GetStatus() {
	lock.LockRead();
	int result = Resource::GetStatus();
	lock.Unlock();
	return result;
}

inline void SharedResource::SetStatus(int status) {
	lock.LockWrite();
	Resource::SetStatus(status);
	lock.Unlock();
}

inline void SharedResource::SetFlag(Resource::Flag flag) {
	lock.LockWrite();
	Resource::SetFlag(flag);
	lock.Unlock();
}

inline void SharedResource::ResetFlag(Resource::Flag flag) {
	lock.LockWrite();
	Resource::ResetFlag(flag);
	lock.Unlock();
}

inline bool SharedResource::IsSetFlag(Resource::Flag flag) {
	lock.LockRead();
	bool result = Resource::IsSetFlag(flag);
	lock.Unlock();
	return result;
}

inline void SharedResource::SetAttachedResource(Resource *attachedResource) {
	lock.LockWrite();
	Resource::SetAttachedResource(attachedResource);
	lock.Unlock();
}

inline Resource *SharedResource::GetAttachedResource() {
	lock.LockRead();
        Resource *result = Resource::GetAttachedResource();
	lock.Unlock();
	return result;
}

inline void SharedResource::ClearAttachedResource() {
	lock.LockWrite();
	Resource::SetAttachedResource(NULL);
	lock.Unlock();
}

inline void SharedResource::LockRead() {
	lock.LockRead();
}

inline void SharedResource::LockWrite() {
	lock.LockWrite();
}

inline void SharedResource::Unlock() {
	lock.Unlock();
}

#endif
