// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Technical_Animator_Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Custom_Movement_Component.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Debug/DebugHelper.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character_Direction/Character_Direction_Arrow.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation_Instance/Character_Animation_Instance.h"
#include "Net/UnrealNetwork.h"
#include "World_Actors/Wall_Pipe_Actor.h"
#include "World_Actors/Balance_Traversal_Actor.h"
#include "World_Actors/Wall_Vault_Actor.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATechnical_Animator_Character

ATechnical_Animator_Character::ATechnical_Animator_Character(const FObjectInitializer& ObjectInitializer)
	:	Super(ObjectInitializer.SetDefaultSubobjectClass<UCustom_Movement_Component>(ACharacter::CharacterMovementComponentName))
{
	//Set this actor to replicate for network compatibility.
	bReplicates = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 98.0f);
	GetCapsuleComponent()->SetVisibility(true);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	Custom_Movement_Component = Cast<UCustom_Movement_Component>(GetCharacterMovement());
	//Set this component to replicate for network compatibility.
	Custom_Movement_Component->SetIsReplicated(true);

	// Configure character movement
	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 270.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint.
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 240.f;
	GetCharacterMovement()->MaxAcceleration = 400.f;
	GetCharacterMovement()->BrakingFrictionFactor = .5f;
	GetCharacterMovement()->GroundFriction = 5.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	Camera_Boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera_Boom"));
	Camera_Boom->SetupAttachment(RootComponent);
	Camera_Boom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	.
	Camera_Boom->bUsePawnControlRotation = true; // Rotate the arm based on the controller.

	// Create a follow camera
	Follow_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow_Camera"));
	Follow_Camera->SetupAttachment(Camera_Boom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation.
	Follow_Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm.

	Motion_Warping_Component = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("C++ Motion Warping Component"));
	Motion_Warping_Component->SetIsReplicated(true);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATechnical_Animator_Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	Add_Input_Mapping_Context(DefaultMappingContext, 0);

	//Get a reference to self to pass onto "&UCustom_Movement_Component::Initialize_References()".
	Character_Reference = this;
	
	if(Custom_Movement_Component && Character_Reference && Motion_Warping_Component && Follow_Camera)
	{
		Custom_Movement_Component->Initialize_Parkour(Character_Reference, Motion_Warping_Component, Follow_Camera);

		Custom_Movement_Component->On_Enter_Climb_State_Delegate.BindUObject(this, &ThisClass::On_Player_Enter_Climb_State);
		Custom_Movement_Component->On_Exit_Climb_State_Delegate.BindUObject(this, &ThisClass::On_Player_Exit_Climb_State);
		
		Custom_Movement_Component->On_Enter_Take_Cover_State_Delegate.BindUObject(this, &ThisClass::On_Player_Enter_Take_Cover_State);
		Custom_Movement_Component->On_Exit_Take_Cover_State_Delegate.BindUObject(this, &ThisClass::On_Player_Exit_Take_Cover_State);

		UE_LOG(LogTemp, Warning, TEXT("Custom_Movement_Component && Character_Reference && Motion_Warping_Component && Follow_Camera INITIALIZATION SUCCEEDED"));
	}

	else
	UE_LOG(LogTemp, Warning, TEXT("Custom_Movement_Component && Character_Reference && Motion_Warping_Component && Follow_Camera INITIALIZATION FAILED"));

	Character_Animation_Instance = Cast<UCharacter_Animation_Instance>(GetMesh()->GetAnimInstance());

	if(Character_Animation_Instance)
	{
		Debug::Print("Character_Animation_Instance_Reference_Obtained_In_Character_Class");
	}
	
	else
	{
		Debug::Print("Character_Animation_Instance_Reference_Not_Obtained_In_Character_Class");
	}

	TArray<AActor*> Actors{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Direction_Arrow::StaticClass(), Actors);

	if(Actors.IsValidIndex(0))
	{
		Character_Direction_Arrow = Cast<ACharacter_Direction_Arrow>(Actors[0]);
		Debug::Print("Character_Direction_Arrow_Reference_Obtained_In_Character_Class");
	}

	else
	{
		Debug::Print("Character_Direction_Arrow_Reference_Not_Obtained_In_Character_Class");
	}
	
}

