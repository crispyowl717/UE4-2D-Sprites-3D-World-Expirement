/*
**********************************************************************;
*	Project      : HeavenlyBlue
*
*	Program name : APlayableSprite
*
*	Author		 : ResponsibleFile (Dawson McThay)
*
*	Date created : 09/08/2019
*
*	Purpose		 : This class is designed to outline the baseline
*				   functions of a playable sprite character.
*
*	Revisions	 : 11/07/2019
*
**********************************************************************;
*/

#pragma once

//Engine Includes
#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

//UObject/UAsset Includes
#include "PaperCharacter.h"

//Components
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AFollowCamera.h"
#include "AConversationInstance.h"
#include "AInfoBox.h"

//Generated File (Must Be Last)
#include "APlayableSprite.generated.h"

/*
 * Enumeration:  EBaseSpriteDirection
 * --------------------
 * This is designed to represent the direction the sprite is facing
 * in respect to the camera/player input.
 */
UENUM(BlueprintType)
enum class EMainSpriteDirection : uint8
{
	SD_Forward 			UMETA(DisplayName = "Forward"),
	SD_ForwardRight 	UMETA(DisplayName = "ForwardRight"),
	SD_Right 			UMETA(DisplayName = "Right"),
	SD_BackwardRight 	UMETA(DisplayName = "BackwardRight"),
	SD_Backward 		UMETA(DisplayName = "Backward"),
	SD_BackwardLeft 	UMETA(DisplayName = "BackwardLeft"),
	SD_Left 			UMETA(DisplayName = "Left"),
	SD_ForwardLeft 		UMETA(DisplayName = "ForwardLeft")
};


/*
 * Enumeration:  EBaseSpriteState
 * --------------------
 * There are different action states that a sprite can currently be in.
 * These states are dependent on which direction the sprite is currently facing.
 */
UENUM(BlueprintType)
enum class EMainSpriteState : uint8
{
	SA_Idle				UMETA(DisplayName = "Idle"),
	SA_Walking 			UMETA(DisplayName = "Walking"),
	SA_Interact 		UMETA(DisplayName = "Interacting"),
	SA_Sprint			UMETA(DisplayName = "Sprinting")
};

/*
 * Struct:  FBaseSpriteDetails
 * --------------------
 * This is primarily used to handle the animations.
 * It takes in a current direction and a current state.
 * It also contains the radius and half-size as they differ based on animation. 
 * It checks the curDirection and curSpriteState, and from that, it presents an animation
 * By doing this, the struct essentially works likes an dynamic array linked list, and allows
 * these struct components to be visible via. blueprints.
 */
USTRUCT(BlueprintType)
struct FMainSpriteDetails
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite State Properties")
	EMainSpriteDirection Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite State Properties")
	EMainSpriteState SpriteState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite State Properties")
	FVector2D CapsuleSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite State Properties")
	class UPaperFlipbook* PFB_Animation;
};

/*
 * Struct:  FMainSpringArmDetails
 * --------------------
 * This is primarily used to handle the camera's positioning.
 * It takes in a distance represented as TargetArmLength.
 * The TargetPosition refers to the spring arm's end point
 * The TargetRotation refers to the spring arm's start rotation
 */
USTRUCT(BlueprintType)
struct FMainSpringArmDetails
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spring Arm Properties")
	float CustomTargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spring Arm Properties")
	FVector CustomTargetPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spring Arm Properties")
	FRotator CustomTargetRotation;
};

UCLASS()
class HEAVENLYBLUE_API AAPlayableSprite : public APaperCharacter
{
	GENERATED_BODY()

	AAPlayableSprite();

public:

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// This takes in a specific index and changes the current sprite details to
	// that of a specific entry in the complete sprite details array.
	UFUNCTION(BlueprintCallable)
	virtual void SetSpriteAnimation(EMainSpriteDirection Dir, EMainSpriteState State);

	// These variables are describing the current/previous direction and state, as decided by index.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Sprite State Details")
	EMainSpriteDirection CurDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Sprite State Details")
	EMainSpriteDirection PrevDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Sprite State Settings")
	EMainSpriteState CurSpriteState;

