#include "webServer/WebServer.hpp"
#include "webServer/config/configParser/Config.hpp"

int main(int argc, char** argv)
{
	Config config("webServer/config/configFiles/default.conf");

	if (argc == 2) {
		std::string path = argv[1];
		if ((path.compare(path.find('.'), 5, ".conf")) != 0) {
			std::cerr << "Error: Config" << std::endl;
			return (1);
		}
		try {
			config = Config(path);
		}
		catch (std::exception& e) {
			std::cerr << "Error: Config" << std::endl;
			return (1);
		}
	}

	WebServer webServer(config);

	try {
		webServer.openServer();
		webServer.runServer();
	}
	catch (std::exception& e) {
		return (1);
	}

	return (0);
}