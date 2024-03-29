#include "Connection.hpp"

void
Connection::eventLoop()
{
	while (true)
	{
		eventNum = senseEvents();
		clearChangeList();
		for (int i = 0; i < eventNum; i++)
		{
			currEvent = &getEventList()[i];
			if (currEvent->flags & EV_EOF || currEvent->fflags & EV_EOF)
				handleEofEvent();
			else if (currEvent->flags & EV_ERROR)
				handleErrorEvent();
			if (currEvent->filter == EVFILT_TIMER)
				handleTimeOut();
			else if (currEvent->filter == EVFILT_READ)
				handleReadEvent();
			else if (currEvent->filter == EVFILT_WRITE)
				handleWriteEvent();
		}
	}
}

void
Connection::handleEofEvent()
{
	//std::cout << "	HandleEofEvent : " << currEvent->ident << "  errno is :"<< errno <<std::endl;
	if (currEvent->filter == EVFILT_PROC)
		return ;
	handleErrorEvent();
}

void
Connection::handleTimeOut()
{
	if (m_clientMap.find(currEvent->ident) != m_clientMap.end())
	{
		std::cerr << RED << "Time Out ";
		deleteClient(currEvent->ident);
	}
}

void
Connection::handleErrorEvent()
{
	//std::cout << "handleErrorEvent : " << currEvent->ident <<  " errno is : " << errno << std::endl;
	shutdown(currEvent->ident, SHUT_RDWR);
	if (m_serverMap.find(currEvent->ident) != m_serverMap.end())
	{
		if (m_clientMap.empty() == true)
			return ;
		std::vector<int>::iterator it = m_serverMap[currEvent->ident].m_clientVec.begin();
		for (; it != m_serverMap[currEvent->ident].m_clientVec.end(); it++ )
			deleteClient(*it);
		this->m_serverMap.erase(this->m_serverMap.find(currEvent->ident));
		close(currEvent->ident);
	}
	else if (this->m_clientMap.find(currEvent->ident) != this->m_clientMap.end())
		deleteClient(currEvent->ident);

	if (this->m_fileMap.find(currEvent->ident) != this->m_fileMap.end())
	{
		m_fileMap[currEvent->ident]->m_file.buffer.clear();
		this->m_fileMap.erase(this->m_fileMap.find(currEvent->ident));
		close(currEvent->ident);
	}
}

void
Connection::deleteClient(int socket)
{
	if (m_clientMap.find(socket) == m_clientMap.end())
		return ;

	int erase = -1;
	std::map<int, Client*>::iterator it;
	for (it = m_fileMap.begin(); it != m_fileMap.end(); ++it)
	{
		if (it->second->m_clientFd == (int)socket)
			erase = it->first;
	}
	if (erase != -1)
	{
		close (erase);
		m_fileMap.erase(erase);
	}

	int server = m_clientMap.find(socket)->second.ptr_server->m_serverFd;
	std::vector<int>::iterator it2;
	for (it2 = m_serverMap.find(server)->second.m_clientVec.begin(); it2 != m_serverMap.find(server)->second.m_clientVec.end(); it2++)
	{
		if (*it2 == socket)
		{
			m_serverMap.find(server)->second.m_clientVec.erase(it2);
			break;
		}
	}
	m_clientMap.erase(socket);
	close(socket);
	std::cerr << RED << "closed : " << socket << RESET << std::endl;
}

/* read event */
void
Connection::handleReadEvent()
{
	/* Server Event Case */
	if (m_serverMap.find(currEvent->ident) != m_serverMap.end())
		acceptClient();

	/* Client Event Case */
	if (m_clientMap.find(currEvent->ident) != m_clientMap.end())
		clientReadEvent();

	/* File Event Case */
	if (m_fileMap.find(currEvent->ident) != m_fileMap.end())
		fileReadRvent();
}