	// This is to manually add multiple entries for states, directions, and animations.
	// They are generally referenced by the array index.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite State List")
	TArray<struct FMainSpriteDetails> SpriteDetails;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spring Arm Settings")
	TArray<struct FMainSpringArmDetails> SpringArmDetails;

	// This is the array of objects filled to represent all conversation instances for a map
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Find all Actors of: ")
	TArray<AAConversationInstance*> ConversationCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Find all Actors of: ")
	TArray<AAInfoBox*> InfoBoxCollection;

protected:

	// Note: C++ created components (and other UObjects) should always be visible anywhere.
	// Note: C++ simple datatypes should be edit anywhere.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Spring Arm Settings")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Camera Settings")
	class UAFollowCamera* FollowCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Bonus Settings")
	class UPaperSpriteComponent* ExclamationIcon;

private:
	// The capsule radius and height changes based on the animation
	// X = Half-Height, Y = Radius
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Settings", meta = (AllowPrivateAccess = "True"))
	FVector2D CurCapsuleSettings;

	// This details the velocity of the playable character's movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Settings", meta = (AllowPrivateAccess = "True"))
	FVector2D MovementDisplacement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Movement Settings", meta = (AllowPrivateAccess = "True"))
	float MouseSensitivity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Movement Settings", meta = (AllowPrivateAccess = "True"))
	float CurYaw;

	// This is the amount of stick offset before acknowledgement.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings", meta = (AllowPrivateAccess = "True"))
	float DEAD_ZONE = 0.5f;

	// The spring arm has settings that describe it's base location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spring Arm Settings", meta=(AllowPrivateAccess="True"))
	int32 CurSpringArmIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Find all Actors of: ", meta = (AllowPrivateAccess = "True"))
	int32 ConversationCollectionID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Find all Actors of: ", meta = (AllowPrivateAccess = "True"))
	int32 InfoBoxCollectionID;

	// This exists to keep track of player input while responding to questions.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Player State Setting ", meta = (AllowPrivateAccess = "True"))
	int32 Choice;

	// These are a reference to states dictated by player input.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Player State Settings", meta = (AllowPrivateAccess = "True"))
	bool bInventoryPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Player State Settings", meta = (AllowPrivateAccess = "True"))
	bool bInteractPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Player State Settings", meta = (AllowPrivateAccess = "True"))
	bool bMenuPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Player State Settings", meta = (AllowPrivateAccess = "True"))
	bool bSprintPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Player State Settings", meta = (AllowPrivateAccess = "True"))
	bool bInAlternativeState;

	// These are temporary debugging variables.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug Extras", meta = (AllowPrivateAccess = "True"))
	bool bOptionA;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug Extras", meta = (AllowPrivateAccess = "True"))
	bool bOptionB;

	// These functions control the player's movement.
	UFUNCTION()
	void VerticalMovement(float AxisValue);
	UFUNCTION()
	void HorizontalMovement(float AxisValue);
	UFUNCTION()
	void DirectionalMovement();
	UFUNCTION()
	void YawRotation(float AxisValue);

	// These functions control the functionality of the states.
	UFUNCTION()
	void InventorySelected();
	UFUNCTION()
	void InventoryReleased();
	UFUNCTION()
	void InteractSelected();
	UFUNCTION()
	void InteractReleased();
	UFUNCTION()
	void MenuSelected();
	UFUNCTION()
	void MenuReleased();
	UFUNCTION()
	void SprintSelected();
	UFUNCTION()
	void SprintReleased();

	//TEMP > (this is to check if the question answer feature works)
	UFUNCTION()
	void Option1Selected();
	UFUNCTION()
	void Option1Released();
	UFUNCTION()
	void Option2Selected();
	UFUNCTION()
	void Option2Released();
	//TEMP  <

	UFUNCTION()
	void BeginConversationInteraction();
	UFUNCTION()
	void FinishConversationInteraction();

	UFUNCTION()
	void BeginInfoBoxInteraction();
	UFUNCTION()
	void FinishInfoBoxInteraction();

	// This expidites the process of the "AddOnScreenDebugMessage" function.
	UFUNCTION()
	void Message(FString Name);

	// The animations are based on an array index, this provides that index given a direction
	// and state.
	UFUNCTION()
	int32 FindArrayIndex(EMainSpriteDirection Dir, EMainSpriteState State);
};