void ATechnical_Animator_Character::Tick(float Deltatime)
{
	// Call the base class  
	Super::Tick(Deltatime);

}

void ATechnical_Animator_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATechnical_Animator_Character, bIs_Jogging, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ATechnical_Animator_Character, bDrop_To_Shimmy, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ATechnical_Animator_Character, Wall_Pipe_Actor, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ATechnical_Animator_Character, Balance_Traversal_Actor, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ATechnical_Animator_Character, Wall_Vault_Actor, COND_OwnerOnly);
}

void ATechnical_Animator_Character::Add_Input_Mapping_Context(UInputMappingContext* Context_To_Add, int32 In_Priority)
{
	if(!Context_To_Add) return;

	//Add Input Mapping Context
	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(Context_To_Add, In_Priority);
		}
}

void ATechnical_Animator_Character::Remove_Input_Mapping_Context(UInputMappingContext* Context_To_Remove)
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

void ATechnical_Animator_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		
	// Jumping
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATechnical_Animator_Character::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	// Moving
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::Handle_Ground_Movement_Input_Triggered);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATechnical_Animator_Character::Handle_Ground_Movement_Input_Completed);
	EnhancedInputComponent->BindAction(Climbing_Move_Action, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::Handle_Climb_Movement_Input);
	EnhancedInputComponent->BindAction(Take_Cover_Move_Action, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::Handle_Take_Cover_Movement_Input);

	// Looking
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::Look);

	//Custom
	EnhancedInputComponent->BindAction(Jog_Action, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::On_Jogging_Started);

	EnhancedInputComponent->BindAction(Jog_Action, ETriggerEvent::Completed, this, &ATechnical_Animator_Character::On_Jogging_Ended);

	EnhancedInputComponent->BindAction(Parkour_Action, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::On_Parkour_Started);

	EnhancedInputComponent->BindAction(Parkour_Action_Double_Tap, ETriggerEvent::Completed, this, &ATechnical_Animator_Character::On_Parkour_Started_Double_Tap);

	EnhancedInputComponent->BindAction(Exit_Parkour_Action, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::On_Parkour_Ended);

	EnhancedInputComponent->BindAction(Exit_Parkour_Action, ETriggerEvent::None, this, &ATechnical_Animator_Character::On_Parkour_Ended_Completed);

	EnhancedInputComponent->BindAction(Wall_Run_Action, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::On_Wall_Run_Started);

	EnhancedInputComponent->BindAction(Wall_Pipe_Action, ETriggerEvent::Triggered, this, &ATechnical_Animator_Character::On_Wall_Pipe_Climb_Started);

	EnhancedInputComponent->BindAction(Debug_Action, ETriggerEvent::Started, this, &ATechnical_Animator_Character::On_Debug_Action);

	EnhancedInputComponent->BindAction(Climb_Action, ETriggerEvent::Started, this, &ATechnical_Animator_Character::On_Climb_Action_Started);

	EnhancedInputComponent->BindAction(Climb_Hop_Action, ETriggerEvent::Started, this, &ATechnical_Animator_Character::On_Climb_Hop_Action_Started);

	EnhancedInputComponent->BindAction(Take_Cover_Action, ETriggerEvent::Started, this, &ATechnical_Animator_Character::On_Take_Cover_Action_Started);

	}

	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATechnical_Animator_Character::Jump()
{
	Super::Jump();

	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Execute_Jump_Out_Of_Shimmy();
		Custom_Movement_Component->Execute_Exit_Wall_Run_With_Jump_Forward();
		Custom_Movement_Component->Execute_Parkour_Jump();
	}
}

