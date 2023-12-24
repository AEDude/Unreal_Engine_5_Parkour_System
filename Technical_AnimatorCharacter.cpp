// Copyright Epic Games, Inc. All Rights Reserved.

#include "Technical_AnimatorCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Custom_Movement_Component.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DebugHelper.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATechnical_AnimatorCharacter

ATechnical_AnimatorCharacter::ATechnical_AnimatorCharacter(const FObjectInitializer& ObjectInitializer)
	:	Super(ObjectInitializer.SetDefaultSubobjectClass<UCustom_Movement_Component>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	Custom_Movement_Component = Cast<UCustom_Movement_Component>(GetCharacterMovement());
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Motion_Warping_Component = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("C++ Motion Warping Component"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATechnical_AnimatorCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	Add_Input_Mapping_Context(DefaultMappingContext, 0);
	
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->On_Enter_Climb_State_Delegate.BindUObject(this, &ThisClass::On_Player_Enter_Climb_State);
		Custom_Movement_Component->On_Exit_Climb_State_Delegate.BindUObject(this, &ThisClass::On_Player_Exit_Climb_State);
		
		Custom_Movement_Component->On_Enter_Take_Cover_State_Delegate.BindUObject(this, &ThisClass::On_Player_Enter_Take_Cover_State);
		Custom_Movement_Component->On_Exit_Take_Cover_State_Delegate.BindUObject(this, &ThisClass::On_Player_Exit_Take_Cover_State);
	}
	
}

void ATechnical_AnimatorCharacter::Add_Input_Mapping_Context(UInputMappingContext* Context_To_Add, int32 In_Priority)
{
	if(!Context_To_Add) return;

	//Add Input Mapping Context
	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(Context_To_Add, In_Priority);
		}
}

void ATechnical_AnimatorCharacter::Remove_Input_Mapping_Context(UInputMappingContext* Context_To_Remove)
{
	if(!Context_To_Remove) return;

	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(Context_To_Remove);
		}
}


//////////////////////////////////////////////////////////////////////////
// Input

void ATechnical_AnimatorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
		{
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATechnical_AnimatorCharacter::Handle_Ground_Movement_Input);
		EnhancedInputComponent->BindAction(Climbing_Move_Action, ETriggerEvent::Triggered, this, &ATechnical_AnimatorCharacter::Handle_Climb_Movement_Input);
		EnhancedInputComponent->BindAction(Take_Cover_Move_Action, ETriggerEvent::Triggered, this, &ATechnical_AnimatorCharacter::Handle_Take_Cover_Movement_Input);


		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATechnical_AnimatorCharacter::Look);

		EnhancedInputComponent->BindAction(Parkour_Action, ETriggerEvent::Started, this, &ATechnical_AnimatorCharacter::On_Parkour_Started);

		EnhancedInputComponent->BindAction(Exit_Parkour_Action, ETriggerEvent::Started, this, &ATechnical_AnimatorCharacter::On_Parkour_Ended);

		EnhancedInputComponent->BindAction(Climb_Action, ETriggerEvent::Started, this, &ATechnical_AnimatorCharacter::On_Climb_Action_Started);

		EnhancedInputComponent->BindAction(Climb_Hop_Action, ETriggerEvent::Started, this, &ATechnical_AnimatorCharacter::On_Climb_Hop_Action_Started);

		EnhancedInputComponent->BindAction(Take_Cover_Action, ETriggerEvent::Started, this, &ATechnical_AnimatorCharacter::On_Take_Cover_Action_Started);
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
		}
}

void ATechnical_AnimatorCharacter::Handle_Ground_Movement_Input(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATechnical_AnimatorCharacter::Handle_Climb_Movement_Input(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FVector ForwardDirection = FVector::CrossProduct(
			-Custom_Movement_Component->Get_Climbable_Surface_Normal(),
			GetActorRightVector()
		);

		const FVector RightDirection = FVector::CrossProduct(
			-Custom_Movement_Component->Get_Climbable_Surface_Normal(),
			-GetActorUpVector()
		);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATechnical_AnimatorCharacter::Handle_Take_Cover_Movement_Input(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FVector ForwardDirection = FVector::CrossProduct(
			-Custom_Movement_Component->Get_Climbable_Surface_Normal(),
			GetActorRightVector()
		);

		const FVector RightDirection = FVector::CrossProduct(
			-Custom_Movement_Component->Get_Take_Cover_Surface_Normal(),
			-GetActorUpVector()
		);

		// add movement 
		AddMovementInput(RightDirection, MovementVector.X); 
	}
}

void ATechnical_AnimatorCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATechnical_AnimatorCharacter::On_Parkour_Started(const FInputActionValue& Value)
{
	Debug::Print(TEXT("Parkour Is Working"));
}

void ATechnical_AnimatorCharacter::On_Parkour_Ended(const FInputActionValue& Value)
{
	Debug::Print(TEXT("Exited Parkour"));
}

void ATechnical_AnimatorCharacter::On_Climb_Action_Started(const FInputActionValue& Value)
{
	if(!Custom_Movement_Component) return;

	if(!Custom_Movement_Component->Is_Climbing())
	{
		Custom_Movement_Component->Toggle_Climbing(true);
	}
	else
	{
		Custom_Movement_Component->Toggle_Climbing(false);
	}
}

void ATechnical_AnimatorCharacter::On_Player_Enter_Climb_State()
{
	Add_Input_Mapping_Context(Climbing_Mapping_Context, 1);
	Debug::Print(TEXT("Entered Climb State."));
}

void ATechnical_AnimatorCharacter::On_Player_Exit_Climb_State()
{
	Remove_Input_Mapping_Context(Climbing_Mapping_Context);
	Debug::Print(TEXT("Exited Climb State."));
}

void ATechnical_AnimatorCharacter::On_Player_Enter_Take_Cover_State()
{
	Add_Input_Mapping_Context(Take_Cover_Mapping_Context, 1);
	Debug::Print(TEXT("Entered Take Cover State."));
}

void ATechnical_AnimatorCharacter::On_Player_Exit_Take_Cover_State()
{
	Remove_Input_Mapping_Context(Take_Cover_Mapping_Context);
	Debug::Print(TEXT("Exited Take Cover State."));
}

void ATechnical_AnimatorCharacter::On_Climb_Hop_Action_Started(const FInputActionValue &Value)
{
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Request_Hopping();
	}
	Debug::Print(TEXT("Hopping Started"));
}

void ATechnical_AnimatorCharacter::On_Take_Cover_Action_Started(const FInputActionValue &Value)
{
	if(!Custom_Movement_Component) return;

	if(!Custom_Movement_Component->Is_Taking_Cover())
	{
		Custom_Movement_Component->Toggle_Take_Cover(true);
	}
	else
	{
		Custom_Movement_Component->Toggle_Take_Cover(false);
	}
}