void
Connection::acceptClient()
{
		std::cerr << YELLOW << "SERVER : " << currEvent->ident << "	=> ";
		int clientSocket = accept(currEvent->ident,
									(sockaddr *)&m_serverMap[currEvent->ident].m_serverAddr,
									&m_serverMap[currEvent->ident].m_serverAddrLen);
		if (clientSocket == FAIL)
		{
			std::cerr << "  ERROR : accept() in Server Event Case\n";
			return;
		}
		std::cerr << "ACCEPT : " << clientSocket << RESET << std::endl;
		setNonBlock(clientSocket);
		enrollEventToChangeList(clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		enrollEventToChangeList(clientSocket, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMER, NULL);
		initClient(clientSocket);
}

void
Connection::clientReadEvent()
{
	//std::cout << "\n--IN CLIENT : " << currEvent->ident << "\n";

	std::vector<char> reqBuffer(BUFFER_SIZE);
	int valRead = recv(currEvent->ident, reqBuffer.data(), reqBuffer.size(), 0);
	std::stringstream ss;
	ss << std::string(reqBuffer.begin(), reqBuffer.begin() + valRead);

	if (valRead == FAIL)
	{
		std::cerr << currEvent->ident<<"	ERROR : read() in Client Event Case\n";
		std::vector<int>::iterator it;
		for (it = m_serverMap[m_clientMap[currEvent->ident].ptr_server->m_serverFd].m_clientVec.begin();
				it != m_serverMap[m_clientMap[currEvent->ident].ptr_server->m_serverFd].m_clientVec.end(); ++it)
		{
			if (*it == (int)currEvent->ident)
				break;
		}
		if (it != m_serverMap[m_clientMap[currEvent->ident].ptr_server->m_serverFd].m_clientVec.end())
		{
			m_serverMap[m_clientMap[currEvent->ident].ptr_server->m_serverFd].m_clientVec.at(currEvent->ident);
		}
		close(currEvent->ident);
		m_clientMap.erase(currEvent->ident);
	}
	else if (valRead > 0)
	{
		//std::cout << "\n\n\nRequest \n" << ss.str() << "\n\n";
		m_clientMap[currEvent->ident].reqParser.makeRequest(ss.str());
		m_clientMap[currEvent->ident].status = Res::None;
		if (m_clientMap[currEvent->ident].reqParser.t_result.pStatus == Request::ParseComplete)
		{
			std::cerr <<GREEN << "client : " << currEvent->ident << "  => REQUEST : " << getMethodToStr(m_clientMap[currEvent->ident].reqParser.t_result.method )<< RESET<< std::endl;
			enrollEventToChangeList(currEvent->ident, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, NULL);
			if (m_clientMap[currEvent->ident].reqParser.t_result.header["Cookie"] != "")
			{
				std::cout << "[!]cookie exist\n";
				m_clientMap[currEvent->ident].isCookie = true;
 				/* session manage */
				std::string sessionId = idFromReq(m_clientMap[currEvent->ident].reqParser);
				Log log = logFromReq(m_clientMap[currEvent->ident].reqParser);
				if (m_sessionMap.find(sessionId) != m_sessionMap.end())
				{
					m_sessionMap[sessionId].push_back(log);
					std::map<std::string, std::vector<Log> >::iterator it;
					for (it = m_sessionMap.begin(); it != m_sessionMap.end(); ++it)
					{
						logPrint(it->first, it->second);
						std::cout << "-----------------\n";
					}
					// logPrint(sessionId, m_sessionMap[sessionId]);
				}
				else
				{
					std::vector<Log> logs;
					logs.push_back(log);
					m_sessionMap.insert(std::make_pair(sessionId, logs));
				}

			}
			else
			{
				std::cout << "[!]cookie not set\n";
				m_clientMap[currEvent->ident].isCookie = false;
			}
	
			if (m_clientMap[currEvent->ident].status == Res::None)
			{
				m_clientMap[currEvent->ident].openResponse();
				if (m_clientMap[currEvent->ident].isCgi == false)
				{
					if (m_clientMap[currEvent->ident].m_file.fd != -1)
						readyToResponse();
					else // kq 에 등록할 file event 가 없는 경우 -> autoindex listing or redirecton
						enrollEventToChangeList(currEvent->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				}
				else
				{
					int pipeWrite = m_clientMap[currEvent->ident].m_file.inFds[1];
					int pipeRead = m_clientMap[currEvent->ident].m_file.outFds[0];

					enrollEventToChangeList(pipeWrite, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					fcntl(pipeWrite, F_SETFL, O_NONBLOCK);
					m_fileMap.insert(std::make_pair(pipeWrite, &m_clientMap[currEvent->ident]));
					fcntl(pipeRead, F_SETFL, O_NONBLOCK);
					m_fileMap.insert(std::make_pair(pipeRead, &m_clientMap[currEvent->ident]));
				}
			}
		}
		if (m_clientMap[currEvent->ident].reqParser.t_result.pStatus == Request::ParseError)
		{
			enrollEventToChangeList(currEvent->ident, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, NULL);
			std::cerr <<currEvent->ident<< " : Error : parse\n";
			m_clientMap[currEvent->ident].openErrorResponse(m_clientMap[currEvent->ident].reqParser.t_result.status);
			if (m_clientMap[currEvent->ident].m_file.fd != -1)
				readyToResponse();
		}
	}
}

void
Connection::fileReadRvent()
{
	if (m_fileMap[currEvent->ident]->status == Res::Making)
	{
		int res = m_fileMap[currEvent->ident]->readFile(currEvent->ident);

		switch (res)
		{
		case File::Error:
			m_fileMap[currEvent->ident]->m_file.buffer.clear();
			m_fileMap[currEvent->ident]->openErrorResponse(500);
			if (m_fileMap[currEvent->ident]->m_file.fd != -1)
				readyToResponse(m_fileMap[currEvent->ident]->m_clientFd);
			this->m_fileMap.erase(this->m_fileMap.find(currEvent->ident));
			close(currEvent->ident);
			break;

		case File::Making:
			break;

		case File::Complete:
			m_fileMap[currEvent->ident]->status = Res::Complete;
			m_fileMap[currEvent->ident]->startResponse();
			enrollEventToChangeList(m_fileMap[currEvent->ident]->m_clientFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			close(currEvent->ident);
			m_fileMap.erase(m_fileMap.find(currEvent->ident)->first);
			break;
		}
	}
}


/* write event */
void
Connection::handleWriteEvent()
{

	/* Client Event Case */
	if (m_clientMap.find(currEvent->ident) != m_clientMap.end())
	{
		int result;
		result = m_clientMap[currEvent->ident].sendResponse();
		switch (result)
		{
		case Send::Error:
			deleteClient(currEvent->ident);
			break;
		
		case Send::Making:
			m_clientMap[currEvent->ident].status = Res::Making;
			break;
		
		case Send::Complete:
			if (m_clientMap[currEvent->ident].getConnection() == "close")
			{
				deleteClient(currEvent->ident);
				break;
			}
			m_clientMap[currEvent->ident].status = Res::Complete;
			enrollEventToChangeList(currEvent->ident, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, NULL);
			enrollEventToChangeList(currEvent->ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			m_clientMap[currEvent->ident].clearClient();
			waitpid(m_clientMap[currEvent->ident].m_file.pid, NULL, WNOHANG);
			break;
		}
	}

	/* File Event Case */
	if ((m_fileMap.find(currEvent->ident) != m_fileMap.end()) && (m_fileMap[currEvent->ident]->isCgi == true))
	{
		int result = m_fileMap[currEvent->ident]->writePipe(currEvent->ident);

		switch (result)
		{
		case Write::Error:
			m_fileMap[currEvent->ident]->openErrorResponse(500);
			if (m_fileMap[currEvent->ident]->m_file.fd != -1)
				readyToResponse(m_fileMap[currEvent->ident]->m_clientFd);
			m_fileMap[currEvent->ident]->m_file.m_pipe_sentBytes = 0;
			this->m_fileMap.erase(this->m_fileMap.find(currEvent->ident));
			close(currEvent->ident);
			break;
		case Write::Making:
			break;
		case Write::Complete:
			enrollEventToChangeList(m_fileMap[currEvent->ident]->m_file.outFds[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			close(currEvent->ident);
			m_fileMap.erase(currEvent->ident);
			break;
		}
	}
}

void
Connection::setNonBlock(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == FAIL)
		std::cerr << "	ERROR : fcntl()";
}

void
Connection::initClient(int clientSocket)
{
	m_serverMap[currEvent->ident].m_clientVec.push_back(clientSocket);

	Client tmpClient;
	tmpClient.m_clientFd = clientSocket;
	tmpClient.ptr_server = &m_serverMap[currEvent->ident];
	tmpClient.isCgi = false;
	tmpClient.status = Res::None;
	tmpClient.m_resMsg = "";
	tmpClient.m_totalBytes = 0;
	tmpClient.m_sentBytes = 0;
	tmpClient.cgiOutPath = "";
	tmpClient.cgiOutTarget = "";
	tmpClient.m_file.buffer = "";
	tmpClient.m_file.fd = -1;
	tmpClient.m_file.m_pipe_sentBytes = 0;
	tmpClient.m_file.m_sentBytes = 0;
	tmpClient.m_file.m_totalBytes = 0;
	tmpClient.m_file.size = 0;
	m_clientMap.insert(std::pair<int, Client>(clientSocket, tmpClient));
	m_clientMap.find(clientSocket)->second.reqParser.setMaxBody(m_serverMap[currEvent->ident].maxRequestBodySize);
}

std::string
Connection::getMethodToStr(int num)
{
	if (num == 0)
		return ("GET");
	if (num == 1)
		return ("POST");
	if (num == 2)
		return ("DELETE");
	return("UNKOWN");
}

void
Connection::readyToResponse()
{
	int fileFd = m_clientMap[currEvent->ident].m_file.fd;

	enrollEventToChangeList(fileFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	fcntl(fileFd, F_SETFL, O_NONBLOCK);
	m_fileMap.insert(std::make_pair(fileFd, &m_clientMap[currEvent->ident]));
}

void
Connection::readyToResponse(int ident)
{
	int fileFd = m_clientMap[ident].m_file.fd;

	enrollEventToChangeList(fileFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	fcntl(fileFd, F_SETFL, O_NONBLOCK);
	m_fileMap.insert(std::make_pair(fileFd, &m_clientMap[ident]));
}
