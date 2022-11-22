// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UGrabber : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Grab();

	UFUNCTION(BlueprintCallable)
	void Release();

private:
	UPROPERTY(EditAnywhere)
	float MaxGrabDistance = 400; // Length of the grabby line generated to search for objects

	UPROPERTY(EditAnywhere)
	float GrabRadius = 100; // Radius along the grabby line in which the grabber checks for objects

	UPROPERTY(EditAnywhere)
	float HoldDistance = 200; // Distance at which the bearer holds a grabbed object

	FString GrabbedActor = "Empty"; // name of grabbed actor, reused for debug messages

	UPhysicsHandleComponent* GetPhysicsHandle(); // gets a pointer to the physics handle component, logs an error if it can't be accessed

	FHitResult GetGrabbableInReach(bool& OutHasHit) const; // Check if any grabble objects are within reach of the component
};
