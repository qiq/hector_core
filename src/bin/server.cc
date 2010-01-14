/**
 * Universal server: load server libraries based on the configuration.
 */

#include <config.h>

#include <getopt.h>
#include <string>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include "common.h"
#include "Config.h"
#include "Server.h"

log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("bin.server");

const char *configFile = "config.xml";
bool foreground = false;
bool help = false;
int verbose = 0;
char *serverId;

void printHelp() {
	fprintf(stderr, "\
usage: server [options] serverId\n\
options:\n\
  --config, -c\tConfig file path (config.xml)\n\
  --foreground, -f\tDo not fork (0)\n\
  --verbose, -v\t\t\tBe verbose\n\
  --help, -h\t\t\tThis help\n\
  --version, -V\t\t\tVersion information\n");
	exit(EXIT_SUCCESS);
}

void printVersion() {
	fprintf(stderr, "server %s(%s)\nbugreports: %s\n", PACKAGE_STRING, PACKAGE_VERSION, PACKAGE_BUGREPORT);
	exit(0);
}

int processOptions(int argc, char *argv[]) {
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "config", 1, NULL, 'c' },
			{ "foreground", 0, NULL, 'f'},
			{ "verbose", 0, NULL, 'v' },
			{ "help", 0, NULL, 'h' },
			{ "version", 0, NULL, 'V'},
			{ NULL, 0, NULL, 0 }
		};

		int c = getopt_long(argc, argv, "c:fvhV", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'c':
			configFile = strdup(optarg);
			break;
		case 'f':
			foreground = true;
			break;
		case 'v':
			verbose++;
			break;
		case 'h':
			help = true;
			break;
		case 'V':
			printVersion();
			break;
		default:
			break;
		}
	}

	return optind;
}

int main(int argc, char *argv[]) {
	char buffer[1024];

	// parse command line
	int cmdIndex = processOptions(argc, argv);
	if (cmdIndex >= argc)
		printHelp();
	serverId = argv[cmdIndex];

	// load config file
	Config *config = new Config();
	if (!config->parseFile(configFile))
		exit(EXIT_FAILURE);

	// check that serverId does exist
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']", serverId);
	char *s = config->getFirstValue(buffer);
	if (!s)
		die("serverId %s not found in %s\n", serverId, configFile);
	free(s);

	// set up logging
	char *baseDir = config->getFirstValue("/Config/@baseDir");
	if (!baseDir)
		die("Cannot find baseDir\n");
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/logConfig", serverId);
	char *logConfig = config->getFirstValue(buffer);
	if (!logConfig)
		die("logConfig not found\n");
	snprintf(buffer, sizeof(buffer), "%s/%s", baseDir, logConfig);
	free(baseDir);

	log4cxx::PropertyConfigurator::configure(buffer);

	// create and initialize the Server object
	Server *server = new Server(serverId);
	if (!server->Init(config))
		die("Cannot initialize server, sorry.\n");

	// run server
	server->start(true);

	// TODO: daemon mode: http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html

	delete server;
	delete config;
	
	exit(EXIT_SUCCESS);
}
