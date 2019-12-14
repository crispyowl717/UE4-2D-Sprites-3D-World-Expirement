// Fill out your copyright notice in the Description page of Project Settings.


#include "AConversationInstance.h"
#include "Engine/Engine.h"

/*
 * Function:  AConversationInstance
 * --------------------
 * This is the constructor. It does nothing right now lol.
 */
AAConversationInstance::AAConversationInstance() : 
bInCollision(false),
bSkippedText(false), 
bAllowRepeat (true)
{}

/*
 * Function:  BeginPlay
 * --------------------
 * This is called soon as the object is instaniated. 
 */
void AAConversationInstance::BeginPlay()
{
	// These allow the conversation instance to recognize collison with an actor.
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAConversationInstance::OnBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AAConversationInstance::OnEndOverlap);
	PrimaryActorTick.bCanEverTick = true;
}

/*
 * Function:  PrintSubtitle
 * --------------------
 * This function sets the speaker and begins the typewriting process
 *
 *
 */
void AAConversationInstance::PrintSubtitle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Name: " + CurrentSpeakerName);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Text: " + CurrentSubtitleText);


	// Check if there's a question here
	if (ConversationList[CurrentConversationNodeID].DialougeNodes[CurrentDialogueNodeID].SubtitlesNodes[CurrentSubtitleNodeID].bHasQuestion)
	{
		if (!bInQuestion)
			PrintQuestions(QuestionList, CurrentSubtitleNodeID, CurrentDialogueNodeID, CurrentConversationNodeID);

		bProceed = false;
		bInQuestion = true;
		CurrentLetterIteration = 0;

		HandleQuestions(QuestionList, CurrentSubtitleNodeID, CurrentDialogueNodeID, CurrentConversationNodeID, CurrentQuestionIteration);
	}
	else
	{
		bProceed = true;
		bInQuestion = false;
		CurrentLetterIteration = 0;
		CurrentQuestionIteration = 0;

		Increment(ConversationList);
	}

	//TypewriterEffect(CurrentSubtitleText);
}

/*
 * Function:  TypewriterEffect
 * --------------------
 * This prints letter by letter, and after the process is complete, it checks if you're at a question or not.
 *
 * CurString: This is the text of the current subtitle node
 */
void AAConversationInstance::TypewriterEffect(FString CurString)
{
	if (CurrentLetterIteration < CurString.Len())
	{
		AddLetter(GetLetter(CurString, CurrentLetterIteration), (CurrentSubtitleTimer / CurString.Len()));
	}
	else
	{
		// Check if there's a question here
		if (ConversationList[CurrentConversationNodeID].DialougeNodes[CurrentDialogueNodeID].SubtitlesNodes[CurrentSubtitleNodeID].bHasQuestion)
		{
			if (!bInQuestion)
				PrintQuestions(QuestionList, CurrentSubtitleNodeID, CurrentDialogueNodeID, CurrentConversationNodeID);

			bProceed = false;
			bInQuestion = true;
			CurrentLetterIteration = 0;

			HandleQuestions(QuestionList, CurrentSubtitleNodeID, CurrentDialogueNodeID, CurrentConversationNodeID, CurrentQuestionIteration);
		}
		else
		{
			bProceed = true;
			bInQuestion = false;
			CurrentLetterIteration = 0;
			CurrentQuestionIteration = 0;

			Increment(ConversationList);
		}
	}
}

/*
 * Function:  GetLetter
 * --------------------
 * This returns the character of a complete string at a specific index.
 *
 * CurString: This is the text of the current subtitle node.
 * Start: This is the index to return the character.
 *
 */
FString AAConversationInstance::GetLetter(FString CurString, int32 Start){	return CurString.Mid(Start, 1); }

/*
 * Function:  AddLetter
 * --------------------
 * This returns the character of a complete string at a specific index.
 *
 * Letter: This is the letter recieved from the GetLetter function.
 * Start: This is how fast should the typewriter type.
 *
 */
void AAConversationInstance::AddLetter(FString Letter, float Time)
{	
	CurrentLetter = Letter;
	if(!bSkippedText)
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAConversationInstance::TimerEnd, Time, false);
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, CurrentLetter);
		UGameplayStatics::PlaySound2D(GetWorld(), CurrentSubtitleVoice);
		CurrentLetterIteration++;
		TypewriterEffect(CurrentSubtitleText);
	}
}

/*
 * Function:  TimerEnd
 * --------------------
 * This is the function that is called after the timer is completed in AddLetter.
 * This is also responsible for handling an single letter and returning back to Typewriter recursively. 
 *
 */
void AAConversationInstance::TimerEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, CurrentLetter);
	UGameplayStatics::PlaySound2D(GetWorld(), CurrentSubtitleVoice);
	CurrentLetterIteration++;
	TypewriterEffect(CurrentSubtitleText);
}

/*
 * Function:  OnBeginOverlap
 * --------------------
 * This is called when an actor enters a collision box.
 */
void AAConversationInstance::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		bInCollision = true;
	}
}

/*
 * Function:  OnEndOverlap
 * --------------------
 * This is called when an actor exits a collision box.
 */
void AAConversationInstance::OnEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		bInCollision = false;
	}
}
