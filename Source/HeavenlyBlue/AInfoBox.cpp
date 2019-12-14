// Fill out your copyright notice in the Description page of Project Settings.


#include "AInfoBox.h"

/*
 * Function:  BeginPlay
 * --------------------
 * This creates the base functionality of the AAInfoBox class.
 *
 */
void AAInfoBox::BeginPlay() 
{
	// These allow the conversation instance to recognize collison with an actor.
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAInfoBox::OnBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AAInfoBox::OnEndOverlap);
	PrimaryActorTick.bCanEverTick = true;
}

/*
 * Function:  OnBeginOverlap
 * --------------------
 * This descibes the behavior when an actor enters the trigger box.
 *
 */
void AAInfoBox::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		CurrentPhase = EInteractablePhase::SD_OVERLAP;
		PrintInteractableName(CurrentItem);
		bInCollision = true;
	}
}

/*
 * Function:  OnEndOverlap
 * --------------------
 * This descibes the behavior when an actor leaves the trigger box.
 *
 */
void AAInfoBox::OnEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		CurrentPhase = EInteractablePhase::SD_NO_OVERLAP;
		bInCollision = false;
	}
}
