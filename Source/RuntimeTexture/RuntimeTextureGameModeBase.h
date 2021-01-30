// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Containers/Array.h"
#include "RuntimeTexture.h"
#include "RuntimeTextureGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RUNTIMETEXTURE_API ARuntimeTextureGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ARuntimeTextureGameModeBase();
	virtual ~ARuntimeTextureGameModeBase();
	virtual int ParseRawImageData(uint8_t* data , const UINT dataLength);
	virtual bool AttachToTextureUpdate(UActorComponent* actorRef);
	virtual void UpdateWidgets(void);
protected:
	virtual void BeginPlay(void) override;
	virtual void SetWidgetTexture(UTexture2D* texture, TArray<uint8_t>* dataBuffer, uint8_t* rawBytes);
private:
	FCriticalSection mutex;
	uint8_t* rawDataStore = nullptr;
	TArray<uint8_t>* dataStore = nullptr;
	UTexture2D* curTexture = nullptr;
	TArray<UActorComponent*>* callbackQueue = nullptr;
	void ClearPointers(void);
};
