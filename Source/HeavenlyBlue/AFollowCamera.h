/*
**********************************************************************;
*	Project      : HeavenlyBlue
*
*	Program name : AFollowCamera
*
*	Author		 : ResponsibleFile (Dawson McThay)
*
*	Date created : 10/07/2019
*
*	Purpose		 : This class describes the functionality of the in-game camera 
*
*	Revisions	 : 10/18/2019
*
**********************************************************************;
*/

#pragma once

// Engine Includes
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"

// Components
#include "CineCameraComponent.h"

//Generated File (Must Be Last)
#include "AFollowCamera.generated.h"

UCLASS()
class HEAVENLYBLUE_API UAFollowCamera : public UCineCameraComponent
{
	GENERATED_BODY()

	UAFollowCamera();

public:
protected:
private:
};
