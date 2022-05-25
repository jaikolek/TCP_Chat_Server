#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#pragma comment (lib, "ws2_32.lib")

void receiveSocket(SOCKET sock)
{
	char buf[4096];
	while (1)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		if (bytesReceived > 0)
		{
			std::cout << "FROM SERVER> " << std::string(buf, 0, bytesReceived) << std::endl;
		}
	}
}

int main()
{
	std::string ipAddress = "127.0.0.1";	// IP Address of the server
	int port = 12000;				// Listening port number on the server

	// Initialize Winsock
	WSAData data; // WinSock data-structure
	WORD ver = MAKEWORD(2, 2); // 2.2 version

	int wsResult = WSAStartup(ver, &data); //init winsock lib
	if (wsResult != 0)
	{
		std::cout << "Winsock can't restart, Error in #" << wsResult << std::endl;
		return 1;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cout << "Socket can't create, Error in #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cout << "Can`t connect to server, Error in #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// thread to receive data
	std::thread receiveThread(receiveSocket, sock);

	// Do-while loop to send data
	std::string userInput;
	do
	{
		// Prompt the user for some text
		std::getline(std::cin, userInput);
		if (userInput.size() > 0) // Make sure the user has typed in something
		{
			// Send the text
			// std::cout << "You: ";
			send(sock, userInput.c_str(), userInput.size() + 1, 0);
			std::cout << std::endl;
		}
	} while (userInput.size() > 0);

	receiveThread.join();
	
	// close
	closesocket(sock);
	WSACleanup();

	// pause system for exe file
	system("pause");
	return 0;
}
