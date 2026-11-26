// Copyright Lekaum

#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"


AAuraEnemy::AAuraEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetCustomDepthStencilValue(HIGHLIGHT_DEPTH_RED);
	Weapon->SetCustomDepthStencilValue(HIGHLIGHT_DEPTH_RED);

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	// Replication Mode set to Minimal because this is the enemy, AI-Controlled
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	// init Ability actor info for enemies
	check(AbilitySystemComponent);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		AuraASC->AbilityActorInfoSet();
	}
}


void AAuraEnemy::HighlightActor()
{
	// because of our custom post process volume material, we can highlight actor simply enable the renderCustomDepth and set depth stencil value to CUSTOM_DEPTH_RED (done in constructor)
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}
