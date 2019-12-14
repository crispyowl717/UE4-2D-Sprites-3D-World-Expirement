// Fill out your copyright notice in the Description page of Project Settings.


#include "IDialogueTree.h"
#include "Engine/Engine.h"

/*
 * Function:  IIDialogueTree
 * --------------------
 * This creates the base functionality of the IIDialogueTree class.
 *
 */
IIDialogueTree::IIDialogueTree() : 
CurrentSubtitleText(""),
CurrentSpeakerName(""),
CurrentSubtitleTimer(0.0f),
NextConversationNodeID(0),
CurrentDialogueNodeID(0),
CurrentSubtitleNodeID(0),
CurrentLetterIteration(0),
CurrentQuestionIteration(0),
CurrentSubtitleVoice(nullptr),
bProceed(true)
{}

/*
 * Function:  SetNodeID
 * --------------------
 * This automatically sets the current conversation node, dialouge node, and subtitle node
 * to the newer values.
 *
 */
void IIDialogueTree::SetNodeID(int32 ConversationNode, int32 DialougeNode, int32 SubtitleNode) {
	CurrentConversationNodeID = ConversationNode;
	CurrentDialogueNodeID = DialougeNode;
	CurrentSubtitleNodeID = SubtitleNode;
}

/*
 * Function:  ResetIteration
 * --------------------
 * This is called when the subtitle has no other possible successors.
 *
 */
 void IIDialogueTree::ResetIteration()
{
	CurrentLetterIteration = 0;
	CurrentQuestionIteration = 0;
	bInQuestion = false;
	bProceed = true;
}



 /* Function:  GetDialougeListSize/GetSubtitlesListSize
 * --------------------
 * This does what you think it does.
 *
 */
int32 IIDialogueTree::GetDialougeListSize(TArray<FDialogueNode> List){return List.Num();}

int32 IIDialogueTree::GetSubtitlesListSize(TArray<FSubtitleNode> List){return List.Num();}

/*
 * Function:  TraverseDialouge/TraverseSubtitle
 * --------------------
 * This goes through the entries of the dialouges and subtitles
 *
 */
void IIDialogueTree::TraverseDialouge(TArray<FConversationNode> List)
{
	if (List.IsValidIndex(CurrentConversationNodeID))
	{
		if (List[CurrentConversationNodeID].DialougeNodes.IsValidIndex(CurrentDialogueNodeID) &&
			List[CurrentConversationNodeID].DialougeNodes[CurrentDialogueNodeID].SubtitlesNodes.IsValidIndex(CurrentSubtitleNodeID))
		{
			TraverseSubtitle(List[CurrentConversationNodeID].DialougeNodes);
		}
	}

}

void IIDialogueTree::TraverseSubtitle(TArray<struct FDialogueNode> List)
{
	if (List.IsValidIndex(CurrentDialogueNodeID))
	{
		CurrentSpeakerName = List[CurrentDialogueNodeID].SpeakerName;
		SetSubtitleProperties(List[CurrentDialogueNodeID].SubtitlesNodes);
		PrintSubtitle();
	}
}

/*
 * Function:  SetSubtitleProperties
 * --------------------
 * This sets the current properties of the current subtitle to the one recieved from the new list.
 *
 */
void IIDialogueTree::SetSubtitleProperties(TArray<struct FSubtitleNode> List)
{
	if (List.IsValidIndex(CurrentSubtitleNodeID))
	{
		CurrentSubtitleText = List[CurrentSubtitleNodeID].SubtitleText;
		CurrentSubtitleTimer = List[CurrentSubtitleNodeID].SubtitleTimer;
		CurrentSubtitleVoice = List[CurrentSubtitleNodeID].SubtitleSound;
	}
}

/*
 * Function:  HandleQuestions
 * --------------------
 * This switches to a new dialouge by choosing an option.
 * 
 * List: This is the collection of all questions.
 * SubtitleID/DialougeID: The specific subtitle used to activate the question handling.
 * 
 */
void IIDialogueTree::HandleQuestions(TArray<struct FQuestionNode> List, int32 SubtitleID, int32 DialougeID, int32 ConversationID, int32 Input)
{
	for (int i = 0; i < List.Num(); i++)
	{
		if (Input == List[i].NodeID && SubtitleID == List[i].SubtitleRefrenceID && 
			DialougeID == List[i].DialougeReferenceID && 
			ConversationID == List[i].ConversationReferenceID)
		{
			SetNodeID(List[i].GoToConversationNodeID, 0, 0);
			ResetIteration();
		}
	}
}

/*
 * Function:  PrintSubtitle
 * --------------------
 * This prints the name and subtitle.
 *
 */
void IIDialogueTree::PrintSubtitle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Name: " + CurrentSpeakerName);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Text: ");
}

/*
 * Function:  PrintQuestions
 * --------------------
 * This prints the options for the subtitle with a question.
 *
 */
void IIDialogueTree::PrintQuestions(TArray<struct FQuestionNode> List, int32 SubtitleNode, int32 DialougeNode, int32 ConversationNode)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "");

	for (int i = 0; i < List.Num(); i++)
	{
		if (List[i].SubtitleRefrenceID == SubtitleNode && List[i].DialougeReferenceID == DialougeNode && List[i].ConversationReferenceID == ConversationNode)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, List[i].Option);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "");
}

/*
 * Function:  Increment
 * --------------------
 * 1) Increase the subtitle entry.
 * 2) If the subtitle entry is maxed out, increase the dialouge entry and try again.
 * 3) If there's no more dialouge, then you're done.
 */
void IIDialogueTree::Increment(TArray <FConversationNode> List)
{
	
	if (bProceed && !bInQuestion)
	{
		if (List[CurrentConversationNodeID].DialougeNodes[CurrentDialogueNodeID].SubtitlesNodes.IsValidIndex(CurrentSubtitleNodeID + 1))
		{
			CurrentLetterIteration = 0;
			SetNodeID(CurrentConversationNodeID, CurrentDialogueNodeID, (CurrentSubtitleNodeID + 1));
		}

		else
		{
			if (List[CurrentConversationNodeID].DialougeNodes.IsValidIndex(CurrentDialogueNodeID + 1))
			{
				CurrentLetterIteration = 0;
				SetNodeID(CurrentConversationNodeID, (CurrentDialogueNodeID + 1), 0);
			}
			else
			{
				bFinished = true;
			}
		}
	}
}
