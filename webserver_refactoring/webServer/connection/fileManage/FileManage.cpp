#include "FileManage.hpp"

// int
// FileManage::isValidTarget(std::string &target)
// {
// 	if (target == "")
// 		return (404);
//     std::string srcPath;
//     if (target == "/" || target == "/home")
//         target = "index.html";
//     else if (target == "/submit")
//         target = "submit.html";
//     else if (target == "/upload")
//         target = "upload.html";
//     else if (target == "/server")
//         target = "server.html";
//     else if (target == "/post.py")
//         target = "post.py";
//     else if (target == "/upload.py")
//         target = "delete.py";
//     else if (target == "/submit.py")
//         target = "submit.py";
//     if (m_infoFileptr->m_infoClientPtr->reqParser.t_result.method == GET)
//         srcPath = this->getCwdPath() + "/www/statics";
//     if (m_infoFileptr->m_infoClientPtr->reqParser.t_result.method == POST)
//         srcPath = this->getCwdPath() + "/www/cgi-bin";
//     // std::cout << "path : " << srcPath << std::endl;
//     DIR *dir = opendir(srcPath.c_str());
//     struct dirent *dirent = NULL;
//     // 405 etc to be added.
//     while (true)
//     {
//         dirent = readdir(dir);
//         if (!dirent)
//             break;
//         if (strcmp(dirent->d_name, (target).c_str()) == SUCCESS)
//         {
//             (target).insert(0, "/");
//             struct stat ss;
//             std::string resPath = srcPath + target;
//             if (stat(resPath.c_str(), &ss) == -1 || S_ISREG(ss.st_mode) != true)
//                 return (500);
//             return (200);
//         }
//     }
//     return (404);
// }

int
FileManage::isValidTarget(std::string &target)
{
	if (target == "")
		return 404;
	if (target == "/" || target == "/home")
		target = "index.html";
	else if (target == "/submit")
		target = "submit.html";
	else if (target == "/upload")
		target = "upload.html";
	else if (target == "/server")
		target = "server.html";
	else if (target == "/post.py")
        target = "post.py";
    else if (target == "/upload.py")
        target = "delete.py";
    else if (target == "/submit.py")
        target = "submit.py";

	std::string srcPath;
    srcPath = this->getCwdPath() + "/www/statics";
    if (m_infoFileptr != NULL && m_infoFileptr->m_infoClientPtr->reqParser.t_result.method == POST)
        srcPath = this->getCwdPath() + "/www/cgi-bin";
	
	// std::cout << "path : " << staticPath << std::endl;
	DIR *dir = opendir(srcPath.c_str());
	struct dirent *dirent = NULL;
	while (true)
	{
		dirent = readdir(dir);
		if (!dirent)
			break;
		if (strcmp(dirent->d_name, (target).c_str()) == SUCCESS)
		{
			(target).insert(0, "/");
			return (1);
		}
	}

	return (404);
}

int
FileManage::readFile(int fd)
{
	char buffer[BUFFER_SIZE + 1];

	memset(buffer, 0, sizeof(buffer));
	//std::cout << "reading\n";
	ssize_t size = read(fd, buffer, BUFFER_SIZE);
	//std::cout << size << std::endl;
	if (size < 0)
	{
		close(fd);
		m_infoFileptr->m_fileFdMapPtr->erase(fd);
		m_file.buffer.clear();
		return File::Error;
	}
	m_file.buffer += std::string(buffer, size);
	m_file.size += size;
	if (size < BUFFER_SIZE)
	{
		// close(fd);
		// _fdMap.erase(fd);
		return File::Complete;
	}
	return File::Making;
}

void
FileManage::clearFileEvent()
{
	m_file.fd = -1;
	m_file.size = 0;
	m_file.buffer = "";
}


int
FileManage::writePipe(int fd)
{
    size_t size;
    size = write(fd, m_infoFileptr->m_infoClientPtr->reqParser.t_result.body.c_str() + m_file.m_pipe_sentBytes, \
                m_infoFileptr->m_infoClientPtr->reqParser.t_result.body.length() - m_file.m_pipe_sentBytes);
    if (size < 0)
    {
        close(fd);
        m_file.m_pipe_sentBytes = 0;
        return Write::Error;
    }
    m_file.m_pipe_sentBytes+= size;
    if (m_file.m_pipe_sentBytes >= m_infoFileptr->m_infoClientPtr->reqParser.t_result.body.length() )
    {
        close(fd);
        m_file.m_pipe_sentBytes = 0;
        return Write::Complete;
    }
    return Write::Making;
}

bool
FileManage::isCgiOutDone()
{
	std::string target = m_infoFileptr->m_infoClientPtr->m_responserPtr->cgiOutTarget;
	std::string srcPath = this->getCwdPath() + "/www/cgi-bin";
	DIR *dir = opendir(srcPath.c_str());
    struct dirent *dirent = NULL;
    // 405 etc to be added.
    while (true)
    {
        dirent = readdir(dir);
        if (!dirent)
            break;
        if (strcmp(dirent->d_name, (target).c_str()) == SUCCESS)
        {
            (target).insert(0, "/");
            struct stat ss;
            std::string resPath = srcPath + target;
            if (stat(resPath.c_str(), &ss) == -1 || S_ISREG(ss.st_mode) != true)
                return (false);
            return (true);
        }
    }
	return (false);
}

