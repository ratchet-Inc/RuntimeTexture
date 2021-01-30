// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/InputComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "RuntimeTextureGameModeBase.h"

// Sets default values for this component's properties
UMyActorComponent::UMyActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	this->targetWidgetName = "";
}


// Called when the game starts
void UMyActorComponent::BeginPlay()
{
	Super::BeginPlay();
	AGameModeBase* gm = UGameplayStatics::GetGameMode(GetWorld());
	this->gameModePtr = gm;
	APlayerController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UInputComponent* comp = pController->InputComponent;
	comp->BindKey(EKeys::Period, EInputEvent::IE_Released, this, &UMyActorComponent::InputCallback);
}

void UMyActorComponent::InputCallback(void)
{
	UE_LOG(LogTemp, Warning, TEXT("key pressed."));
	this->FindTarget();
	if (this->gameModePtr != nullptr) {
		ARuntimeTextureGameModeBase*  gm = StaticCast<ARuntimeTextureGameModeBase*>(this->gameModePtr);
		gm->AttachToTextureUpdate(this);
	}
}


// Called every frame
void UMyActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMyActorComponent::SetTexture(UTexture2D* texture)
{
	if (this->WidgetImage == nullptr) {
		return;
	}
	this->WidgetImage->SetBrushFromTexture(texture, false);
}

void UMyActorComponent::ClearTexture(void)
{
	this->WidgetImage->SetBrush(this->defaultBrush);
}

void UMyActorComponent::FindTarget()
{
	TArray<UWidgetComponent*> arr;
	this->GetOwner()->GetComponents<UWidgetComponent>(arr);
	if (arr.Num() > 0) {
		UWidgetComponent* widg = arr[0];
		if (widg->GetUserWidgetObject() == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Widget pointer is null."));
			return;
		}
		UWidgetTree* tree = widg->GetUserWidgetObject()->WidgetTree;
		this->WidgetImage = tree->FindWidget<UImage>(FName(this->targetWidgetName));
		this->defaultBrush = this->WidgetImage->Brush;
	}
}