void ATechnical_Animator_Character::Handle_Ground_Movement_Input_Triggered(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D Movement_Vector{Value.Get<FVector2D>()};

	if(Controller)
	{
		if(Custom_Movement_Component)
		{	//Ground movement is handled in the Custom_Movement Component, alongside the Parkour Locomotion. Within this function
		// 
			Custom_Movement_Component->Add_Movement_Input(Movement_Vector, true);
			Custom_Movement_Component->Add_Movement_Input(Movement_Vector, false);
		}
	}

	/*if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator Yaw_Rotation{(0, Rotation.Yaw, 0)};

		// get forward vector
		const FVector Forward_Direction{FRotationMatrix(Yaw_Rotation).GetUnitAxis(EAxis::X)};
	
		// get right vector 
		const FVector Right_Direction{FRotationMatrix(Yaw_Rotation).GetUnitAxis(EAxis::Y)};

		// add movement 
		AddMovementInput(Forward_Direction, Movement_Vector.Y);
		AddMovementInput(Right_Direction, Movement_Vector.X);
	}*/
}

void ATechnical_Animator_Character::Handle_Ground_Movement_Input_Completed(const FInputActionValue& Value)
{
	if(Controller)
	{
		if(Custom_Movement_Component)
		{	
			//When the call to "Handle_Ground_Movement_Input_Started" is completed this function will be called. It resets the values
			//of the "Forward_Backward_Movement_Value" and the "Right_Left_Movement_Value" which are set within 
			//"&Ucustom_Movement_Component::Add_Movement_Input". It also sets the FGameplaytag "Parkour_Direction" to
			//"Parkour.Direction.None". The reason for the if check is because the custom ground locomotion within the Animation instance needs
			//braking distance in order for some of the animations to work as intended. Therefore These variables will only be reset if the character is 
			//not in Parkour_State "Parkour.State.Free.Roam".
			if(Custom_Movement_Component->Get_Parkour_State() != FGameplayTag::RequestGameplayTag(FName(TEXT("Parkour.State.Free.Roam"))))
			{
				Custom_Movement_Component->Stop_Parkour_Movement_Immediately_And_Reset_Movement_Input_Variables();
				Custom_Movement_Component->Forward_Backward_Movement_Value = 0.f;
				Custom_Movement_Component->Right_Left_Movement_Value = 0.f;
				Custom_Movement_Component->Server_Set_Add_Movement_Input_Variables(0.f, 0.f);
			}

			else
			{
				Custom_Movement_Component->Forward_Backward_Movement_Value = 0.f;
				Custom_Movement_Component->Right_Left_Movement_Value = 0.f;
				Custom_Movement_Component->Server_Set_Add_Movement_Input_Variables(0.f, 0.f);
			}
		}
	}
}

void ATechnical_Animator_Character::Handle_Climb_Movement_Input(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D Movement_Vector{Value.Get<FVector2D>()};

	if (Controller != nullptr)
	{
		const FVector Forward_Direction{FVector::CrossProduct(
			-Custom_Movement_Component->Get_Climbable_Surface_Normal(),
			GetActorRightVector()
		)};

		const FVector Right_Direction{FVector::CrossProduct(
			-Custom_Movement_Component->Get_Climbable_Surface_Normal(),
			-GetActorUpVector()
		)};

		// add movement 
		AddMovementInput(Forward_Direction, Movement_Vector.Y);
		AddMovementInput(Right_Direction, Movement_Vector.X);
	}
}

void ATechnical_Animator_Character::Handle_Take_Cover_Movement_Input(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D Movement_Vector{Value.Get<FVector2D>()};

	if (Controller != nullptr)
	{
		if(Custom_Movement_Component)
		{
			const FVector Forward_Direction{FVector::CrossProduct(
				-Custom_Movement_Component->Get_Take_Cover_Surface_Normal(),
				GetActorRightVector()
			)};

			const FVector Right_Direction{FVector::CrossProduct(
				-Custom_Movement_Component->Get_Take_Cover_Surface_Normal(),
				-GetActorUpVector()
			)};

			// add movement 
			AddMovementInput(Right_Direction, Movement_Vector.X);
		} 
	}

}

void ATechnical_Animator_Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	//FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	Up_Down_Look_Value =  Value.Get<FVector2D>().Y;
	Left_Right_Look_Value = Value.Get<FVector2D>().X;

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Left_Right_Look_Value);
		AddControllerPitchInput(Up_Down_Look_Value);
	}
}

