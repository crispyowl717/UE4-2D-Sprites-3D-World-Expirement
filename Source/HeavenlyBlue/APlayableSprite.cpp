#include "APlayableSprite.h"
#include "Engine/Engine.h"

/*
 * Function:  AAPlayableSprite
 * --------------------
 * This creates the base functionality of the APlayableSprite class.
 *
 */
AAPlayableSprite::AAPlayableSprite() :
CurCapsuleSettings(130.0f, 40.0f),
DEAD_ZONE(0.5),
CurSpringArmIndex(0),
MouseSensitivity(9.0f),
bInAlternativeState(false)
{
	if(ConversationCollection.IsValidIndex(0))
		ConversationCollection[0]->SetNodeID(0, 0, 0);

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	SetActorEnableCollision(true);	

	// Base Sprite Settings
	GetSprite()->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	GetSprite()->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	GetCapsuleComponent()->SetCapsuleSize(CurCapsuleSettings.Y, CurCapsuleSettings.X);

	// Component Initialization 
	if(!RootComponent)
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Spring Arm Settings
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 6.0f;
	SpringArm->bDrawDebugLagMarkers = true;

	//Camera Base Settings
	FollowCamera = CreateDefaultSubobject<UAFollowCamera>(TEXT("Camera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	//Exclamation Icon Settings
	ExclamationIcon = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Exclamation Icon"));
	ExclamationIcon->SetupAttachment(GetSprite());
	ExclamationIcon->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	ExclamationIcon->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	ExclamationIcon->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	ExclamationIcon->SetVisibility(false);

}

/*
 * Function:  BeginPlay
 * --------------------
 * This is created the moment the object is instantiated in the scene.
 *
 */
void AAPlayableSprite::BeginPlay()
{
	Super::BeginPlay();
	GEngine->GameViewport->Viewport->LockMouseToViewport(true);


	// The GetAllActorsOfClass requires the output be an actor just like that of the orignal class.
	// To directly reference a AConversationInstance object, the FoundConversationInstances must be casted to it.
	TArray<AActor*> FoundConversationInstances;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAConversationInstance::StaticClass(), FoundConversationInstances);
	for (auto FoundConversationCollection : FoundConversationInstances)
	{
		AAConversationInstance* Output = Cast<AAConversationInstance>(FoundConversationCollection);

		if (Output != nullptr)
			ConversationCollection.Add(Output);
	}

	TArray<AActor*> FoundInfoBoxInstance;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAInfoBox::StaticClass(), FoundInfoBoxInstance);
	for (auto FoundInfoBoxCollection : FoundInfoBoxInstance)
	{
		AAInfoBox* Output = Cast<AAInfoBox>(FoundInfoBoxCollection);

		if (Output != nullptr)
			InfoBoxCollection.Add(Output);
	}

}

/*
 * Function:  Message
 * --------------------
 * This is the function that is called at each frame.
 *
 * Statment: This is the message that will be output to the screen.
 *
 */
void AAPlayableSprite::Message(FString Statement)
{
	//Get it? It's cyan because the game is called Heavenly Blue. I'm W O K E.
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Statement);
}

/*
 * Function:  Tick
 * --------------------
 * This is the function that is called at each frame.
 * This is used for functions/actions that constantly update.
 *
 * DeltaTime: This measures the amount of time between any two frames.
 *
 */
