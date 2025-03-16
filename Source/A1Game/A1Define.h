#pragma once

UENUM(BlueprintType)
enum class ESlotState : uint8
{
	Default,
	Invalid,
	Valid
};

/*---------------------------------------
*   Character가 가지고 있어야 할 정보
---------------------------------------*/

// Character의 skin 색상
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

// Character Mesh Material type
UENUM(BlueprintType)
enum class EBodyType : uint8
{
	Head		UMETA(DisplayName = "Head"),
	Torso		UMETA(DisplayName = "Torso"),
	Limbs		UMETA(DisplayName = "Limbs"),

	Count UMETA(Hidden)
};

// 장비 장착 slot 타입 
UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Unarmed_LeftHand,
	Unarmed_RightHand,

	LeftHand,
	RightHand,
	TwoHand,

	Count	UMETA(Hidden)
};

// 메인으로 사용하는 손이 무엇인지
UENUM(BlueprintType)
enum class EEquipState : uint8
{
	Unarmed,

	Left,
	Right,
	Both,

	Count	UMETA(Hidden)
};


/*---------------------------------------
*    Item이 가지고 있어야 할 정보
---------------------------------------*/
UENUM(BlueprintType)
enum class EItemSlotType : uint8
{
	Left,
	Right,

	Count	UMETA(Hidden)
};

// 어디에 장착 가능한 건지
UENUM(BlueprintType)
enum class EItemHandType : uint8
{
	LeftHand,
	RightHand,
	TwoHand,

	Count	UMETA(Hidden)
};


// Item 분류
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Armor,
	Weapon,
	Utility,

	Count	UMETA(Hidden)
};

// 무기 종류
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed,
	OneHandSword,
	TwoHandSword,
	Gun,

	Count	UMETA(Hidden)
};

// Utillity 종류
UENUM(BlueprintType)
enum class EUtilityType : uint8
{
	Fuel,
	Oxygen,
	Drink,
	LightSource,

	Count	UMETA(Hidden)
};

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
enum class ED1TeamID : uint8
{
	NoTeam,
	Monster = 1,
};