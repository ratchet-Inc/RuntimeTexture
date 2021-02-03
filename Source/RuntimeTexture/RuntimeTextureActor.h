// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "RuntimeTextureActor.generated.h"

UCLASS()
class RUNTIMETEXTURE_API ARuntimeTextureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARuntimeTextureActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// functions
	virtual void SetTexture(UTexture2D* texture);
	virtual void ClearTexture(void);
	virtual void SetSampleTexture(void);
	virtual void FetchSampleTexture(void);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	AGameModeBase* gameModePtr = nullptr;
	UImage* WidgetImage = nullptr;
};
