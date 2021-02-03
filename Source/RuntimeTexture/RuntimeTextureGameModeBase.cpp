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
		DuplicateObject(testTexture.Object, this->curTexture);
		if (testTexture.Object == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("texture object is null."));
		}
		if (this->curTexture == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("copy failed."));
		}
	}
}

ARuntimeTextureGameModeBase::~ARuntimeTextureGameModeBase()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroying gamemode."));
	if (this->socketThread != nullptr) {
		this->socketObject->Stop();
		this->socketThread->WaitForCompletion();
		delete this->socketThread;
		delete this->socketObject;
	}
	ClearPointers();
	//Super();
}

void ARuntimeTextureGameModeBase::BeginPlay(void)
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode started."));
	// force the loading of sample texture for debugging.
	// currently disabled, set to true to enable
	if (!true) {
		UE_LOG(LogTemp, Warning, TEXT("Texture is null."));
		this->curTexture = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/k.k'"));
		this->isSampleTexture = true;
		return;
	}
	this->CreateSocketThread();
	UE_LOG(LogTemp, Warning, TEXT("created the socket."));
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
	if (this->curTexture != nullptr && this->isSampleTexture == false) {
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
	UE_LOG(LogTemp, Warning, TEXT("texture parsed."));

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
	UE_LOG(LogTemp, Warning, TEXT("Setting texture."));
	for (int x = 0; x < this->callbackQueue->Num(); ++x) {
		UActorComponent* item = (*this->callbackQueue)[x];
		UMyActorComponent* tmp = StaticCast<UMyActorComponent*>(item);
		tmp->SetTexture(this->curTexture);
	}
}

void ARuntimeTextureGameModeBase::CreateSocketThread(void)
{
	FString s("Image stream thread.");
	FString ip = this->ipAddrString;
	int portNum = this->ipAddrPort;
	// setting defaults to be safe.
	if (ip.Len() == 0) {
		ip = ARuntimeTextureGameModeBase::DEFAULT_ADDR();
	}
	if (this->ipAddrPort == 0) {
		portNum = ARuntimeTextureGameModeBase::DEFAULT_PORT;
	}
	this->socketObject = new(std::nothrow) SocketThread(ip, portNum);
	if (this->socketObject == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Failed to allocate socket thread class."));
		return;
	}
	this->socketThread = FRunnableThread::Create(this->socketObject, *s);
	if (this->socketThread == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Failed to create socket thread."));
		return;
	}
	this->socketObject->AttachActor(this);
}

void ARuntimeTextureGameModeBase::FetchTexture(void)
{
	if (this->socketObject != nullptr) {
		this->socketObject->ToggleFetch();
	}
}

