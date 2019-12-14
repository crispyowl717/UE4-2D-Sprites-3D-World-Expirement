/*
**********************************************************************;
*	Project      : HeavenlyBlue
*
*	Program name : IDialogueTree
*
*	Author		 : ResponsibleFile (Dawson McThay)
*
*	Date created : 10/08/2019
*
*	Purpose		 : This class is designed to create a unique instance of a dialogue
*				   interaction.
*
*	Revisions	 : 11/02/2019
*
**********************************************************************;
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericPlatform/GenericPlatformProcess.h" 
#include "IDialogueTree.generated.h"

/*
 * Struct:  FQuestion
 * --------------------
 * Each node contains options (option name), and it checks if a specific dialouge and subtitle contains a question.
 * It also can set global bools to be true or false (important for quests).
 */
USTRUCT(BlueprintType)
struct FQuestionNode // Optional Forth Level
{
	GENERATED_USTRUCT_BODY()

	// Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle Properties")
	FString Option;

	// Node Identication
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 SubtitleRefrenceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 DialougeReferenceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 ConversationReferenceID;
	// Node Identication
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 NodeID;

	// Node Identication
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 GoToConversationNodeID;
};


/*
 * Struct:  FSubtitleNode
 * --------------------
 * This describes the text, print time, if it should lead into a question, and the ID of a single subtitle
 *
 */
USTRUCT(BlueprintType)
struct FSubtitleNode // Third Level
{
	GENERATED_USTRUCT_BODY()

	// Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle Properties")
	FString SubtitleText;

	// Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle Properties")
	float SubtitleTimer;

	// Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle Properties")
	bool bHasQuestion;

	// Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Subtitle Properties")
	class USoundWave* SubtitleSound;

	// Node Identication
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 NodeID;
};

/*
 * Struct:  FDialougeNode
 * --------------------
 * This describes the name, ID, and subtitle colletion for a single dialouge.
 *
 */
USTRUCT(BlueprintType)
struct FDialogueNode // Second Level
{
	GENERATED_USTRUCT_BODY()

	// Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Properties")
	FString SpeakerName;

	// Node Identication
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 NodeID;

	// Children Nodes
	// Note: This would be a pointer if TArray didn't already allocate the memory. 
	// This is also why we can't check if index == nullptr.	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Properties")
	TArray<struct FSubtitleNode> SubtitlesNodes;
};

/*
 * Struct:  FConversationNode
 * --------------------
 * This is primarily used handle the collection of all dialouges.
 * In the context of a linked list, this would be the root node.
 *
 */
USTRUCT(BlueprintType)
struct FConversationNode // First Level (Root)
{
	GENERATED_USTRUCT_BODY()

	// Node Identication
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	int32 NodeID;

	// Children Nodes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Properties")
	TArray<struct FDialogueNode> DialougeNodes;
};

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UIDialogueTree : public UInterface
{
	GENERATED_BODY()
};

class HEAVENLYBLUE_API IIDialogueTree
{

	GENERATED_BODY()

public:
	IIDialogueTree();

	virtual void SetNodeID(int32 convo, int32 dia, int32 sub);
	virtual void ResetIteration();
	virtual int32 GetDialougeListSize(TArray<struct FDialogueNode> List);
	virtual int32 GetSubtitlesListSize(TArray<struct FSubtitleNode> List);

	virtual void TraverseDialouge(TArray <FConversationNode> List);
	virtual void TraverseSubtitle(TArray<struct FDialogueNode> List);
	virtual void SetSubtitleProperties(TArray<struct FSubtitleNode> List);
	virtual void HandleQuestions(TArray<struct FQuestionNode> List, int32 SubtitleID, int32 DialougeNode, int32 ConversationNode, int32 Input);

	// Print Nodes
	virtual void PrintSubtitle();
	virtual void PrintQuestions(TArray<struct FQuestionNode> List, int32 SubtitleNode, int32 DialougeNode, int32 ConversationNode);

	// This progresses to new subtitle/dialouge
	virtual void Increment(TArray<FConversationNode> List);

	// The base collection of root nodes
	TArray <FConversationNode> ConversationList;
	TArray<FQuestionNode> QuestionList;

	// Refrences to current node information and IDs
	FString CurrentSubtitleText;
	FString CurrentSpeakerName;
	float CurrentSubtitleTimer;
	int32 CurrentConversationNodeID;
	int32 NextConversationNodeID;
	int32 CurrentDialogueNodeID;
	int32 CurrentSubtitleNodeID;
	int32 CurrentLetterIteration;
	int32 CurrentQuestionIteration;
	class USoundWave* CurrentSubtitleVoice;

	bool bFinished;
	bool bProceed;
	bool bInQuestion;

protected:
private:
};
