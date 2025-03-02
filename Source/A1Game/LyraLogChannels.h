// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

A1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyra, Log, All);
A1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraExperience, Log, All);
A1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraAbilitySystem, Log, All);
A1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraTeams, Log, All);

A1GAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
