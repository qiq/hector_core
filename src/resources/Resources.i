%module Resources

%{
#undef New // protocol buffers + SWIG interaction
#define SWIG_FILE_WITH_INIT // for Python
#include "Resources.h"
%}

Resource *ConstructResource(int type);
WebResource *Resource2WebResource(Resource *resource);
