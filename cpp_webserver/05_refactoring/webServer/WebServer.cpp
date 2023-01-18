#include "WebServer.hpp"

WebServer::WebServer(Config &config)
{
	// config.print_config();
	std::vector<BaseServer> tmpBase = config.getConfigBase();
	std::vector<BaseServer>::iterator it;
	for (it = tmpBase.begin(); it != tmpBase.end(); ++it)
	{
		InfoServer tmpInfo;

		tmpInfo._serverSocket = NONE;
		tmpInfo._ipAddress = it.base()->getBServer().host;
		tmpInfo._port = it.base()->getBServer().port;
		tmpInfo._serverAddr.sin_family = AF_INET; // ip v4
		tmpInfo._serverAddr.sin_port = htons(tmpInfo._port);
		tmpInfo._serverAddr.sin_addr.s_addr = inet_addr(tmpInfo._ipAddress.c_str()); // inet_addr converts 'char const *' to 'unsigned long' in network byte order
		tmpInfo._serverAddrLen = sizeof(tmpInfo._serverAddr);
		memset(tmpInfo._serverAddr.sin_zero, 0, sizeof(tmpInfo._serverAddr.sin_zero)); // it's a buffer only needed to convert 'sockaddr_in' type to 'sockaddr' type, which is larger type.

		_infoServers.push_back(tmpInfo);
	}
}
