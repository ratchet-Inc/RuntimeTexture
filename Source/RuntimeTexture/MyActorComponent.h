// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "MyActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNTIMETEXTURE_API UMyActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyActorComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// functions
	virtual void SetTexture(UTexture2D* texture);
	virtual void ClearTexture(void);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InputCallback(void);

private:
	UPROPERTY(EditAnywhere)
		FString targetWidgetName;
	AGameModeBase* gameModePtr = nullptr;
	UImage* WidgetImage = nullptr;
	FSlateBrush defaultBrush;
	void FindTarget(void);
};
