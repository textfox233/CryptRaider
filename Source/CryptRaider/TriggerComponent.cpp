// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"
//#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"

// Sets default values for this component's properties
UTriggerComponent::UTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("TriggerComponent BeginPlay()"))
}

// Called every frame
void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Display, TEXT("TriggerComponent TICKING"))

	AActor* KeyActor = GetAcceptableActor();
	// check that there's a key actor AND that it's actually a primitive component
	if (KeyActor)
	{
		Mover->SetShouldMove(true);
		//UE_LOG(LogTemp, Display, TEXT("Key found: %s"), *KeyActor->GetActorNameOrLabel());

		// get primitive component
		UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(KeyActor->GetRootComponent());
		if (Component)
		{
			// turn off physics so it moves with the wall
			Component->SetSimulatePhysics(false);
			// attach the actor to the wall 
			KeyActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		}


	}
	else						
	{ 
		Mover->SetShouldMove(false);
		//UE_LOG(LogTemp, Display, TEXT("no key found"));
	}

	//// Retrieve an array of all actors overlaping this component
	//TArray<AActor*> Actors;
	//GetOverlappingActors(Actors);
	//// Log them...
	//for (int32 i = 0; i < Actors.Num(); i++)
	//{
	//	// ...but only if they are tagged
	//	if (Actors[i]->ActorHasTag(KeyTag))
	//	{
	//		UE_LOG(LogTemp, Display, TEXT("Unlocking: [%d]%s"), i, *Actors[i]->GetActorNameOrLabel());
	//	}
	//}

	//for (AActor* Actor : Actors)
	//{
	//	UE_LOG(LogTemp, Display, TEXT("Actor = %s"), *Actor->GetActorNameOrLabel()); 
	//}
}

void UTriggerComponent::SetMover(UMover* NewMover)
{
	Mover = NewMover;
}


AActor* UTriggerComponent::GetAcceptableActor() const
{
	// Retrieve an array of all actors overlaping this component
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);
	
	bool IsKey, IsGrabbed;
	for (int32 i = 0; i < Actors.Num(); i++)
	{
		// ...but only if they are tagged AND is not being grabbed
		IsKey = Actors[i]->ActorHasTag(KeyTag);
		IsGrabbed = Actors[i]->ActorHasTag("Grabbed");
		if (IsKey && !IsGrabbed)
		{
			return Actors[i];
			//UE_LOG(LogTemp, Display, TEXT("Unlocking: [%d]%s"), i, *Actors[i]->GetActorNameOrLabel());
		}
	}
	// nothing found, returns null
	return nullptr;
}
