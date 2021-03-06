# TestSimple.py, simple, python
# Process resources using ProcessSimple()
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed
# foo		r/w	Test string
# flipStatus	r/w	Whether to change status from 1 to 0 (or vice-versa)
# setStatus	r/w	Whether to set status to an arbitrary value (setStatus >= 0)

import Hector

class TestSimple:
    def __init__(self, _object, _id, threadIndex):
	self._object = _object
	self._id = _id
	self.threadIndex = threadIndex
    	self.values = { 'items': 0, 'foo': "", 'flipStatus': 0, 'setStatus': -1 }

    def __del__(self):
	pass

    def Init(self, params):
	# second stage?
	if params is None:
	    return True

	try:
	    for p in params:
		if p[0] in self.values:
		    self.values[p[0]] = int(p[1]) if isinstance(self.values[p[0]], int) else str(p[1])
	    return True
	except:
	    return False

    def GetType(self):
	return Hector.Module.SIMPLE

    def Start(self):
	pass

    def Stop(self):
	pass

    def Pause(self):
	pass

    def Resume(self):
	pass

    def GetValue(self, name):
	if name in self.values:
	    return str(self.values[name])
	self._object.log_error("Invalid value name: "+name);
	return None;

    def SetValue(self, name, value):
	try:
	    if name in self.values:
		self.values[name] = int(values) if isinstance(self.values[name], int) else str(value)
	    return True
	except:
	    pass
	self._object.log_error("Invalid value name: "+name);
	return False;

    def ListNames(self):
	return self.values.keys()

    def SaveCheckpoint(self, path, id):
	self._object.log_info("SaveCheckpoint(path, id)");

    def RestoreCheckpoint(self, path, id):
	self._object.log_info("RestoreCheckpoint(path, id)");

    def ProcessSimple(self, resource):
	if (resource is None or resource.GetTypeString() != "TestResource"):
		self._object.log_error("Invalid resource: "+resource.GetTypeString())
		return resource
	self._object.log_info(resource.ToStringShort()+" Processing ("+resource.GetStr()+")")
	if self.values['flipStatus']:
		resource.SetStatus(1 if resource.GetStatus() == 0 else 0)
	if self.values['setStatus'] >= 0:
		resource.SetStatus(self.values['setStatus'])
	self.values['items'] += 1
	return resource
