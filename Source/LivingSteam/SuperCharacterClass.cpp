// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperCharacterClass.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ASuperCharacterClass::ASuperCharacterClass()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//SpringArm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 700;
	SpringArmComp->SocketOffset = FVector(0,0,600);
	SpringArmComp->SetRelativeRotation(FRotator(0,45,0));

	//Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	CameraComp->SetupAttachment(SpringArmComp,USpringArmComponent::SocketName);
	CameraComp->SetRelativeRotation(FRotator(-30,0,0));
	//Mesh Component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("PlayerMesh");
	MeshComp->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ASuperCharacterClass::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
}

// Called every frame
void ASuperCharacterClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentStamina<MaxStamina && bIsRunning == false)
	{
		CurrentStamina+=DeltaTime*StaminaRegen;
	}
	if (bIsRunning==true && CurrentStamina>=0)
	{
		CurrentStamina-=DeltaTime;
	}

}

// Called to bind functionality to input
void ASuperCharacterClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Bind Axis
	PlayerInputComponent->BindAxis("MoveForward",this,&ASuperCharacterClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ASuperCharacterClass::MoveRight);

	//Bind Action
	PlayerInputComponent->BindAction("Run",IE_Pressed,this, &ASuperCharacterClass::Run);
	PlayerInputComponent->BindAction("Run",IE_Released,this,&ASuperCharacterClass::Run);
	PlayerInputComponent->BindAction("Attack",IE_Pressed,this,&ASuperCharacterClass::Attack);
}

void ASuperCharacterClass::MoveForward(const float Axis)
{
	AddMovementInput(FRotator(0,45,0).RotateVector(GetActorForwardVector()) * Axis);
}

void ASuperCharacterClass::MoveRight(const float Axis)
{
	AddMovementInput((FRotator(0,45,0).RotateVector(GetActorRightVector()) * Axis));
}

void ASuperCharacterClass::Run()
{

	CharacterMovement = GetCharacterMovement();
	if(CharacterMovement->MaxWalkSpeed == MaxMovementSpeed)
	{
		CharacterMovement->MaxWalkSpeed *= RunningSpeedMultiplyer;
		bIsRunning = true;
	}
	else
	{
		CharacterMovement->MaxWalkSpeed = MaxMovementSpeed;
		bIsRunning = false;
	}
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