void AAPlayableSprite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetSpriteAnimation(CurDirection, CurSpriteState);

	// During the diagonal movement, both the X and Y vectors are combined thus making the character move 
	// double the normal speed, this is fixed by normalizing the vector.
	MovementDisplacement.Normalize();

	// The spring arm's axis are changed to better represent how they appear visually in the blueprint editor
	SpringArm->SetRelativeLocation(FVector(-SpringArmDetails[CurSpringArmIndex].CustomTargetPosition.Z, 
										    -SpringArmDetails[CurSpringArmIndex].CustomTargetPosition.X, 
										   -SpringArmDetails[CurSpringArmIndex].CustomTargetPosition.Y));

	SpringArm->TargetArmLength = SpringArmDetails[CurSpringArmIndex].CustomTargetArmLength;

	// The sprite needs to be facing the camera at all times.
	// Using GetPlayerCameraManager function, you're able to find the current active camera without specifing it directly.
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation());
	// This finds the rotation of the sprite, removes the pitch because the sprite should not rotate to follow camera's vertical poisiton
	// subtracts 90 from Yaw because the sprite begins with a yaw of +90, and keeps roll.
	FRotator Convert = FRotator(0.0f, PlayerRot.Yaw-90.0f, PlayerRot.Roll);
	GetSprite()->SetRelativeRotation(Convert);

	if (ConversationCollection.IsValidIndex(0))
	{
		for (int i = 0; i < ConversationCollection.Num(); i++)
		{
			if (ConversationCollection[i]->bInCollision)
				ConversationCollectionID = i;

			if (!bInAlternativeState && ConversationCollection[i]->bInCollision)
			{
				ExclamationIcon->SetVisibility(true);
			}

			if (bInAlternativeState || !ConversationCollection[i]->bInCollision || (FMath::Abs(CurYaw) > 0.0f))
			{
				ExclamationIcon->SetVisibility(false);
			}
		}
	}

	if (InfoBoxCollection.IsValidIndex(0))
	{
		for (int i = 0; i < InfoBoxCollection.Num(); i++)
		{
			if (InfoBoxCollection[i]->bInCollision)
				InfoBoxCollectionID = i;

			if (!bInAlternativeState && InfoBoxCollection[i]->bInCollision)
			{
				ExclamationIcon->SetVisibility(true);
			}
			
			if (InfoBoxCollection[InfoBoxCollectionID]->CurrentItem.bHasQuestion && 
					InfoBoxCollection[InfoBoxCollectionID]->bFinished)
			{
				ExclamationIcon->SetVisibility(false);
			}

			if (bInAlternativeState || !InfoBoxCollection[i]->bInCollision || (FMath::Abs(CurYaw) > 0.0f))
			{
				ExclamationIcon->SetVisibility(false);
			}
		}
	}
}


 /* Function:  SetSpriteAnimation
 * --------------------
 * This uses the resulting array index from the FindArrayIndex method, and sets the corresponding animation.
 *
 * Dir: The sprite changes direction based on camera position. The directions are split into the cardinal directions.
 * State: The sprite is able to do actions. The current action the player is doing is considered the state.
 *
 */
void AAPlayableSprite::SetSpriteAnimation(EMainSpriteDirection Dir, EMainSpriteState State)
{
	if (FindArrayIndex(CurDirection, CurSpriteState) < SpriteDetails.Num())
	{
		GetSprite()->SetFlipbook(SpriteDetails[FindArrayIndex(CurDirection, CurSpriteState)].PFB_Animation);
		GetCapsuleComponent()->SetCapsuleSize(CurCapsuleSettings.Y, CurCapsuleSettings.X);
	}
}

/*
/*
 * Function:  FindArrayIndex
 * --------------------
 * Because the animations are based on a indices, instead of manually inputting a index,
 * this allows you to just use the current direction and state and it'll find the corresponding index.
 *
 * Dir: The sprite changes direction based on camera position. The directions are split into the cardinal directions.
 * State: The sprite is able to do actions. The current action the player is doing is considered the state.
 *
 */
int32 AAPlayableSprite::FindArrayIndex(EMainSpriteDirection Dir, EMainSpriteState State)
{
	int32 tempArrayIndex = 0;

	for (int i = 0; i < SpriteDetails.Num(); i++)
	{
		if (SpriteDetails[i].Direction == Dir && SpriteDetails[i].SpriteState == State)
		{
			tempArrayIndex = i;
			CurDirection = SpriteDetails[i].Direction;
			CurSpriteState = SpriteDetails[i].SpriteState;
		}
	}

	return tempArrayIndex;
}


/*
 * Function:  SetupPlayerInputComponent
 * --------------------
 * This binds the inputs decided by the project settings to callable functions.
 *
 * InternalInputComponent: This allows an actor to receive input.
 *
 * Fun Fact: The parameter name must be different than it's default "InputComponent" name.
 */
