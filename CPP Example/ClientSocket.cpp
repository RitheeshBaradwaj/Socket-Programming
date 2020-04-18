#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;
#define PORT 5555


DWORD WINAPI sendFunction(LPVOID lpParam) {
    char buffer[1024];
    SOCKET server = *(SOCKET *)lpParam;
    while(true){
        cin.getline(buffer, sizeof(buffer));
        send(server, buffer, sizeof(buffer), 0);
        if(!strcmp(buffer,"bye")){
            return 0;
        }
    }
    return 0;
}

DWORD WINAPI recvFunction(LPVOID lpParam) {
    char buffer[1024];
    SOCKET server = *(SOCKET *)lpParam;
    while(true){
        recv(server, buffer, sizeof(buffer), 0);
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
    SOCKET server;
    SOCKADDR_IN address;
    HANDLE sendThread, recvThread;
    DWORD sendThreadID, recvThreadID;
    char buffer[1024];

    WSAStartup(MAKEWORD(2,0), &WSAData);

    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server == INVALID_SOCKET){
        cout << "Socket Creation failed" << endl;
        WSACleanup();
		return -1;
    }

    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
 
    int connectID = connect(server, (SOCKADDR *)&address, sizeof(address));
    if(connectID == -1){
        cout << "Connection failed" << endl;
        closesocket(server);
        WSACleanup();
		return -1;
    }
    cout << "Connected to server!" << endl;

    sendThread = CreateThread(NULL, 0, sendFunction, (LPVOID)&server, 0, &sendThreadID);
    if(sendThread == NULL){
        cout << "Error in Sending Messages" <<endl;
        closesocket(server);
        CloseHandle(sendThread);
        WSACleanup();
        return -1;
    }

    recvThread = CreateThread(NULL, 0, recvFunction, (LPVOID)&server, 0, &recvThreadID);
    if(recvThread == NULL){
        cout << "Error in Receiving Messages" <<endl;
        closesocket(server);
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

    closesocket(server);
    WSACleanup();
    cout << "Socket closed." << endl;
}