void ATechnical_Animator_Character::On_Jogging_Started(const FInputActionValue& Value)
{
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Execute_Start_Running();
	}
}

void ATechnical_Animator_Character::On_Jogging_Ended(const FInputActionValue& Value)
{
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Execute_Stop_Running();
	}
}

void ATechnical_Animator_Character::On_Parkour_Started(const FInputActionValue& Value)
{
	if(Custom_Movement_Component)
	{
		if(Custom_Movement_Component->Get_bIs_Falling())
		{
			Debug::Print("Parkour_Cool_Down_Activated", FColor::MakeRandomColor(), 11);
			return;
		}

		else
		{
			Debug::Print(TEXT("Parkour_Is_Working"), FColor::MakeRandomColor(), 8);
			Custom_Movement_Component->Execute_Parkour_Action();
			Custom_Movement_Component->Execute_Parkour_Wall_Pipe_Climb_Action();
			Custom_Movement_Component->Execute_Balance_Walk_Hop();
		}
	}
}

void ATechnical_Animator_Character::On_Parkour_Started_Double_Tap(const FInputActionValue& Value)
{
	if(Custom_Movement_Component)
	{
		Debug::Print(TEXT("Parkour_Double_Tap_Is_Working"), FColor::MakeRandomColor(), 8);
		Custom_Movement_Component->Execute_Free_Hang_To_Balanced_Walk();
	}
}

void ATechnical_Animator_Character::On_Parkour_Ended(const FInputActionValue& Value)
{
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Release_From_Shimmying();
		Custom_Movement_Component->Execute_Drop_Into_Shimmy();
		bDrop_To_Shimmy = true;
		Custom_Movement_Component->Release_From_Parkour_Wall_Pipe_Climb();
		Custom_Movement_Component->Execute_Balance_Drop_Hanging();
	}
}

void ATechnical_Animator_Character::On_Parkour_Ended_Completed(const FInputActionValue& Value)
{
	bDrop_To_Shimmy = false;
	Server_On_Parkour_Ended_Completed(Value);
}

void ATechnical_Animator_Character::Server_On_Parkour_Ended_Completed_Implementation(const FInputActionValue& Value)
{
	bDrop_To_Shimmy = false;
}

void ATechnical_Animator_Character::On_Wall_Run_Started(const FInputActionValue& Value)
{
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Execute_Wall_Run();
	}
}

void ATechnical_Animator_Character::On_Wall_Pipe_Climb_Started(const FInputActionValue& Value)
{
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Execute_Parkour_Wall_Pipe_Climb();
	}
}

void ATechnical_Animator_Character::On_Debug_Action(const FInputActionValue& Value)
{
	if(Custom_Movement_Component && Character_Direction_Arrow && Character_Animation_Instance)
	{
		if(Debug_Selector == 2)
		{
			Debug_Selector = -1;
		}
		
		Debug_Selector++;

		switch(Debug_Selector)
		{
			case 0:
			Custom_Movement_Component->Debug_Action = EDrawDebugTrace::None;
			GetCapsuleComponent()->SetVisibility(false);
			Character_Direction_Arrow->SetHidden(true);
			Character_Animation_Instance->Set_bLook_Left_Right_Debug_Visibility(false);
			Debug::Print("Debug_Mode_Changed: None", FColor::Emerald, 22);
			break;
			
			case 1:
			Custom_Movement_Component->Debug_Action = EDrawDebugTrace::ForDuration;
			GetCapsuleComponent()->SetVisibility(true);
			Character_Direction_Arrow->SetHidden(false);
			Character_Animation_Instance->Set_bLook_Left_Right_Debug_Visibility(true);
			Debug::Print("Debug_Mode_Changed: For_Duration", FColor::Emerald, 22);
			break;

			case 2:
			Custom_Movement_Component->Debug_Action = EDrawDebugTrace::ForOneFrame;
			GetCapsuleComponent()->SetVisibility(true);
			Character_Direction_Arrow->SetHidden(false);
			Character_Animation_Instance->Set_bLook_Left_Right_Debug_Visibility(true);
			Debug::Print("Debug_Mode_Changed: For_One_Frame", FColor::Emerald, 22);
			break;
		}
	}
}

