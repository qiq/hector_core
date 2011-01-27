# TestInput.py, input, python
# Create TestResource resources.
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed
# maxItems	init	Number of resources to generate
# idPrefix	r/w	Prefix to be used as prefix for string in TestResource

import Hector

class TestInput:
    def __init__(self, _object, _id, threadIndex):
	self._object = _object
	self._id = _id
	self.threadIndex = threadIndex
    	self.values = { 'items': 0, 'maxItems': 0, 'idPrefix': None }

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
	    pass
	return False

    def getType(self):
	return Hector.Module.INPUT

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

    def ProcessInput(self):
	if self.values['maxItems'] > 0 and self.values['items'] >= self.values['maxItems']:
		return None
	resource = Hector.TestResource()
	resource.setId(self.threadIndex*10000+self.values['items'])
	s = "" if self.values['idPrefix'] is None else self.values['idPrefix']
	s += str(self.threadIndex)+"-"+str(self.values['items'])
	resource.setStr(s)
	self._object.log_info(resource.toStringShort()+" Loading resource ("+resource.getStr()+")")
	self.values['items'] += 1
	return resource
