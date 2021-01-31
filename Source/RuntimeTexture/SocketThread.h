// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "GameFramework/GameModeBase.h"

/**
 * 
 */
class RUNTIMETEXTURE_API SocketThread : public FRunnable
{
public:
	SocketThread(void);
	SocketThread(const FString serverAddr, const uint16_t serverPort);
	~SocketThread();

	//Interface
	virtual bool init(void);
	virtual uint32 Run(void);
	virtual void Stop(void);

	//functions
	virtual void AttachActor(AGameModeBase* pointer);
	virtual void FetchImage(void) { this->isToFetch = true; };
	virtual void SetThreadInfo(const FString serverAddr, const uint16_t serverPort);
private:
	bool isRunning = false;
	bool forceStop = false;
	bool isToFetch = false;
	FString addrString;
	uint16_t addrPort;
	AGameModeBase* gm = nullptr;
	FSocket* socket = nullptr;
	TSharedPtr<FInternetAddr> addrInfo = nullptr;
};
