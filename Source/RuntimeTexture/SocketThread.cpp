// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketThread.h"
#include "SocketSubsystem.h"
#include "RuntimeTextureGameModeBase.h" 
#include <String>
#include <new>
#include <fstream>

SocketThread::SocketThread()
{
    this->addrString = "";
    this->addrPort = 0;
    this->gm = nullptr;
}

SocketThread::SocketThread(const FString serverAddr, const uint16_t portnum)
{
    this->addrString = serverAddr;
    this->addrPort = portnum;
    this->gm = nullptr;
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
        int len = 0;
        uint8_t* buf = this->LoadTestImg(len);
        this->isToFetch = false;
        ((ARuntimeTextureGameModeBase*)this->gm)->ParseRawImageData(buf, len);
        //continue;
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
        std::string s(buffer);
        std::wstring wide_s;
        wide_s.assign(s.begin(), s.end());
        FString fs(wide_s.c_str());
        UE_LOG(LogTemp, Warning, L"bytes received: %s.", *fs);
        int bytesToAllocate = std::stoi(std::string(buffer));
        if (bytesToAllocate == 0) {
            this->isToFetch = false;
            UE_LOG(LogTemp, Warning, L"No file is available for streaming.");
            continue;
        }
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
        this->isToFetch = false;
        if (this->gm == nullptr) {
            delete[] fileBuffer;
            continue;
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

uint8* SocketThread::LoadTestImg(int& bufLen)
{
    std::ifstream f("F:/php_uploads/k60183f1b47c25.png", std::ios::binary);
    if (!f.is_open()) {
        return nullptr;
    }
    //get length of file
    f.seekg(0, std::ios::end);
    size_t length = f.tellg();
    f.seekg(0, std::ios::beg);
    uint8_t* buffer = new(std::nothrow) uint8_t[length];
    if (buffer == nullptr) {
        return nullptr;
    }
    //read file
    f.read((char*)buffer, length);
    f.close();
    bufLen = length;

    return buffer;
}
