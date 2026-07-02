#include "WistoriaTitleGameMode.h"

#include "WistoriaTitlePlayerController.h"

AWistoriaTitleGameMode::AWistoriaTitleGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AWistoriaTitlePlayerController::StaticClass();
}
