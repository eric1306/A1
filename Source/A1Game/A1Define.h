#pragma once

UENUM(BlueprintType)
enum class ESlotState : uint8
{
	Default,
	Invalid,
	Valid
};

/*---------------------------------------
*   Character�� ������ �־�� �� ����
---------------------------------------*/

// Character�� skin ����
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

// ��� ���� slot Ÿ�� 
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

// �������� ����ϴ� ���� ��������
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
*    Item�� ������ �־�� �� ����
---------------------------------------*/
UENUM(BlueprintType)
enum class EItemSlotType : uint8
{
	Left,
	Right,

	Count	UMETA(Hidden)
};

// ��� ���� ������ ����
UENUM(BlueprintType)
enum class EItemHandType : uint8
{
	LeftHand,
	RightHand,
	TwoHand,

	Count	UMETA(Hidden)
};


// Item �з�
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Armor,
	Weapon,
	Utility,

	Count	UMETA(Hidden)
};

// ���� ����
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed,
	OneHandSword,
	TwoHandSword,
	Gun,

	Count	UMETA(Hidden)
};

// Utillity ����
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