#pragma once

UENUM(BlueprintType)
enum class ECharacterSkinType : uint8
{
	Red,
	Green,
	Blue,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	Helmet,
	Chest,
	Legs,
	Hands,
	Foot,

	Count UMETA(Hidden)
};