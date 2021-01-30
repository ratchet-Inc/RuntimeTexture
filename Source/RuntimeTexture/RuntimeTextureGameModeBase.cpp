// Copyright Epic Games, Inc. All Rights Reserved.


#include "RuntimeTextureGameModeBase.h"
#include "ImageUtils.h"
#include "ImageCore.h"
#include "IImageWrapper.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "MyActorComponent.h"
#include <new>

ARuntimeTextureGameModeBase::ARuntimeTextureGameModeBase() : Super()
{
	this->callbackQueue = new(std::nothrow) TArray<UActorComponent*>();
	ConstructorHelpers::FObjectFinder<UTexture2D> testTexture(TEXT("Texture2D'/Game/k.k'"));
	if (testTexture.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("Sample texture found."));
		DuplicateObject(testTexture.Object, this->curTexture, "Sample Texture");
	}
}

ARuntimeTextureGameModeBase::~ARuntimeTextureGameModeBase()
{
	ClearPointers();
	//Super();
}

void ARuntimeTextureGameModeBase::BeginPlay(void)
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode started."));
}

void ARuntimeTextureGameModeBase::SetWidgetTexture(UTexture2D* texture, TArray<uint8_t>* dataSet, uint8_t* rawData)
{
	this->ClearPointers();
	this->rawDataStore = rawData;
	this->dataStore = dataSet;
	this->curTexture = texture;
}

void ARuntimeTextureGameModeBase::ClearPointers(void)
{
	if (this->curTexture != nullptr) {
		delete this->curTexture;
	}
	if (this->dataStore != nullptr) {
		this->dataStore->Empty();
		delete this->dataStore;
	}
	if (this->rawDataStore != nullptr) {
		delete[] this->rawDataStore;
	}
}

int ARuntimeTextureGameModeBase::ParseRawImageData(uint8_t* data, const UINT dataLength)
{
	if (data == nullptr || dataLength == 0) {
		return 1;
	}
	TArray<uint8_t>* dataSet = new(std::nothrow) TArray<uint8_t>();
	if (dataSet == nullptr) {
		return 1;
	}
	for (UINT i = 0; i < dataLength; ++i) {
		dataSet->Add(data[i]);
	}
	UTexture2D *texture = FImageUtils::ImportBufferAsTexture2D(*dataSet);
	this->mutex.Lock();
	this->SetWidgetTexture(texture, dataSet, data);
	this->mutex.Unlock();

	return 0;
}

bool ARuntimeTextureGameModeBase::AttachToTextureUpdate(UActorComponent* actorRef)
{
	if (this->callbackQueue != nullptr) {
		this->callbackQueue->Add(actorRef);
		return true;
	}
	return false;
}

void ARuntimeTextureGameModeBase::UpdateWidgets(void)
{
	if (this->callbackQueue == nullptr) {
		return;
	}
	for (int x = 0; x < this->callbackQueue->Num(); ++x) {
		UMyActorComponent* tmp = StaticCast<UMyActorComponent*>(this->callbackQueue[x]);
		tmp->SetTexture(this->curTexture);
	}
}

