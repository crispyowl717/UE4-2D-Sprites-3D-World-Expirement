// Fill out your copyright notice in the Description page of Project Settings.


#include "IBaseInteractable.h"

/*
 * Function:  IIBaseInteractable
 * --------------------
 * This creates the base functionality of the IIBaseInteractable class.
 *
 */
IIBaseInteractable::IIBaseInteractable() :
bProceed(false),
CurrentPhase(EInteractablePhase::SD_NO_OVERLAP)
{}

/*
 * Function:  Refresh
 * --------------------
 * This changes the behavior of the object after a traversal
 *
 */
void IIBaseInteractable::Refresh(FInteractableInfo InteractableItem)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Interact called");
	switch (CurrentPhase)
	{
		case EInteractablePhase::SD_NO_OVERLAP:
		{
			bProceed = true;
			CurrentPhase = EInteractablePhase::SD_OVERLAP;
			break;
		}

		case EInteractablePhase::SD_OVERLAP:
		{
			bProceed = true;
			CurrentPhase = EInteractablePhase::SD_ACTIVE;
			break;
		}

		case EInteractablePhase::SD_ACTIVE:
		{
			if (InteractableItem.bHasQuestion)
				CurrentPhase = EInteractablePhase::SD_QUESTION;
			else
				CurrentPhase = EInteractablePhase::SD_EXIT;
			bProceed = true;
			break;
		}

		case EInteractablePhase::SD_QUESTION:
		{
			bProceed = true;
			break;
		}

		case EInteractablePhase::SD_EXIT:
		{
			bProceed = false;
			bFinished = true;
			break;
		}
	}
}

/*
 * Function:  PRINT
 * --------------------
 * This creates these are the results of the names and descriptions.
 *
 */
void IIBaseInteractable::PrintInteractableName(FInteractableInfo InteractableItem)
{
	if (CurrentPhase == EInteractablePhase::SD_OVERLAP)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Item Name: " + InteractableItem.ItemName);
}

void IIBaseInteractable::PrintInteractableDescription(FInteractableInfo InteractableItem)
{
	if (CurrentPhase == EInteractablePhase::SD_ACTIVE)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Item Description: " + InteractableItem.ItemDescription);
}

/*
 * Function:  Traverse
 * --------------------
 * This controls the behavior of the states.
 *
 */
void IIBaseInteractable::Traverse(FInteractableInfo InteractableItem)
{
	PrintInteractableDescription(InteractableItem);
	if (InteractableItem.bHasQuestion && bProceed)
	{
		HandleQuestion(InteractableItem);
	}
	else
	{
		Refresh(InteractableItem);
	}
}

/*
 * Function: QUESTIONS
 * --------------------
 * This creates the base behavior of the questions
 *
 */
void IIBaseInteractable::HandleQuestion(FInteractableInfo InteractableItem)
{
	CurrentPhase = EInteractablePhase::SD_QUESTION;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "1. B");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "2. N");

	if (InteractableItem.bHasQuestion && CurrentPhase == EInteractablePhase::SD_QUESTION)
	{
		if (InputIndex == 1)
		{
			TrueCondition();
			bProceed = false;
		}
		else if (InputIndex == 2)
		{
			FalseCondition();
			bProceed = false;
		}
		else
		{
			InputIndex = 0;
			bProceed = false;
		}
	}
}

void IIBaseInteractable::TrueCondition()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "True");
	CurrentPhase = EInteractablePhase::SD_EXIT;
}

void IIBaseInteractable::FalseCondition()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "False");
	CurrentPhase = EInteractablePhase::SD_EXIT;
}
