/***************************************************/
/* CODED BY JIN H. BANG ===========================*/
/***************************************************/

#include "Response.hpp"

void
Response::responseToClient(int clientSocket, InfoClient infoClient)
{
	std::string resMsg = "";
	if (infoClient.req.t_result.method == GET)
	{
		resMsg = makeResponseGET(infoClient);
	}
	else
	{
		resMsg = makeResponseERR();
	}

	(void)infoClient; // to be used

	std::cout << " response to client : " << clientSocket << "\n";
	long valWrite = write(clientSocket, resMsg.c_str(), resMsg.size());
	if (valWrite == (long)resMsg.size())
		std::cout << "SERVER RESPONSE SENT\n";
	// close(clientSocket);
}

std::string
Response::makeResponseGET(InfoClient &infoClient)
{
	std::stringstream httpRes;
	std::string htmlMsg;
	if (infoClient.req.t_result.target == "/home" || infoClient.req.t_result.target == "/")
		htmlMsg = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 200\n\n<h1>HOME</h1>";

	if (infoClient.req.t_result.target == "/hello")
		htmlMsg = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 200\n\n<h1>Hello</h1><img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAYAAACNbyblAAAAHElEQVQI12P4//8/w38GIAXDIBKE0DHxgljNBAAO9TXL0Y4OHwAAAABJRU5ErkJggg==\" alt=\"Red dot\" />";
	return (htmlMsg);
}

std::string
Response::makeResponseERR()
{
	std::stringstream httpRes;
	std::string htmlMsg = "HTTP/1.1 400 Bad Request\nContent-Type: text/html; charset=utf-8\nContent-Length: 200\n\n<h1>ERROR</h1><img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAYAAACNbyblAAAAHElEQVQI12P4//8/w38GIAXDIBKE0DHxgljNBAAO9TXL0Y4OHwAAAABJRU5ErkJggg==\" alt=\"Red dot\" />";
	return (htmlMsg);
}
