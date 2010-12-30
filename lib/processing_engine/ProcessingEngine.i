%{
#include "ProcessingEngine.h"
%}

class ProcessingEngine : public Object {
public:
        ProcessingEngine(ObjectRegistry *objects, const char *id);
        ~ProcessingEngine();

        SyncQueue<Resource> *getOutputQueue();

        /* helper methods for SWIG */
        static void ResourceQueuePush(std::queue<Resource*> *queue, Resource *resource);
        static Resource *ResourceQueuePop(std::queue<Resource*> *queue);
        static int ResourceQueueSize(std::queue<Resource*> *queue);
};

