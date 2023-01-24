#ifndef FILEMANAGE_HPP
#define FILEMANAGE_HPP

#include "../../includes/libraries.hpp"
#include "../../includes/Define.hpp"
#include "../../InfoFd.hpp"
#include "CGI.hpp"

class InfoFile;

class FileManage
{
	public:
		struct FileEvent
		{
			int fd;
			std::size_t size;
			std::string buffer;
			FileEvent() : fd(-1), size(0), buffer("") {}
		};

	public:
		FileEvent m_file;
		InfoFile *m_infoFileptr;
		CGI m_cgi;

	public:
		int isValidStaticSrc(std::string &targetPath);
		int	readFile(int fd);
		void clearFileEvent();

	public:
		std::string getCwdPath()
		{
			char cwd[1024] = {0,};
			getcwd(cwd, 1024);
			std::string cwdPath(cwd);
			return (cwdPath);
		}
};

#endif
