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

class TestInput:
    _object = None
    _id = None
    _threadIndex = None
    values = { 'items': 0, 'foo': "", 'flipStatus': 0, 'setStatus': -1 }

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
	return Hector.Module.SIMPLE

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

    def ProcessSimple(self, resource):
	if self.values['maxItems'] > 0 and self.values['items'] >= self.values['maxItems']:
		self._object.log_error("Invalid resource: "+resource.getTypeStr())
		return resource
	self._object.log_info(resource.toStringShort()+" Processing ("+resource.getStr()+")")
	if self.values['flipStatus']:
		resource.setStatus(1 if resource.getStatus() == 0 else 0)
	if self.values['setStatus'] >= 0:
		resource.setStatus(self.values['setStatus'])
	self.values['items'] += 1
	return resource
