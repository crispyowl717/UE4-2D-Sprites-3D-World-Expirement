/*
**********************************************************************;
*	Project      : HeavenlyBlue
*
*	Program name : AInfoBox
*
*	Author		 : ResponsibleFile (Dawson McThay)
*
*	Date created : 10/08/2019
*
*	Purpose		 : This class is designed to create a unique instance of an info box
*				   
*
*	Revisions	 : 11/07/2019
*
**********************************************************************;
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Components/ShapeComponent.h"
#include "Engine/World.h" 
#include "GameFramework/Actor.h"
#include "IBaseInteractable.h"
#include "AInfoBox.generated.h"


UCLASS()
class HEAVENLYBLUE_API AAInfoBox : public ATriggerBox, public IIBaseInteractable
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InfoBox Properties")
	FInteractableInfo CurrentItem;

	UPROPERTY()
	bool bInCollision;

protected:
private:
	// Overlap Functions
	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
