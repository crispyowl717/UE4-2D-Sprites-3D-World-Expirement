#include "AFollowCamera.h"

/*
 * Function:  UAFollowCamera
 * --------------------
 * This creates the base functionality of the UAFollowCamera class.
 *
 */
UAFollowCamera::UAFollowCamera()
{
	// Baseline Settings for the Camera
	FilmbackSettings.SensorWidth = 28.2f;
	FilmbackSettings.SensorHeight = 12.0f;
	SetLensPresetByName("30mm Prime f/1.4");
	CurrentFocalLength = 30.0f;
	CurrentAperture = 2.8f;
	PrimaryComponentTick.bCanEverTick = true;
}
