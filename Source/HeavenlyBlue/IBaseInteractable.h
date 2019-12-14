/*
**********************************************************************;
*	Project      : HeavenlyBlue
*
*	Program name : IBaseInteractable
*
*	Author		 : ResponsibleFile (Dawson McThay)
*
*	Date created : 10/08/2019
*
*	Purpose		 : This class is designed to represent any game object that the player can 
				   interact with.
*
*
*	Revisions	 : 11/07/2019
*
**********************************************************************;
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericPlatform/GenericPlatformProcess.h" 
#include "Engine/Engine.h" 
#include "IBaseInteractable.generated.h"

/*
 * Struct:  EInteractablePhase
 * --------------------
 * During any interaction, there is a variety of states that the player can be
 * in relation to the interaction object.
 */
UENUM(BlueprintType)
enum class EInteractablePhase : uint8
{
	SD_NO_OVERLAP 	UMETA(DisplayName = "Not Overlapping Player"),
	SD_OVERLAP 		UMETA(DisplayName = "Overlapping Player"),
	SD_ACTIVE 		UMETA(DisplayName = "Currently Interacting"),
	SD_QUESTION		UMETA(DisplayName = "Currently Asking A Question"),
	SD_EXIT			UMETA(DisplayName = "Finished"),
};

/*
 * Struct:  EInteractableType
 * --------------------
 * ALthough the interactables will have their own ID, there will be special events
 * specific to their type.
 */
UENUM(BlueprintType)
enum class EInteractableType : uint8
{
	SD_Save 	UMETA(DisplayName = "Save"),
	SD_Info 	UMETA(DisplayName = "Info"),
};

/*
 * Struct:  FInteractableInfo
 * --------------------
 * These set the properties for a single item in the game.
 */
USTRUCT(BlueprintType)
struct FInteractableInfo
{
	GENERATED_USTRUCT_BODY()

	// This is a reference to the box.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Properties")
	int32 InteractableID;

	// The is a reference to the item itself 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Properties")
	int32 ItemID;

	// This is the name of the item.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Properties")
	FString ItemName;

	// This is the description of the item.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Properties")
	FString ItemDescription;

	// This detials the specific version of an items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Properties")
	EInteractableType ItemType;
	
	// After the interaction has finished, it checks for a question
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Properties")
	bool bHasQuestion;
};

// This class does not need to be modified.

UINTERFACE(Blueprintable)
class UIBaseInteractable : public UInterface
{
	GENERATED_BODY()
};

class HEAVENLYBLUE_API IIBaseInteractable
{
	GENERATED_BODY()

public:
	IIBaseInteractable();

	FInteractableInfo CurrentItem;
	EInteractablePhase CurrentPhase;

	virtual void PrintInteractableName(FInteractableInfo InteractableItem);
	virtual void PrintInteractableDescription(FInteractableInfo InteractableItem);
	virtual void Traverse(FInteractableInfo InteractableItem);
	virtual void HandleQuestion(FInteractableInfo InteractableItem);
	virtual void Refresh(FInteractableInfo InteractableItem);
	virtual void TrueCondition();
	virtual void FalseCondition();

	int32 InputIndex;
	bool bFinished;
	bool bProceed;
protected:
private:
};
