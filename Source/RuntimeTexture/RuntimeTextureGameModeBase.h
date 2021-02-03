// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Containers/Array.h"
#include "RuntimeTexture.h"
#include "HAL/RunnableThread.h"
#include "SocketThread.h"
#include "RuntimeTextureGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class RUNTIMETEXTURE_API ARuntimeTextureGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ARuntimeTextureGameModeBase();
	virtual ~ARuntimeTextureGameModeBase();
	virtual int ParseRawImageData(uint8_t* data , const UINT dataLength);
	virtual bool AttachToTextureUpdate(UActorComponent* actorRef);
	virtual void UpdateWidgets(void);
	virtual void FetchTexture(void);
protected:
	static const FString DEFAULT_ADDR(void) { return FString("127.0.0.1");  };
	static const UINT16 DEFAULT_PORT = 8089;
	virtual void BeginPlay(void) override;
	virtual void SetWidgetTexture(UTexture2D* texture, TArray<uint8_t>* dataBuffer, uint8_t* rawBytes);
private:
	UPROPERTY(Config)
		FString ipAddrString;
	UPROPERTY(Config)
		int ipAddrPort = 0;
	bool isSampleTexture = false;
	FCriticalSection mutex;
	uint8_t* rawDataStore = nullptr;
	TArray<uint8_t>* dataStore = nullptr;
	UTexture2D* curTexture = nullptr;
	TArray<UActorComponent*>* callbackQueue = nullptr;
	SocketThread* socketObject = nullptr;
	FRunnableThread* socketThread = nullptr;
	void ClearPointers(void);
	void CreateSocketThread(void);
};
