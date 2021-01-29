// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

/**
 * 
 */
class RUNTIMETEXTURE_API SocketThread : public FRunnable
{
public:
	SocketThread();
	~SocketThread();

	//Interface
	virtual bool init(void);
	virtual uint32 Run(void);
	virtual void Stop(void);

	//functions
	virtual void AttachActor(AGameModeBase* pointer);
private:
	AGameModeBase* gm = nullptr;
};
