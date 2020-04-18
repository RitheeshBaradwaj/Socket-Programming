#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;
#define PORT 5555


DWORD WINAPI sendFunction(LPVOID lpParam) {
    char buffer[1024];
    SOCKET client = *(SOCKET *)lpParam;
    while(true){
        cin.getline(buffer, sizeof(buffer));
        send(client, buffer, sizeof(buffer), 0);
        if(!strcmp(buffer,"bye")){
            return 0;
        }
    }
    return 0;
}

DWORD WINAPI recvFunction(LPVOID lpParam) {
    char buffer[1024];
    SOCKET client = *(SOCKET *)lpParam;
    while(true){
        recv(client, buffer, sizeof(buffer), 0);
        cout << "\t\t\t\t";
        cout << buffer << endl;
        if(!strcmp(buffer,"bye")){
            return 0;
        }
    }
    return 0;
}
 
int main() {

    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddress, clientAddress;
    HANDLE sendThread, recvThread;
    DWORD sendThreadID, recvThreadID;
    
    WSAStartup(MAKEWORD(2,0), &WSAData);

    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server == INVALID_SOCKET){
        cout << "Socket Creation failed" << endl;
        WSACleanup();
		return -1;
    }
 
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
 
    int bindID = bind(server, (SOCKADDR *)&serverAddress, sizeof(serverAddress));
    if(bindID == -1){
        cout << "Binding failed" << endl;
        closesocket(client);
        WSACleanup();
		return -1;
    }

    int listenID = listen(server, 0);
    if(listenID == -1){
        cout << "Listening failed" << endl;
        closesocket(client);
        WSACleanup();
		return -1;
    }

    cout << "Listening for incoming connections..." << endl;
    int clientAddressLength = sizeof(clientAddress);
    client = accept(server, (SOCKADDR *)&clientAddress, &clientAddressLength);
    
    if(client == INVALID_SOCKET){
        cout << "Accepting failed" << endl;
        closesocket(client);
        cout << "Client disconnected." << endl;
        WSACleanup();
		return -1;
    }
    cout << "Client connected!" << endl;

    sendThread = CreateThread(NULL, 0, sendFunction, (LPVOID)&client, 0, &sendThreadID);
    if(sendThread == NULL){
        cout << "Error in Sending Messages" <<endl;
        closesocket(client);
        CloseHandle(sendThread);
        WSACleanup();
        return -1;
    }

    recvThread = CreateThread(NULL, 0, recvFunction, (LPVOID)&client, 0, &recvThreadID);
    if(recvThread == NULL){
        cout << "Error in Receiving Messages" <<endl;
        closesocket(client);
        CloseHandle(recvThread);
        WSACleanup();
        return -1;
    }

    HANDLE h[2];
    h[0]=sendThread;
    h[1]=recvThread;
    // WaitForSingleObject(sendThread,INFINITE);
    // WaitForSingleObject(recvThread,INFINITE);
    DWORD dw = WaitForMultipleObjects(2,h,FALSE,INFINITE);
    switch(dw-WAIT_OBJECT_0){
        case 0:if(!TerminateThread(recvThread,(DWORD)NULL))
        cout<<"Error due to Terimanation!"<<endl;
        break;
        case 1:if(!TerminateThread(sendThread,(DWORD)NULL))
        cout<<"Error due to Terimanation!"<<endl;
        break;
    }
    CloseHandle(sendThread);
    CloseHandle(recvThread);

    closesocket(client);
    cout << "Client disconnected." << endl;
    WSACleanup();
    return 0;
}