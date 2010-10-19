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

        static void ResourceQueuePush(std::queue<Resource*> *queue, Resource *resource);
        static Resource *ResourceQueuePop(std::queue<Resource*> *queue);
        static int ResourceQueueSize(std::queue<Resource*> *queue);
};