void ATechnical_Animator_Character::On_Climb_Action_Started(const FInputActionValue& Value)
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

void ATechnical_Animator_Character::On_Player_Enter_Climb_State()
{
	Add_Input_Mapping_Context(Climbing_Mapping_Context, 1);
	Debug::Print(TEXT("Entered Climb State."));
}

void ATechnical_Animator_Character::On_Player_Exit_Climb_State()
{
	Remove_Input_Mapping_Context(Climbing_Mapping_Context);
	Debug::Print(TEXT("Exited Climb State."));
}

void ATechnical_Animator_Character::On_Player_Enter_Take_Cover_State()
{
	Add_Input_Mapping_Context(Take_Cover_Mapping_Context, 2);
	Debug::Print(TEXT("Entered Take Cover State."));
}

void ATechnical_Animator_Character::On_Player_Exit_Take_Cover_State()
{
	Remove_Input_Mapping_Context(Take_Cover_Mapping_Context);
	Debug::Print(TEXT("Exited Take Cover State."));
}

void ATechnical_Animator_Character::On_Climb_Hop_Action_Started(const FInputActionValue &Value)
{
	if(Custom_Movement_Component)
	{
		Custom_Movement_Component->Request_Hopping();
	}
	Debug::Print(TEXT("Hopping Started"));
}

void ATechnical_Animator_Character::On_Take_Cover_Action_Started(const FInputActionValue &Value)
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

#pragma region Network

void ATechnical_Animator_Character::Set_Overlapping_Wall_Pipe_Actor(AWall_Pipe_Actor* Overlapping_Wall_Pipe_Actor)
{
	/*This function is only called from the server. Therefore when the first "if" check successfully passes this means the Server is calling this function locally 
	(a client isn't calling this funtion from from the server).*/
	
	if(Wall_Pipe_Actor)
	Wall_Pipe_Actor->Set_Wall_Pipe_Actor_Widget_Visibility(false);
	
	Wall_Pipe_Actor = Overlapping_Wall_Pipe_Actor;

	if(IsLocallyControlled())
	{
		if(Wall_Pipe_Actor)
		Wall_Pipe_Actor->Set_Wall_Pipe_Actor_Widget_Visibility(true);
	}
}

void ATechnical_Animator_Character::On_Replication_Wall_Pipe_Actor(AWall_Pipe_Actor* Previous_Wall_Pipe_Actor)
{
	/* This fucntion (a rep notify) will only be called on the owning player of the variable which is being replicated "Wall_Pipe_Actor" -  
	See &ATechnical_Animator_Character::GetLifetimeReplicatedProps. The input parameter holds the previous value of the variable which is being replicated. Therefore, when 
	"Wall_Pipe_Actor" initially replicates to a valid pointer from being null the input parameter "Previous_Wall_Pipe_Actor" will be a nullptr and when "Wall_Pipe_Actor" replicates to a null pointer 
	from being vaild then the input argument will be hold the valid data that "Wall_Pipe_Actor" was storing before it became a nullptr.*/
	
	if(Wall_Pipe_Actor)
	Wall_Pipe_Actor->Set_Wall_Pipe_Actor_Widget_Visibility(true);

	else if(Previous_Wall_Pipe_Actor)
	Previous_Wall_Pipe_Actor->Set_Wall_Pipe_Actor_Widget_Visibility(false);
}

