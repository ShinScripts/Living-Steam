// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperCharacterClass.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ASuperCharacterClass::ASuperCharacterClass()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SpringArm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 0;
	SpringArmComp->SocketOffset = FVector(0,0,0);
	SpringArmComp->SetRelativeRotation(FRotator(0,0,0));

	//Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	CameraComp->SetupAttachment(SpringArmComp,USpringArmComponent::SocketName);
	CameraComp->SetRelativeRotation(FRotator(0,0,0));
	CameraComp->SetProjectionMode(ECameraProjectionMode::Perspective);
	//Mesh Component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("PlayerMesh");
	MeshComp->SetupAttachment(RootComponent);
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
}

// Called when the game starts or when spawned
void ASuperCharacterClass::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;

	PC = Cast<APlayerController>(GetController());

	if(PC)
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if(SubSystem)
		{
			SubSystem->AddMappingContext(PlayerMapping,0);
		}
		PC->SetControlRotation(GetActorRotation());
	}
}

// Called every frame
void ASuperCharacterClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bRechargeStamina && CurrentStamina < MaxStamina)
	{
		CurrentStamina+=DeltaTime*StaminaRegen;
	}
	
	DashInterpolation(DeltaTime);
}

// Called to bind functionality to input
void ASuperCharacterClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//Bind Axis
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ASuperCharacterClass::Move);
		EnhancedInputComponent->BindAction(RunAction,ETriggerEvent::Triggered,this,&ASuperCharacterClass::Run);
		EnhancedInputComponent->BindAction(RunAction,ETriggerEvent::Completed,this,&ASuperCharacterClass::Run);
		EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&ASuperCharacterClass::Look);
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Triggered,this,&ACharacter::Jump);
		EnhancedInputComponent->BindAction(DashAction,ETriggerEvent::Triggered,this,&ASuperCharacterClass::Dash);
	}
}

void ASuperCharacterClass::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ASuperCharacterClass::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	DashDirection = Value.Get<FVector2D>();
	if(PC && bCanMove)
	{
		const FRotator Rotation = PC->GetControlRotation();
		const FRotator YawRotation(0,Rotation.Yaw,0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection,MovementVector.Y);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection,MovementVector.X);
	}
}

void ASuperCharacterClass::Run(const FInputActionValue& Value)
{
	bool Running = Value.Get<bool>();

	if(Running && CurrentStamina >= 0)
	{
		bRechargeStamina = false;
		CharacterMovement = GetCharacterMovement();
		CharacterMovement->MaxWalkSpeed = MaxMovementSpeed * RunningSpeedMultiplyer;
		CurrentStamina-=GetWorld()->GetDeltaSeconds();
		UE_LOG(LogTemp,Warning,TEXT("RUNNING"));
	}
	else
	{
		bRechargeStamina = true;
		UE_LOG(LogTemp,Warning,TEXT("NOT RUNNING"));
		CharacterMovement->MaxWalkSpeed = MaxMovementSpeed;
	}
}

void ASuperCharacterClass::Dash(const FInputActionValue& Value)
{
	bIsDashing = Value.Get<bool>();

	if(bIsDashing)
	{
		bCanMove = false;
		DashEndLocation = GetActorForwardVector() * DashDistance;
	}
}

void ASuperCharacterClass::DashInterpolation(float DeltaTime)
{
	if(bIsDashing)
	{
		bCanMove = false;
		const float DashSpeed = (GetActorLocation() - DashEndLocation).Length()/DashDuration;
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(),DashEndLocation + GetActorLocation(),GetWorld()->DeltaTimeSeconds,DashSpeed));
		DashDuration -= DeltaTime;
	}

	if(CurrentDashDuration < 0)
	{
		bIsDashing = false;
		bCanMove = true;
		CurrentDashDuration = DashDuration;
	}
}


void ASuperCharacterClass::EndDash()
{
}



float ASuperCharacterClass::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) 
{
	CurrentHealth-=DamageAmount;
	if(CurrentHealth<=0)
	{
	
		UGameplayStatics::GetGameMode(GetWorld());
	}

	return 0;
}

void ASuperCharacterClass::Attack()
{
	//Do nothing
}