void AAPlayableSprite::SetupPlayerInputComponent(class UInputComponent* InternalInputComponent)
{
	Super::SetupPlayerInputComponent(InternalInputComponent);

	// Movement Input
	InternalInputComponent->BindAxis("Vertical", this, &AAPlayableSprite::VerticalMovement);
	InternalInputComponent->BindAxis("Horizontal", this, &AAPlayableSprite::HorizontalMovement);
	InternalInputComponent->BindAxis("TurnHorizontal", this, &AAPlayableSprite::YawRotation);

	//Inventory
	InternalInputComponent->BindAction("Inventory", IE_Pressed, this, &AAPlayableSprite::InventorySelected);

	//Menu Selected
	InternalInputComponent->BindAction("Menu", IE_Pressed, this, &AAPlayableSprite::MenuSelected);

	// Interact 
	InternalInputComponent->BindAction("Interact", IE_Pressed, this, &AAPlayableSprite::InteractSelected);
	InternalInputComponent->BindAction("Interact", IE_Released, this, &AAPlayableSprite::InteractReleased);

	// Sprint
	InternalInputComponent->BindAction("Sprint", IE_Pressed, this, &AAPlayableSprite::SprintSelected);
	InternalInputComponent->BindAction("Sprint", IE_Repeat, this, &AAPlayableSprite::SprintSelected);
	InternalInputComponent->BindAction("Sprint", IE_Released, this, &AAPlayableSprite::SprintReleased);

	//Debug
	InternalInputComponent->BindAction("Option1", IE_Pressed, this, &AAPlayableSprite::Option1Selected);
	InternalInputComponent->BindAction("Option1", IE_Released, this, &AAPlayableSprite::Option1Released);
	InternalInputComponent->BindAction("Option2", IE_Pressed, this, &AAPlayableSprite::Option2Selected);
	InternalInputComponent->BindAction("Option2", IE_Released, this, &AAPlayableSprite::Option2Released);
}

/*
 * Function:  VerticalMovement
 * --------------------
 * Although this refers to the Z axis, that axis is mapped to 'Y'.
 * This function affects movement to the Z axis.
 *
 * AxisValue: The vertical input ranges from a -1 to 1. The current input is axis value.
 *
 */
