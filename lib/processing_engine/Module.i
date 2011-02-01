%nodefaultctor Module;
%{
#include "Module.h"
%}

class Module {
public:
        enum Type {
                INVALID = 0,
                INPUT =  1,
                OUTPUT = 2,
                SIMPLE = 3,
                MULTI =  4,
        };
};
