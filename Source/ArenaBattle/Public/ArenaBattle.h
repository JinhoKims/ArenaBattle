// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);

#define ABLOG_CALLINFO(as)	(TEXT("	ABLOG : ")+FString(__FUNCTION__) + TEXT("(")+ FString::FromInt(__LINE__)+TEXT(") (")+*FString::FromInt(as)+TEXT(")")) // 함수명과 라인 번호 전달
#define ABLOG_Short(Verbosity)	UE_LOG(ArenaBattle, Verbosity, TEXT("%s"), *ABLOG_CALLINFO(0)); // 로그 타입 1 (로그명, 중요도, 텍스트{*ABLOG_CALLINFO})
#define ABLOG_Long(Verbosity, Format, ...)	UE_LOG(ArenaBattle, Verbosity, TEXT("%s	%s"), *ABLOG_CALLINFO(1), *FString::Printf(Format, ##__VA_ARGS__)) // 로그 타입 2 (,,텍스트{*ABLOG, 가변 인자 TEXT 묶음})
#define ABCHECK(Expr,...) {if(!(Expr)) { ABLOG_Long(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; }}																																						// ... = ##__VA_ARGS , Format = TEXT()

UENUM(BlueprintType)
enum class ECharacterState : uint8 // 캐릭터 스테이트 열거형
{
	PREINIT,
	LOADING,
	READY,
	DEAD
};

/* 
	Verbosity = 로깅 수준(색상)
	ArenaBAttle = 로그 카테고리
*/

