// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeTextureActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "RuntimeTextureGameModeBase.h"

// Sets default values
ARuntimeTextureActor::ARuntimeTextureActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARuntimeTextureActor::BeginPlay()
{
	Super::BeginPlay();
	this->gameModePtr = GetWorld()->GetAuthGameMode();
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UInputComponent* comp = controller->InputComponent;
	if (comp != nullptr) {
		comp->BindKey(EKeys::E, EInputEvent::IE_Pressed, this, &ARuntimeTextureActor::SetSampleTexture);
	}
}

// Called every frame
void ARuntimeTextureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARuntimeTextureActor::SetTexture(UTexture2D* texture)
{
}

void ARuntimeTextureActor::ClearTexture(void)
{
}

void ARuntimeTextureActor::SetSampleTexture(void)
{
	if (this->gameModePtr != nullptr) {
		((ARuntimeTextureGameModeBase*)this->gameModePtr);
	}
}

