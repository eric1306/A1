#pragma once

UENUM(BlueprintType)
enum class ESlotState : uint8
{
	Default,
	Invalid,
	Valid
};

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Unarmed_LeftHand,
	Unarmed_RightHand,

	LeftHand,
	RightHand,
	TwoHand,

	//Utility_Primary,
	//Utility_Secondary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	Unarmed,

	Weapon_Primary,
	Weapon_Secondary,

	Utility_Primary,
	Utility_Secondary,

//	Primary,
//	Secondary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Armor,
	Weapon,
	Utility,

	Count	UMETA(Hidden)
};

//UENUM(BlueprintType)
//enum class EArmorType : uint8
//{
//	Helmet,
//	Chest,
//	Legs,
//	Hands,
//	Foot,
//
//	Count UMETA(Hidden)
//};

UENUM(BlueprintType)
enum class EBodyType : uint8
{
	Head		UMETA(DisplayName = "Head"),
	Torso		UMETA(DisplayName = "Torso"),
	Limbs		UMETA(DisplayName = "Limbs"),

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed,
	OneHandSword,
	TwoHandSword,
	Gun,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponSlotType : uint8
{
	Primary,
	Secondary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponHandType : uint8
{
	LeftHand,
	RightHand,
	TwoHand,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EUtilityType : uint8
{
	Fuel,
	Oxygen,
	Drink,
	LightSource,

	Count	UMETA(Hidden)
};

//UENUM(BlueprintType)
//enum class EUtilitySlotType : uint8
//{
//	Primary,
//	Secondary,
//	Tertiary,
//	Quaternary,
//
//	Count	UMETA(Hidden)
//};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Poor,
	Common,
	Uncommon,
	Rare,
	Legendary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	None,
	Projectile,
	AOE,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EOverlayTargetType : uint8
{
	None,
	Weapon,
	Character,
	All,
};

UENUM(BlueprintType)
enum class ECharacterSkinType : uint8
{
	Black,
	Grey,
	White,
	Red,
	Yellow,
	Blue,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ED1TeamID : uint8
{
	NoTeam,
	Monster = 1,
};