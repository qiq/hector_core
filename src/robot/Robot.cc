/**
 * Main robot class
 */

#include "Config.h"
#include "ProcessingChain.h"
#include "RobotHTTPServer.h"

vector<ProcessingChain*> processingChains;

bool parseConfig(const char *fileName) {
	// read & parse config file
	Config *config = new Config();
	config->parseFile(fileName);

	// find which programs we should run
	int run = config->getSize("robot", "run");
	for (int i = 0; i < run; i++) {
		const char *processName = config->getValue("robot", "run", i);
		const char *pType = config->getType(processName);
		if (!strcmp(pType, "ProcessingChain")) {
			ProcessingChain *pc = new ProcessingChain();
			if (!pc->Init(config, processName))
				return false;
			processingChains.push_back(pc);
		} else {
			// TODO: log("Unknown process type");
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
	// process Config file
	parseConfig("../config/config.xml"); // FIXME: configurable config file path :-)

	// create HTTP server
	RobotHTTPServer *server = new RobotHTTPServer(NULL, 1234);
	server->RestrictAccess("127.0.0.1");
	server->Start(2);

//	fprintf(stderr, "x: %s\n", config->getValue("robot", "server-port"));
//	fprintf(stderr, "x: %s\n", config->getValue("robot", "server-port", 1));

	printf("Server running\n");
	char s[10];
	fgets(s, sizeof(s), stdin);
	exit(0);
}
