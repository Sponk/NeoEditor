#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

bool downloadFileToFile(const char* host, const char* file, const char* target, int port = 80);
std::string downloadFileToString(const char* host, const char* file, int port = 80);


#endif
