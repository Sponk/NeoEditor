#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

bool downloadFileToFile(const char* host, const char* file, const char* target, int* error, int port = 80);
std::string downloadFileToString(const char* host, const char* file, int* error, int port = 80);
std::string postRequestToString(const char* host, const char* file, const char* data, int* error, int port = 80);

#endif
