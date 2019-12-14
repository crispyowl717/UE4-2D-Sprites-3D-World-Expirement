/*
**********************************************************************;
*	Project      : HeavenlyBlue
*
*	Program name : AConversationInstance
*
*	Author		 : ResponsibleFile (Dawson McThay)
*
*	Date created : 10/08/2019
*
*	Purpose		 : This class is designed to create a unique instance of a dialogue
*				   interaction between two characters upon entering a collision box.
*
*	Revisions	 : 11/02/2019
*
**********************************************************************;
*/

#pragma once

//Engine Includes
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h" 

//UObject/UAsset Includes
#include "IDialogueTree.h"
#include "TimerManager.h"

//Components
#include "Engine/TriggerBox.h"
#include "Components/ShapeComponent.h"
#include "Engine/World.h" 
#include "GameFramework/Actor.h"

#include "AConversationInstance.generated.h"


UCLASS(Blueprintable)
class HEAVENLYBLUE_API AAConversationInstance : public ATriggerBox, public IIDialogueTree
{
	GENERATED_BODY()

	AAConversationInstance();
public:
	UFUNCTION()
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void PrintSubtitle() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	TArray<FConversationNode> ConversationList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	TArray<FQuestionNode> QuestionList;

	UPROPERTY()
	int32 CurrentLetterIteration;
	UPROPERTY()
	bool bInCollision;
	UPROPERTY()
	bool bSkippedText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	bool bAllowRepeat;
protected:

	
private:
	// The timer handle allows delay features. It requires a function to be called after timer concludes.
	UPROPERTY()
	FTimerHandle TimerHandle;
	UPROPERTY()
	FString CurrentLetter;

	// This begins the process of printing letters on-to the screen.
	UFUNCTION()
	void TypewriterEffect(FString CurString);
	UFUNCTION()
	void AddLetter(FString Letter, float Time);
	UFUNCTION()
	FString GetLetter(FString CompleteString, int32 start);
	UFUNCTION()
	void TimerEnd();

	// Overlap Functions
	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
