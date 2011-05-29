# TestMulti.py, multi, python
# Process resources using ProcessMulti()
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed
# foo		r/w	Test string
# timeTick	r/w	Max time to spend in ProcessMulti()

import Hector
import select
import time

class TestMulti:
    MAX_RESOURCES = 100

    def __init__(self, _object, _id, threadIndex):
	self._object = _object
	self._id = _id
	self.threadIndex = threadIndex
	self.resources = []
    	self.values = { 'items': 0, 'foo': "", 'timeTick': 1000 }

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
	    	elif p[0] == "alias":
		    self.values['foo'] = int(p[1]) if isinstance(self.values['foo'], int) else str(p[1])
	    return True
	except:
	    return False

    def GetType(self):
	return Hector.Module.MULTI

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
	elif name == "alias":
		return str(self.values['foo'])
	self._object.log_error("Invalid value name: "+name);
	return None;

    def SetValue(self, name, value):
	try:
	    if name in self.values:
		self.values[name] = int(values) if isinstance(self.values[name], int) else str(value)
	    elif name == "alias":
		self.values['foo'] = int(values) if isinstance(self.values['foo'], int) else str(value)
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

    def ProcessMulti(self, inputResources, outputResources):
	if inputResources is None or outputResources is None:
	    return False, 0, TestMulti.MAX_RESOURCES

	while len(inputResources) > 0 and len(self.resources) <= TestMulti.MAX_RESOURCES:
	    resource = inputResources.pop(0)
	    if resource.GetTypeString() == "TestResource":
		self.resources.append(resource)
	    else:
		outputResources.append(resource)

	if len(self.resources) == 0:
		return False, 0, TestMulti.MAX_RESOURCES

	select.select([], [], [], self.values['timeTick']/1000000)

	resource = self.resources.pop(0)
	outputResources.append(resource)
	self._object.log_info(resource.ToStringShort()+" Processed ("+resource.GetStr()+")")
	self.values['items'] += 1
	return len(self.resources) > 0, TestMulti.MAX_RESOURCES-len(self.resources), len(self.resources)
