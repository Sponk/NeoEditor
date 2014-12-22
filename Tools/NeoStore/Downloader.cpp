#include "Downloader.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "HappyHTTP.h"

// TODO: Keep this interface UI agnostic!
#include "NeoStore.h"

using namespace std;

class FileDownloader
{
public:
	static unsigned int count;
	static int status;
	static InstallationDlg dlg;
	static void OnBegin( const happyhttp::Response* r, void* userdata )
	{
		printf("BEGIN (%d %s)\n", r->getstatus(), r->getreason());
		count = 0;

		const char* sizeStr = r->getheader("Content-Length");

		unsigned int size = 0;
		if(sizeStr)
			sscanf(sizeStr, "%d", &size);

		printf("EXPECTED SIZE %d\n", size);

		Fl_Window* win = dlg.create_window();
		win->show();

		dlg.setMaxSize(size);
		status = r->getstatus();
	}

	static void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
	{
		fwrite(data,1,n, (FILE*) userdata);
		count += n;

		dlg.update(count);
		Fl::check();
	}

	static void OnComplete( const happyhttp::Response* r, void* userdata )
	{
		printf("COMPLETE (%d bytes)\n", count);
		dlg.close();
	}
};

unsigned int FileDownloader::count = 0;
int FileDownloader::status = 0;
InstallationDlg FileDownloader::dlg;

bool downloadFileToFile(const char* host, const char* file, const char* target, int* error, int port)
{
	try
	{
		happyhttp::Connection conn(host, port);
		FILE* f = fopen(target, "w");

		if(!f)
			return false;

		conn.setcallbacks(FileDownloader::OnBegin, FileDownloader::OnData, FileDownloader::OnComplete, f);
		conn.request("GET", file,0,0,0);


		while(conn.outstanding())
			conn.pump();

		fclose(f);

		if(error)
			*error = FileDownloader::status;

		return true;
	}
	catch(happyhttp::Wobbly w)
	{
		fprintf(stderr, "Could not connect to %s: %s\n", host, w.what());
		if(error)
			*error = FileDownloader::status;
		return false;
	}
}

class StringDownloader
{
public:
	static int count;
	static int status;
	static void OnBegin( const happyhttp::Response* r, void* userdata )
	{
		printf("BEGIN (%d %s)\n", r->getstatus(), r->getreason());
		count = 0;
		status = r->getstatus();
	}

	static void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
	{
		string d = (const char*)data;
		((string*)userdata)->append(d);
		count += n;
	}

	static void OnComplete( const happyhttp::Response* r, void* userdata )
	{
		printf("COMPLETE (%d bytes)\n", count);
	}
};

int StringDownloader::count = 0;
int StringDownloader::status = 0;

std::string downloadFileToString(const char* host, const char* file, int* error, int port)
{
	try
	{
		happyhttp::Connection conn(host, port);
		std::string output;

		conn.setcallbacks(StringDownloader::OnBegin, StringDownloader::OnData, StringDownloader::OnComplete, &output);
		conn.request("GET", file,0,0,0);

		while(conn.outstanding())
			conn.pump();

		output.erase(StringDownloader::count);

		if(error)
			*error = StringDownloader::status;

		return output;
	}
	catch(happyhttp::Wobbly w)
	{
		fprintf(stderr, "Could not connect to %s: %s\n", host, w.what());

		if(error)
			*error = StringDownloader::status;

		return "";
	}
}
