// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// grab the physics handler, trigger an error message if it can't be accessed
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	// if there's a physics handle AND a grabbed component move it around
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

// attempt to grab an eligible object
void UGrabber::Grab()
{
	// grab the physics handler, trigger an error message if it can't be accessed
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();


	// Check if any grabble objects are within reach of the component
	bool HasHit;
	FHitResult HitResult = GetGrabbableInReach(HasHit);

	// if a collision occurs grab that component
	if (HasHit)
	{
		// Get it
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		AActor* HitActor = HitResult.GetActor();

		// Turn on physics and detach actor
		HitComponent->SetSimulatePhysics(true);

		// Wake it up (query physics)
		HitComponent->WakeAllRigidBodies();

		// Add 'Grabbed' tag
		HitActor->Tags.Add("Grabbed");
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Grab it
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitResult.GetComponent(),
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);

		// Log it
		GrabbedActor = HitResult.GetActor()->GetDebugName(HitResult.GetActor());
		UE_LOG(LogTemp, Display, TEXT("Grabbed %s"), *GrabbedActor);
	}

}

void UGrabber::Release()
{
	// grab the physics handler, trigger an error message if it can't be accessed
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	// check if there is a physics handler and a grabbed component
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		// get the grabbed component
		UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();

		// Wake it
		GrabbedComponent->WakeAllRigidBodies();

		// Release it
		PhysicsHandle->ReleaseComponent();

		// Remove 'Grabbed' tag
		GrabbedComponent->GetOwner()->Tags.Remove("Grabbed");

		// Log it
		UE_LOG(LogTemp, Display, TEXT("Released %s"), *GrabbedActor);
		GrabbedActor = "Empty";
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("No component to released"));
	}
}

// grab the physics handler, trigger an error message if it can't be accessed
UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() {
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!Result) // check if it can be accessed
	{
		// log negative and return result
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}
	return Result;
}

// Check if any grabble objects are within reach of the component
FHitResult UGrabber::GetGrabbableInReach(bool& OutHasHit) const 
{
	//	collect variables
	FVector Start = GetComponentLocation();
	FVector End = Start + (GetForwardVector() * MaxGrabDistance);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	FHitResult HitResult;	// this is just a declaration, the variable will be assigned a value by ref in the next function if an object is found

	//	Check for collisions via UE5 sweep function
	OutHasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,	// use this trace channel to only check eligible objects
		Sphere);

	// debug draws
	//DrawDebugLine( GetWorld(), Start, End, FColor::Red, false, 5 );
	//DrawDebugSphere( GetWorld(), End, 10, 10, FColor::Blue, false, 5 );
	//DrawDebugSphere( GetWorld(), HitResult.Location, 10, 10, FColor::Green, false, 5 );
	//DrawDebugSphere( GetWorld(), HitResult.ImpactPoint, 10, 10, FColor::Yellow, false, 5 );

	return HitResult;
}