void AAPlayableSprite::VerticalMovement(float AxisValue)
{
	if (!bInAlternativeState)
	{
		EMainSpriteDirection TempDirection = CurDirection;
		EMainSpriteState TempState = CurSpriteState;

		MovementDisplacement.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
		AddMovementInput(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector(), MovementDisplacement.Y);

		// Your direction is forward if you're only moving forward
		if (MovementDisplacement.Y > DEAD_ZONE && FMath::Abs(MovementDisplacement.X) < DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_Forward;
		// Your direction is backward if you're only moving backward
		else if (MovementDisplacement.Y < -DEAD_ZONE && FMath::Abs(MovementDisplacement.X) < DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_Backward;

		// This is important because this needs to be saved so the direction snaps to last location if the camera hasn't moved
		PrevDirection = TempDirection;

		//If you aren't moving, you're idle
		if (FMath::Abs(MovementDisplacement.Y) < DEAD_ZONE)
			TempState = EMainSpriteState::SA_Idle;
		//If you are moving, you're walking
		else
			TempState = EMainSpriteState::SA_Walking;

		//If you aren't moving horizontally, then find the array index given the current state and direction
		if (FMath::Abs(MovementDisplacement.X) < DEAD_ZONE)
			FindArrayIndex(TempDirection, TempState);
		//If you are moving in both horizontal and vertical directions, then you're moving diagonal 
		else
			DirectionalMovement();
	}	
}

/*
 * Function:  HorizontalMovement
 * --------------------
 * This function affects movement to the X axis.
 *
 * AxisValue: The horizontal input ranges from a -1 to 1. The current input is axis value.
 *
 */
void AAPlayableSprite::HorizontalMovement(float AxisValue)
{
	if (!bInAlternativeState)
	{
		//For comments, refer to VerticalMovement class
		MovementDisplacement.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
		AddMovementInput(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorRightVector(), (MovementDisplacement.X * 0.8f));

		EMainSpriteDirection TempDirection = CurDirection;
		EMainSpriteState TempState = CurSpriteState;

		if (MovementDisplacement.X > DEAD_ZONE && FMath::Abs(MovementDisplacement.Y) < DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_Right;
		else if (MovementDisplacement.X < -DEAD_ZONE && FMath::Abs(MovementDisplacement.Y) < DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_Left;

		// This is important because this needs to be saved so the direction snaps to last location if the camera hasn't moved
		PrevDirection = TempDirection;

		if (FMath::Abs(MovementDisplacement.X) < DEAD_ZONE)
			TempState = EMainSpriteState::SA_Idle;
		else
			TempState = EMainSpriteState::SA_Walking;

		if (FMath::Abs(MovementDisplacement.Y) < DEAD_ZONE)
			FindArrayIndex(TempDirection, TempState);
		else
			DirectionalMovement();
	}
}

/*
 * Function:  DirectionalMovement
 * --------------------
 * This functions addresses situations where the sprite is moving both horizontally and vertically.
 * 
 */
void AAPlayableSprite::DirectionalMovement()
{
	if (!bInAlternativeState)
	{
		EMainSpriteDirection TempDirection = CurDirection;
		EMainSpriteState TempState = CurSpriteState;

		if (MovementDisplacement.X < -DEAD_ZONE && MovementDisplacement.Y > DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_ForwardLeft;
		else if (MovementDisplacement.X > DEAD_ZONE && MovementDisplacement.Y > DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_ForwardRight;
		else if (MovementDisplacement.X < -DEAD_ZONE && MovementDisplacement.Y < -DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_BackwardLeft;
		else if (MovementDisplacement.X > DEAD_ZONE && MovementDisplacement.Y < -DEAD_ZONE)
			TempDirection = EMainSpriteDirection::SD_BackwardRight;

		// This is important because this needs to be saved so the direction snaps to last location if the camera hasn't moved
		PrevDirection = TempDirection;

		if (FMath::Abs(MovementDisplacement.Y) < DEAD_ZONE && FMath::Abs(MovementDisplacement.X) < DEAD_ZONE)
			TempState = EMainSpriteState::SA_Idle;
		else
			TempState = EMainSpriteState::SA_Walking;

		FindArrayIndex(TempDirection, TempState);
	}
}

/*
 * Function:  YawRotation
 * --------------------
 * This function affects horizontal camera rotation.
 *
 * AxisValue: The camera displacement ranges from a -1 to 1. The current input is axis value.
 *
 */
void AAPlayableSprite::YawRotation(float AxisValue)
{
	// The rotation values are altered because of the orientation of the character in world space 
	if (!bInAlternativeState)
	{
		CurYaw = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
		// MOUSE_SENSITIVITY functions as the speed value.
		SpringArmDetails[CurSpringArmIndex].CustomTargetRotation.Roll -= (CurYaw * MouseSensitivity);
		SpringArm->SetRelativeRotation(FRotator(-SpringArmDetails[CurSpringArmIndex].CustomTargetRotation.Pitch,
												-SpringArmDetails[CurSpringArmIndex].CustomTargetRotation.Roll,
												-SpringArmDetails[CurSpringArmIndex].CustomTargetRotation.Yaw));

		float SpringArmRotation;
		// Because the input axis is -1 to 1, going left of 0 degrees makes it -180 to 0 from middle to left.
		if (SpringArm->GetDesiredRotation().Yaw < 0.0f && SpringArm->GetDesiredRotation().Yaw > -180)
			SpringArmRotation = SpringArm->GetDesiredRotation().Yaw + 360.0f;
		else
			SpringArmRotation = SpringArm->GetDesiredRotation().Yaw;

		//The character should change direction if the camera rotates and the player is not manually chaning the direction via movement.
		if (FMath::Abs(MovementDisplacement.X) == 0.0f && FMath::Abs(MovementDisplacement.Y) == 0.0f && SpringArm->GetDesiredRotation().Yaw != 0.0f)
		{
			// It should remain as the direction of the last movement
			if (CurYaw != 0.0f)
			{
				// 8-Cardinal Directions, so each direction ranges has 360/8
				if (SpringArmRotation >= 0.0f && SpringArmRotation < 45.0f)
					CurDirection = EMainSpriteDirection::SD_Forward;
				else if (SpringArmRotation >= 45.0f && SpringArmRotation < 90.0f)
					CurDirection = EMainSpriteDirection::SD_ForwardRight;
				else if (SpringArmRotation >= 90.0f && SpringArmRotation < 135.0f)
					CurDirection = EMainSpriteDirection::SD_Right;
				else if (SpringArmRotation >= 135.0f && SpringArmRotation < 180.0f)
					CurDirection = EMainSpriteDirection::SD_BackwardRight;
				else if (SpringArmRotation >= 180.0f && SpringArmRotation < 225.0f)
					CurDirection = EMainSpriteDirection::SD_Backward;
				else if (SpringArmRotation >= 225.0f && SpringArmRotation < 270.0f)
					CurDirection = EMainSpriteDirection::SD_BackwardLeft;
				else if (SpringArmRotation >= 270.0f && SpringArmRotation < 315.0f)
					CurDirection = EMainSpriteDirection::SD_Left;
				else if (SpringArmRotation >= 315.0f && SpringArmRotation <= 360.0f)
					CurDirection = EMainSpriteDirection::SD_ForwardLeft;
			}
			else
			{
				// Keep the direction the way it was if you haven't rotated the camera or didn't move.
				CurDirection = PrevDirection;
			}
		}
	}
}

/*
 * Function:  INPUT FUNCTIONALITY FUNCTIONS (Selected/Released)
 * --------------------
 * These functions below are the functions called when pressing or releasing a button
 *
 *
 */
void AAPlayableSprite::InventorySelected() { Message("Inventory Selected"); }

void AAPlayableSprite::InventoryReleased(){ Message("Inventory Released"); }

void AAPlayableSprite::InteractSelected()
{
	// We should only be cheking for a collection of conversations if there exists at least 1 conversation
	if (ConversationCollection.IsValidIndex(0))
		BeginConversationInteraction();
	if (InfoBoxCollection.IsValidIndex(0))
		BeginInfoBoxInteraction();
}

void AAPlayableSprite::InteractReleased()
{
	if (ConversationCollection.IsValidIndex(0))
		FinishConversationInteraction();
	if (InfoBoxCollection.IsValidIndex(0))
		FinishInfoBoxInteraction();
}

void AAPlayableSprite::MenuSelected() { Message("Menu Selected"); }

void AAPlayableSprite::MenuReleased() { Message("Menu Releases"); }

void AAPlayableSprite::SprintSelected()
{
	Message("Sprint Pressed");
	GetCharacterMovement()->MaxWalkSpeed = 660.0f;
	MouseSensitivity = 7.0f;
}

void AAPlayableSprite::SprintReleased()
{
	Message("Sprint Released");
	GetCharacterMovement()->MaxWalkSpeed = 360.0f;
	MouseSensitivity = 9.0f;
}

void AAPlayableSprite::Option1Selected() {
	Message("Option 1 Selected");
	Choice = 1;
	if (ConversationCollection.IsValidIndex(ConversationCollectionID))
		ConversationCollection[ConversationCollectionID]->CurrentQuestionIteration = Choice;
	if (InfoBoxCollection.IsValidIndex(InfoBoxCollectionID))
		InfoBoxCollection[InfoBoxCollectionID]->InputIndex = Choice;
}

void AAPlayableSprite::Option1Released() { Message("Option 1 Released"); }

void AAPlayableSprite::Option2Selected()
{
	Message("Option 2 Selected");
	Choice = 2;
	if(ConversationCollection.IsValidIndex(ConversationCollectionID))
		ConversationCollection[ConversationCollectionID]->CurrentQuestionIteration = Choice;
	if (InfoBoxCollection.IsValidIndex(InfoBoxCollectionID))
		InfoBoxCollection[InfoBoxCollectionID]->InputIndex = Choice;
}

void AAPlayableSprite::Option2Released() { Message("Option 2 Released"); }

/*
 * Function: BEGIN INTERACTION FUNCTIONS
 * --------------------
 * These functions map the player input to interaction functions.
 *
 */
void AAPlayableSprite::BeginConversationInteraction()
{
	// Set the conversation ID to be whatever the player is currently in collision with.
	for (int i = 0; i < ConversationCollection.Num(); i++)
	{
		if (ConversationCollection[i]->bInCollision)
			ConversationCollectionID = i;
	}

	// This checks if you aren't finished with the conversation, but you are in a collision, and there exists an index 
	// for a collection ID in the conversation array.
	if (ConversationCollection.IsValidIndex(ConversationCollectionID) &&
		ConversationCollection[ConversationCollectionID]->bInCollision &&
		!ConversationCollection[ConversationCollectionID]->bFinished)
	{
		CurSpriteState = EMainSpriteState::SA_Interact;
		FindArrayIndex(CurDirection, CurSpriteState);

		bInAlternativeState = true;

		if (ConversationCollection[ConversationCollectionID]->bProceed && !ConversationCollection[ConversationCollectionID]->bInQuestion)
		{
			ConversationCollection[ConversationCollectionID]->TraverseDialouge(ConversationCollection[ConversationCollectionID]->ConversationList);
		}
		// This is used for the event that the player doesn't click on a correct response (this shouldn't happen in GUI)
		else if (!ConversationCollection[ConversationCollectionID]->bProceed && ConversationCollection[ConversationCollectionID]->bInQuestion)
		{
			ConversationCollection[ConversationCollectionID]->HandleQuestions(ConversationCollection[ConversationCollectionID]->QuestionList,
				ConversationCollection[ConversationCollectionID]->CurrentSubtitleNodeID,
				ConversationCollection[ConversationCollectionID]->CurrentDialogueNodeID,
				ConversationCollection[ConversationCollectionID]->CurrentConversationNodeID,
				ConversationCollection[ConversationCollectionID]->CurrentQuestionIteration);
		}
		// This suggests you're clicking interact without a specific reason to.
		else if (!ConversationCollection[ConversationCollectionID]->bProceed && !ConversationCollection[ConversationCollectionID]->bInQuestion)
		{
			ConversationCollection[ConversationCollectionID]->bSkippedText = true;
		}
	}
}

/*
 * Function:  FINISH INTERACTION FUCNTIONS
 * --------------------
 * This refers to the behavior after the interaction is complete.
 *
 */
void AAPlayableSprite::FinishConversationInteraction()
{
	if (ConversationCollection[ConversationCollectionID]->bFinished)
	{
		bInAlternativeState = false;
		CurSpriteState = EMainSpriteState::SA_Idle;

		if (ConversationCollection[ConversationCollectionID]->bAllowRepeat)
		{
			ConversationCollection[ConversationCollectionID]->NextConversationNodeID = 0;
			ConversationCollection[ConversationCollectionID]->CurrentConversationNodeID = 0;
			ConversationCollection[ConversationCollectionID]->CurrentDialogueNodeID = 0;
			ConversationCollection[ConversationCollectionID]->CurrentSubtitleNodeID = 0;
			ConversationCollection[ConversationCollectionID]->CurrentLetterIteration = 0;
			ConversationCollection[ConversationCollectionID]->CurrentQuestionIteration = 0;
			ConversationCollection[ConversationCollectionID]->bProceed = true;
			ConversationCollection[ConversationCollectionID]->bInQuestion = false;
			ConversationCollection[ConversationCollectionID]->bFinished = false;
		}
	}
}

void AAPlayableSprite::BeginInfoBoxInteraction()
{
	// Set the conversation ID to be whatever the player is currently in collision with.
	for (int i = 0; i < InfoBoxCollection.Num(); i++)
	{
		if (InfoBoxCollection[i]->bInCollision)
			InfoBoxCollectionID = i;
	}

	// This checks if you aren't finished with the conversation, but you are in a collision, and there exists an index 
	// for a collection ID in the conversation array.
	if (InfoBoxCollection.IsValidIndex(InfoBoxCollectionID) &&
		InfoBoxCollection[InfoBoxCollectionID]->bInCollision &&
		!InfoBoxCollection[InfoBoxCollectionID]->bFinished)
	{
		CurSpriteState = EMainSpriteState::SA_Interact;
		FindArrayIndex(CurDirection, CurSpriteState);

		bInAlternativeState = true;

		InfoBoxCollection[InfoBoxCollectionID]->Traverse(InfoBoxCollection[InfoBoxCollectionID]->CurrentItem);
	}
}

void AAPlayableSprite::FinishInfoBoxInteraction()
{
	if (InfoBoxCollection[InfoBoxCollectionID]->bFinished)
	{
		bInAlternativeState = false;
		CurSpriteState = EMainSpriteState::SA_Idle;

		if (!InfoBoxCollection[InfoBoxCollectionID]->CurrentItem.bHasQuestion)
		{
			InfoBoxCollection[InfoBoxCollectionID]->bProceed = false;
			InfoBoxCollection[InfoBoxCollectionID]->bFinished = false;
			InfoBoxCollection[InfoBoxCollectionID]->InputIndex = 0;
		}
	}
}

