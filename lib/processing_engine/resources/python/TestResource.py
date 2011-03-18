# TestResource.py, python
# Simple test resource implemented in Python
# 
# Dependencies:
# Parameters:

import Hector

class Dump:
    def __init__(self, _object, _id, threadIndex):
	self._object = _object
	self._id = _id
    	self.values = { }

    def __del__(self):
	pass

    def GetAttr(self, name):
	if name in self.values:
	    return str(self.values[name])
	self._object.log_error("Invalid value name: "+name);
	return None;

    def SetAttr(self, name, value):
	try:
	    if name in self.values:
		self.values[name] = int(values) if isinstance(self.values[name], int) else str(value)
	    return True
	except:
	    pass
	self._object.log_error("Invalid value name: "+name);
	return False;

