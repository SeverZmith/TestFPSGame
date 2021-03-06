// Fill out your copyright notice in the Description page of Project Settings.

#include "Mannequin.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/InputSettings.h"
#include "../Weapons/Gun.h"
#include "DrawDebugHelpers.h"


AMannequin::AMannequin()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

}

void AMannequin::BeginPlay()
{
	Super::BeginPlay();

	if (GunBlueprint == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gun blueprint missing."));
		return;

	}
	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);

	//Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
	if (IsPlayerControlled())
	{
		Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	}
	else
	{
		Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint_0"));

	}

	// Set anim instances for first person and third person.
	Gun->AnimInstance1P = Mesh1P->GetAnimInstance();
	Gun->AnimInstance3P = GetMesh()->GetAnimInstance();

}

void AMannequin::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Gun->Destroy();

}

void AMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent != nullptr)
	{
		// Bind fire event
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMannequin::PullTrigger);

		// Bind jump events
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

		// Enable touchscreen input
		EnableTouchscreenMovement(PlayerInputComponent);

		PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMannequin::OnResetVR);

		// Bind movement events
		PlayerInputComponent->BindAxis("MoveForward", this, &AMannequin::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AMannequin::MoveRight);

		// We have 2 versions of the rotation bindings to handle different kinds of devices differently
		// "turn" handles devices that provide an absolute delta, such as a mouse.
		// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
		PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
		PlayerInputComponent->BindAxis("TurnRate", this, &AMannequin::TurnAtRate);
		PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("LookUpRate", this, &AMannequin::LookUpAtRate);

	}

}

void AMannequin::UnPossessed()
{
	Super::UnPossessed();

	if (Gun == nullptr)
	{
		return;

	}
	// This prevents the Gun mesh from disappearing when player dies.
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint_0"));

}

void AMannequin::PullTrigger()
{
	if (Gun == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Gun Equipped! -- AMannequin::Fire()"));
		return;

	}

	Gun->OnFire();

}

void AMannequin::DrawLineTraceEx()
{
	// Future implementations of shooting will use line traces to improve projectile flight logic.
	FHitResult OutHit;
	FVector Start = Gun->GetFPMuzzleLocation()->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = (Start + (ForwardVector * 1000.f));

	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	bool IsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	if (IsHit)
	{
		if (OutHit.bBlockingHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("You are hitting: %s: "), *OutHit.GetActor()->GetName());

			UE_LOG(LogTemp, Warning, TEXT("Impact Point: %s: "), *OutHit.ImpactPoint.ToString());

			UE_LOG(LogTemp, Warning, TEXT("Normal Point: %s: "), *OutHit.ImpactNormal.ToString());

		}

	}

}

/////////////////////////////////////////////////////////////
// CODE BELOW IS DIRECTLY FROM FirstPersonCharacter class. //
/////////////////////////////////////////////////////////////

void AMannequin::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();

}

void AMannequin::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;

	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		// OnFire();

	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;

}

void AMannequin::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;

	}
	TouchItem.bIsPressed = false;

}

void AMannequin::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);

	}

}

void AMannequin::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);

	}

}

void AMannequin::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void AMannequin::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

}

bool AMannequin::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMannequin::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMannequin::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFirstPersonCharacter::TouchUpdate);
		return true;

	}

	return false;

}