void ATechnical_Animator_Character::Set_Overlapping_Balance_Traversal_Actor(ABalance_Traversal_Actor* Overlapping_Balance_Traversal_Actor)
{
	/*This function is only called from the server. Therefore when the first "if" check successfully passes this means the Server is calling this function locally 
	(a client isn't calling this funtion from from the server).*/
	
	if(Balance_Traversal_Actor && Custom_Movement_Component)
	{
		Balance_Traversal_Actor->Show_Display_Widget(false);
		Custom_Movement_Component->Execute_Balance_Traversal(nullptr);
	}
	
	Balance_Traversal_Actor = Overlapping_Balance_Traversal_Actor;

	if(IsLocallyControlled())
	{
		if(Balance_Traversal_Actor && Custom_Movement_Component)
		{
			Balance_Traversal_Actor->Show_Display_Widget(true);
			Custom_Movement_Component->Execute_Balance_Traversal(Balance_Traversal_Actor);
			Custom_Movement_Component->Execute_Exit_Balance_Traversal();
		}
	}
}

void ATechnical_Animator_Character::On_Replication_Balance_Traversal_Actor(ABalance_Traversal_Actor* Previous_Balance_Traversal_Actor)
{
	/* This fucntion (a rep notify) will only be called on the owning player of the variable which is being replicated "Balance_Traversal_Actor" -  
	See &ATechnical_Animator_Character::GetLifetimeReplicatedProps. The input parameter holds the previous value of the variable which is being replicated. Therefore, when 
	"Balance_Traversal_Actor" initially replicates to a valid pointer from being null the input parameter "Previous_Balance_Traversal_Actor" will be a nullptr and when "Balance_Traversal_Actor" 
	replicates to a null pointer from being vaild then the input argument will be hold the valid data that "Balance_Traversal_Actor" was storing before it became a nullptr.*/
	
	if(Balance_Traversal_Actor && Custom_Movement_Component)
	{
		Balance_Traversal_Actor->Show_Display_Widget(true);
		Custom_Movement_Component->Execute_Balance_Traversal(Balance_Traversal_Actor);
		Custom_Movement_Component->Execute_Exit_Balance_Traversal();
	}
	
	else if(Previous_Balance_Traversal_Actor && Custom_Movement_Component)
	{
		Previous_Balance_Traversal_Actor->Show_Display_Widget(false);
		Custom_Movement_Component->Execute_Balance_Traversal(nullptr);
	}
}

void ATechnical_Animator_Character::Set_Overlapping_Wall_Vault_Actor(AWall_Vault_Actor* Overlapping_Wall_Vault_Actor)
{
	/*This function is only called from the server. Therefore when the first "if" check successfully passes this means the Server is calling this function locally 
	(a client isn't calling this funtion from from the server).*/
	
	Wall_Vault_Actor = Overlapping_Wall_Vault_Actor;

	if(IsLocallyControlled())
	{
		if(Wall_Vault_Actor)
		{
			if(Custom_Movement_Component)
			{
				Custom_Movement_Component->Execute_Wall_Vault(Wall_Vault_Actor);
			}
		}

		else if(!Wall_Vault_Actor)
		{
			if(Custom_Movement_Component)
			{
				Custom_Movement_Component->Execute_Wall_Vault(nullptr);
			}
		}
	}

}

void ATechnical_Animator_Character::On_Replication_Wall_Vault_Actor(AWall_Vault_Actor* Previous_Wall_Vault_Actor)
{
	/* This fucntion (a rep notify) will only be called on the owning player of the variable which is being replicated "Wall_Vault_Actor" -  
	See &ATechnical_Animator_Character::GetLifetimeReplicatedProps. The input parameter holds the previous value of the variable which is being replicated. Therefore, when 
	"Wall_Vault_Actor" initially replicates to a valid pointer from being null the input parameter "Previous_Wall_Vault_Actor" will be a nullptr and when "Wall_Vault_Actor" 
	replicates to a null pointer from being vaild then the input argument will be hold the valid data that "Wall_Vault_Actor" was storing before it became a nullptr.*/
	
	if(Wall_Vault_Actor)
	{
		if(Custom_Movement_Component)
		{
		 	Custom_Movement_Component->Execute_Wall_Vault(Wall_Vault_Actor);
		}
	}

	else if(!Wall_Vault_Actor)
	{
		if(Custom_Movement_Component)
		{
		 	Custom_Movement_Component->Execute_Wall_Vault(nullptr);
		}
	}
	
}

#pragma endregion


