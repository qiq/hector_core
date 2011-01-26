# TestOutput.py, output, python
# Just print out a message that resource arrived.
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed

import Hector
import re

class TestOutput:
    _object = None
    _id = None
    _threadIndex = None
    values = { 'items': 0 }

    def __init__(self, _object, _id, threadIndex):
	self._object = _object
	self._id = _id
	self.threadIndex = threadIndex

    def __del__(self):
	pass

    def Init(self, params):
	# second stage?
	if params is None:
	    return True

	for p in params:
	    if p[0] in self.values:
		self.values[p[0]] = int(p[1])
	return True

    def getType(self):
	return Hector.Module.OUTPUT

    def GetValue(self, name):
	if name in self.values:
	    return str(self.values[name])
	self._object.log_error("Invalid value name: "+name);
	return None;

    def SetValue(self, name, value):
	if name in self.values:
	    self.values[name] = value
	    return True
	self._object.log_error("Invalid value name: "+name);
	return False;

    def ListNames(self):
	expr = re.compile('^[^_]')
	return filter(expr.search, self.values.keys())

    def SaveCheckpoint(self, path, id):
	self._object.log_info("SaveCheckpoint(path, id)");

    def RestoreCheckpoint(self, path, id):
	self._object.log_info("RestoreCheckpoint(path, id)");

    def ProcessOutput(self, resource):
	if (resource is None or resource.getTypeStr() != "TestResource"):
		self._object.log_error("Invalid resource: "+resource.getTypeStr());
	else:
		self._object.log_info(resource.toStringShort()+" Resource arrived ("+resource.getStr()+")");
		self.values['items'] += 1
	return resource
