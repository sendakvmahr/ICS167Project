#include "NTP.h"

void NTPServer::CalculateOffset(){

	char    *hostname = (char *)"timeapi.org/utc/now.json";//"200.20.186.76";
	int portno = 123;     //NTP is port 123
	int maxlen = 1024;        //check our buffers
	int i;          // misc var i
	char msg[48] = { 010, 0, 0, 0, 0, 0, 0, 0, 0 };    // the packet we send
	char  buf[1024]; // the buffer we get back
	//struct in_addr ipaddr;        //  
	struct protoent *proto;     //
	struct sockaddr_in server_addr;
	SOCKET _socket;  // socket
	time_t server_time;   // the time -- This is a time_t sort of

	//Initialize socket system
	WSADATA wsaData;
	BYTE wsMajorVersion = 1;
	BYTE wsMinorVersion = 1;
	WORD wVersionRequested = MAKEWORD(wsMinorVersion, wsMajorVersion);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		cout << "Failed to load winsock stack\n" << endl;
		WSACleanup();
		return;
	}
	if (LOBYTE(wsaData.wVersion) != wsMajorVersion || HIBYTE(wsaData.wVersion) != wsMinorVersion)
	{
		cout << "Winsock stack does not support version which this program requires\n" << endl;
		WSACleanup();
		return;
	}

	proto = getprotobyname("udp");
	int err = GetLastError();
	_socket = socket(PF_INET, SOCK_DGRAM, proto->p_proto);
	if (_socket) {
		perror("asd");
		cout << "socket= " << _socket << endl;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(hostname);
	server_addr.sin_port = htons(portno);


	// send message to server

	cout << "sending data..\n";
	i = sendto(_socket, msg, sizeof(msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
	perror("sendto");

	// receive message

	struct sockaddr saddr;
	socklen_t saddr_l = sizeof(saddr);
	i = recvfrom(_socket, buf, 48, 0, &saddr, &saddr_l);
	perror("recvfr:");


	cout << "Connected to NTP Server 200.20.186.76\n";
	server_time = ntohl(((time_t*)buf)[4]);    //# get transmit time

	server_time -= 2208988800U;

	cout << "\nTime: " << ctime(&server_time) << endl;

	i = time(0);
	offset = i - server_time;
	cout << "\nSystem time is " << offset << " seconds off\n" << endl;
}

int NTPServer::getOffset(){
	return offset;
}