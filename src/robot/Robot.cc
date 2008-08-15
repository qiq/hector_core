/**
 * Main robot class
 */

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include "Config.h"
#include "ProcessingChain.h"
#include "RobotHTTPServer.h"

vector<ProcessingChain*> processingChains;

log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("robot.Robot");

bool parseConfig(const char *fileName) {
	// read & parse config file
	Config *config = new Config();
	config->parseFile(fileName);

	// find which programs we should run
	int run = config->getSize("robot", "run");
	for (int i = 0; i < run; i++) {
		const char *processName = config->getValue("robot", "run", i);
		const char *pType = config->getType(processName);
		if (pType && !strcmp(pType, "ProcessingChain")) {
			ProcessingChain *pc = new ProcessingChain();
			if (!pc->Init(config, processName))
				return false;
			processingChains.push_back(pc);
		} else {
			if (pType) {
				LOG4CXX_ERROR(logger, "Unknown process type: " << pType);
			} else {
				LOG4CXX_ERROR(logger, "Missing process: " << processName);
			}
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
	// set up logging
	log4cxx::PropertyConfigurator::configure("../config/robot.log.props");
	// process Config file
	bool parsed = parseConfig("../config/config.xml"); // FIXME: configurable config file path :-)
	if (!parsed)
		exit(1);

	// create HTTP server
	RobotHTTPServer *server = new RobotHTTPServer(NULL, 1234);
	server->RestrictAccess("127.0.0.1");

	// start everything
	server->Start(1);
	//TODO: start website (load them from disk)
	for (unsigned i = 0; i < processingChains.size(); i++) {
		processingChains[i]->Start();
	}

	printf("Server running\n");
	char s[10];
	fgets(s, sizeof(s), stdin);

	// stop everything
	for (unsigned i = 0; i < processingChains.size(); i++) {
		processingChains[i]->Stop();
	}
	server->Stop();

	printf("Finishing\n");
	fgets(s, sizeof(s), stdin);

	exit(0);
}
