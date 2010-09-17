/**
 * Universal server: load server libraries based on the configuration.
 */

#include <config.h>

#include <getopt.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include "common.h"
#include "Config.h"
#include "Server.h"

log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("bin.server");

const char *configFile = "config.xml";
const char *baseDir = PREFIX "/lib/hector";
bool foreground = false;
bool help = false;
int verbose = 0;
char *serverId;

void printHelp() {
	fprintf(stderr, "\
usage: server [options] serverId [args]\n\
options:\n\
  --config, -c		Config file path (%s)\n\
  --base, -b		Base dir (%s)\n\
  --foreground, -f	Do not fork\n\
  --verbose, -v		Be verbose\n\
  --help, -h		This help\n\
  --version, -V		Version information\n\n\
$1, $2, ... are substituted by args in the config file.\n", configFile, baseDir);
	exit(EXIT_SUCCESS);
}

void printVersion() {
	fprintf(stderr, "server %s(%s)\nbugreports: %s\n", PACKAGE_STRING, PACKAGE_VERSION, PACKAGE_BUGREPORT);
	exit(EXIT_SUCCESS);
}

int processOptions(int argc, char *argv[]) {
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "config", 1, NULL, 'c' },
			{ "base", 1, NULL, 'b'},
			{ "foreground", 0, NULL, 'f'},
			{ "verbose", 0, NULL, 'v' },
			{ "help", 0, NULL, 'h' },
			{ "version", 0, NULL, 'V'},
			{ NULL, 0, NULL, 0 }
		};

		int c = getopt_long(argc, argv, "c:b:fvhV", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'c':
			configFile = strdup(optarg);
			break;
		case 'b':
			baseDir = strdup(optarg);
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
	vector<string> args;
	for (int i = cmdIndex+1; i < argc; i++)
		args.push_back(argv[i]);

	// set environment variables according to baseDir
	char value[10240];
	const char *path = getenv("LD_LIBRARY_PATH");
	snprintf(value, sizeof(value), "%s:%s/modules:%s/resources:%s", baseDir, baseDir, baseDir, path ? path : "");
	setenv("LD_LIBRARY_PATH", value, 1);

	path = getenv("PERL5LIB");
	snprintf(value, sizeof(value), "%s/modules/perl:%s", baseDir, path ? path : "");
	setenv("PERL5LIB", value, 1);

	// load config file
	Config *config = new Config();
	if (!config->parseFile(configFile, &args))
		exit(EXIT_FAILURE);

	// check that serverId does exist
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']", serverId);
	char *s = config->getFirstValue(buffer);
	if (!s)
		die("serverId %s not found in %s\n", serverId, configFile);
	free(s);

	// set up logging
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/logConfig", serverId);
	char *logConfig = config->getFirstValue(buffer);
	if (!logConfig)
		die("logConfig not found\n");

	log4cxx::PropertyConfigurator::configure(logConfig);
	free(logConfig);

	// create and initialize the Server object
	Server *server = new Server(serverId);
	if (!server->Init(config))
		die("Cannot initialize server, sorry.\n");

	if (!foreground) {
		pid_t pid, sid;

        	pid = fork();
		if (pid < 0)
			exit(EXIT_FAILURE);
		if (pid > 0)
			exit(EXIT_SUCCESS);

		// Create a new SID for the child process
		sid = setsid();
		if (sid < 0) {
			LOG4CXX_ERROR(logger, "Error calling setsid()");
			exit(EXIT_FAILURE);
		}

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	// run server
	server->Start(true);

	delete server;
	delete config;
	
	exit(EXIT_SUCCESS);
}
