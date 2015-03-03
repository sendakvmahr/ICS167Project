#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


#pragma comment(lib, "Ws2_32.lib")
using namespace std;


class NTPServer{
public:
	void CalculateOffset();
	int getOffset();
private:
	int offset;




};