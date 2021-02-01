// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketThread.h"
#include "SocketSubsystem.h"
#include "RuntimeTextureGameModeBase.h" 
#include <String>
#include <new>

SocketThread::SocketThread()
{
    this->addrString = "";
    this->addrPort = 0;
}

SocketThread::SocketThread(const FString serverAddr, const uint16_t portnum)
{
    this->addrString = serverAddr;
    this->addrPort = portnum;
}

SocketThread::~SocketThread()
{
    if (this->socket != nullptr) {
        delete this->socket;
    }
    if(this->addrInfo != nullptr) {
        this->addrInfo.Reset();
    }
    UE_LOG(LogTemp, Warning, TEXT("socket destroyed."));
}

bool SocketThread::Init(void)
{
    UE_LOG(LogTemp, Warning, TEXT("socket initializing..."));
    // I still cannot find the definition of: PLATFORM_SOCKETSUBSYSTEM
    ISocketSubsystem* subsys = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    this->addrInfo = subsys->CreateInternetAddr();
    this->socket = subsys->CreateSocket(NAME_Stream, TEXT("PNG stream socket."), false);
    if (this->socket == nullptr) {
        return false;
    }
    bool res = true;
    this->addrInfo->SetIp(*this->addrString, res);
    if (!res) {
        UE_LOG(LogTemp, Warning, TEXT("IP address is invalid: '%s'."), *this->addrString);
        return false;
    }
    this->addrInfo->SetPort(this->addrPort);
    res = this->socket->Connect(*this->addrInfo);
    if (!res) {
        return false;
    }
    UE_LOG(LogTemp, Warning, TEXT("Connected to the PNG Stream server."));
    this->isRunning = true;
    UE_LOG(LogTemp, Warning, TEXT("socket ready."));

    return true;
}

uint32 SocketThread::Run(void)
{
    // read buffer
    char buffer[1024] = {'\0'};
    // request info msg
    wchar_t fetchInfoMsg[] = { 'P', 'L', '\0' };
    // request data msg
    wchar_t fetchDataMsg[] = { 'D', 'A', 'T', 'A', '\0' };
    // deny data msg
    wchar_t denyDataMsg[] = { 'B', 'A', 'D', '\0' };

    while (this->isRunning) {
        if (this->forceStop) {
            break;
        }
        if (!this->isToFetch) {
            // static sleep time of 0.5 seconds
            FPlatformProcess::Sleep(0.5f);
            continue;
        }
        UE_LOG(LogTemp, Warning, L"Fetching PNG data...");
        int sent = 0;
        bool res = true;
        res = this->socket->Send((uint8_t*)fetchInfoMsg, sizeof(fetchInfoMsg), sent);
        if (!res) {
            return 1;
        }
        res = this->socket->Recv((uint8_t*)buffer, 1024, sent);
        if (!res) {
            return 1;
        }
        int bytesToAllocate = std::stoi(std::string(buffer));
        //checking if file size is less than 1MB
        if (bytesToAllocate >= 1024 * 1024) {
            res = this->socket->Send((uint8_t*)denyDataMsg, sizeof(denyDataMsg), sent);
            if (!res) {
                return 1;
            }
            continue;
        }
        // extending buffer by 1 bytes for \0 symbol.
        bytesToAllocate++;
        uint8_t* fileBuffer = new(std::nothrow) uint8_t[bytesToAllocate];
        if (fileBuffer == nullptr) {
            res = this->socket->Send((uint8_t*)denyDataMsg, sizeof(denyDataMsg), sent);
            if (!res) {
                return 1;
            }
            continue;
        }
        res = this->socket->Send((uint8_t*)fetchDataMsg, sizeof(fetchDataMsg), sent);
        if (!res) {
            return 1;
        }
        res = this->socket->Recv(fileBuffer, bytesToAllocate, sent);
        if (!res) {
            return 1;
        }
        if (this->gm == nullptr) {
            delete[] fileBuffer;
        }
        ((ARuntimeTextureGameModeBase*)this->gm)->ParseRawImageData(fileBuffer, bytesToAllocate);
    }
    UE_LOG(LogTemp, Warning, TEXT("socket loop ending."));

    return 0;
}

void SocketThread::Stop(void)
{
    this->isRunning = false;
    if (this->socket != nullptr) {
        this->socket->Close();
        this->socket->Shutdown(ESocketShutdownMode::ReadWrite);
    }
}

void SocketThread::AttachActor(AGameModeBase* pointer)
{
    this->gm = pointer;
}

void SocketThread::SetThreadInfo(const FString serverAddr, const uint16_t serverPort)
{
    this->addrString = serverAddr;
    this->addrPort = serverPort;
}
