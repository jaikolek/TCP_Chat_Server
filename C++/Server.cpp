#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")

std::vector<std::string> chatData;

void savingChat(std::vector<std::string> chats)
{
	std::ofstream file;
	file.open("chat_log.txt");
	for (int i = 0; i < chats.size(); ++i)
	{
		file << chats[i] << std::endl;
	}
	file.close();
}

int main()
{
	int port = 12000;

	// Initialize Winsock
	WSADATA wsData; // WinSock data-structure
	WORD ver = MAKEWORD(2, 2); // means 2.2 version

	int wsOk = WSAStartup(ver, &wsData); // initializing Winsock lib 

	if (wsOk != 0)
	{
		std::cout << "Can`t Initialize Winsock! Quitting..." << std::endl;
		return 1;
	}
	else
	{
		std::cout << "WinSock Started..." << std::endl;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); // make socket with IPv4, TCP (SOCK_DGRAM:UDP)

	if (listening == INVALID_SOCKET)
	{
		std::cout << "Can`t create a socket! Quitting..." << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Socket Created..." << std::endl;
	}

	// Bind the socket to an ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port); // host byte to network byte (little->big endian)
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master; // data-structrue that contains list of socket, and their stats
	FD_ZERO(&master);
	
	FD_SET(listening, &master);

	bool running = true;

	while (running)
	{
		// select function cause changing of fd. so we need copy of fd
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{

			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				std::string welcomeMsg = "Welcome to my Chat Server...\r\n";
				
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

				// broadcast welcome message
				std::ostringstream ss;
				ss << "Client" << " #" << client << " connected.\r\n";
				std::string strOut = ss.str();
				for (int i = 0; i < master.fd_count; i++)
				{
					SOCKET outSock = master.fd_array[i];
					if (outSock != listening && outSock != client)
					{
						send(outSock, strOut.c_str(), strOut.size() + 1, 0);
					}
				}
				// logging welcome message to server
				std::cout << strOut << std::endl;
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive Message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Check to see if it`s a command.  \quit kills the server
					if (buf[0] == '\\')
					{
						std::string cmd = std::string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}
					}

					// Send message to other client, and definiately not the listening socket
					std::ostringstream ss;
					ss << "Client #" << sock << ":" << buf << "\r\n";
					std::string strOut = ss.str();
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && sock != outSock)
						{
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
					// logging user`s message to server
					std::cout << strOut << std::endl;

					chatData.push_back(strOut);
					savingChat(chatData);
				}
			}
		}
	}

	// Close listening socket
	FD_CLR(listening, &master);
	closesocket(listening);

	// Cleanup Winsock
	WSACleanup();

	// pause system for .exe file
	system("pause");
	return 0;
}
