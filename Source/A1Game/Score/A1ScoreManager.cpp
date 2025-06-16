// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Score/A1ScoreManager.h"

#include "A1LogChannels.h"
#include "A1ScoreBlueprintFunctionLibrary.h"
#include "A1ScoreSaveGame.h"
#include "Kismet/GameplayStatics.h"

UA1ScoreManager* UA1ScoreManager::Instance = nullptr;
const FString UA1ScoreManager::SaveSlotName = TEXT("A1ScoreData");

UA1ScoreManager* UA1ScoreManager::Get()
{
	if (!Instance)
	{
		Instance = NewObject<UA1ScoreManager>();
		Instance->AddToRoot();
		Instance->LoadScores();
	}

	return Instance;
}

void UA1ScoreManager::StartNewGame()
{
	CurrentGameScore = FA1ScoreData();
	CurrentGameScore.GameNumber = GetNextGameNumber();

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Started Game %d"), CurrentGameScore.GameNumber);
}

void UA1ScoreManager::EndGame(EGameEndReason EndReason)
{
	CurrentGameScore.GameEndReason = EndReason;
	
	CurrentGameScore.CalculateScore(); // ���� ���� ���

	// ����� ������ �߰�
	if (!SavedScores)
	{
		SavedScores = Cast<UA1ScoreSaveGame>(UGameplayStatics::CreateSaveGameObject(UA1ScoreSaveGame::StaticClass()));
	}
	SavedScores->AddScore(CurrentGameScore);
	SaveScores();

	// �̺�Ʈ ��ε�ĳ��Ʈ
	OnGameEnded.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Game %d ended. Final score: %d"), CurrentGameScore.GameNumber, CurrentGameScore.TotalScore);
	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Score breakdown: %s"), *CurrentGameScore.GetScoreBreakdown());
}

void UA1ScoreManager::AddDaySurvived(int32 Count)
{
	CurrentGameScore.DaysSurvived += Count;
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Days survived: %d"), CurrentGameScore.DaysSurvived);
}

void UA1ScoreManager::AddInventoryItems(int32 Count)
{
	CurrentGameScore.InventoryItems += Count;
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Inventory items: %d"), CurrentGameScore.InventoryItems);
}

void UA1ScoreManager::SetInventoryItems(int32 Count)
{
	CurrentGameScore.InventoryItems = FMath::Max(0, Count);
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Inventory items: %d"), CurrentGameScore.InventoryItems);
}

int32 UA1ScoreManager::GetStorageItems() const
{
	return CurrentGameScore.StorageItems;
}

void UA1ScoreManager::SetStorageItems(int32 Count)
{
	CurrentGameScore.StorageItems = FMath::Max(0, Count);
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Storage items: %d"), CurrentGameScore.StorageItems);
}

void UA1ScoreManager::AddConsumedItems(int32 Count)
{
	CurrentGameScore.ConsumedItems += FMath::Max(0, Count);
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Consumed items: %d"), CurrentGameScore.ConsumedItems);
}

float UA1ScoreManager::GetRepairRate() const
{
	return CurrentGameScore.RepairRate;
}

void UA1ScoreManager::SetRepairRate(float Rate)
{
	CurrentGameScore.RepairRate = FMath::Clamp(Rate, 0.0f, 100.0f);
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Repair rate: %.1f%%"), CurrentGameScore.RepairRate);
}

void UA1ScoreManager::SetRepairedBeforeEscape(bool bRepaired)
{
	CurrentGameScore.bRepairedBeforeEscape = bRepaired;
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Repaired before escape: %s"),
		bRepaired ? TEXT("Yes") : TEXT("No"));
}

void UA1ScoreManager::SetRemainingFuel(int32 Fuel)
{
	CurrentGameScore.RemainingFuel = FMath::Max(0, Fuel);
	OnScoreChanged.Broadcast(CurrentGameScore);

	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Remaining fuel: %d"), CurrentGameScore.RemainingFuel);
}

bool UA1ScoreManager::SaveScores()
{
	if (!SavedScores)
	{
		return false;
	}

	bool bSaveSuccess = UGameplayStatics::SaveGameToSlot(SavedScores, SaveSlotName, 0);
	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Save result: %s"), bSaveSuccess ? TEXT("Success") : TEXT("Failed"));

	return bSaveSuccess;
}

bool UA1ScoreManager::LoadScores()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SavedScores = Cast<UA1ScoreSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

		if (SavedScores)
		{
			UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Loaded %d scores"), SavedScores->GetAllScores().Num());
			return true;
		}
	}

	// ���� ������ ���ų� �ε� ���н� ���� ����
	SavedScores = Cast<UA1ScoreSaveGame>(UGameplayStatics::CreateSaveGameObject(UA1ScoreSaveGame::StaticClass()));
	UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] Created new save data"));

	return SavedScores != nullptr;
}

TArray<FA1ScoreData> UA1ScoreManager::GetAllScores() const
{
	if (SavedScores)
	{
		return SavedScores->GetAllScores();
	}
	return TArray<FA1ScoreData>();
}

int32 UA1ScoreManager::GetHighestScore() const
{
	if (SavedScores)
	{
		return SavedScores->GetHighestScore();
	}
	return 0;
}

int32 UA1ScoreManager::GetNextGameNumber() const
{
	if (SavedScores && SavedScores->GetAllScores().Num() > 0)
	{
		// ������ ���� ��ȣ + 1
		int32 MaxGameNumber = 0;
		for (const FA1ScoreData& Score : SavedScores->GetAllScores())
		{
			MaxGameNumber = FMath::Max(MaxGameNumber, Score.GameNumber);
		}
		return MaxGameNumber + 1;
	}
	return 1; // ù ����
}

void UA1ScoreManager::ClearAllData()
{
	if (SavedScores)
	{
		SavedScores->ClearAllScore();
		SaveScores();
		UE_LOG(LogA1ScoreSystem, Log, TEXT("[ScoreManager] All score data cleared"));
	}
}

int32 UA1ScoreManager::CalculateCurrentScore() const
{
	FA1ScoreData TempScore = CurrentGameScore;
	TempScore.CalculateScore();
	return TempScore.TotalScore;
}

FString UA1ScoreManager::GetCurrentScoreBreakdown() const
{
	FA1ScoreData TempScore = CurrentGameScore;
	TempScore.CalculateScore();
	return TempScore.GetScoreBreakdown();
}
