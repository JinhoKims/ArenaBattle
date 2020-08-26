// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);

#define ABLOG_CALLINFO(as)	(TEXT("	ABLOG : ")+FString(__FUNCTION__) + TEXT("(")+ FString::FromInt(__LINE__)+TEXT(") (")+*FString::FromInt(as)+TEXT(")")) // �Լ���� ���� ��ȣ ����
#define ABLOG_Short(Verbosity)	UE_LOG(ArenaBattle, Verbosity, TEXT("%s"), *ABLOG_CALLINFO(0)); // �α� Ÿ�� 1 (�α׸�, �߿䵵, �ؽ�Ʈ{*ABLOG_CALLINFO})
#define ABLOG_Long(Verbosity, Format, ...)	UE_LOG(ArenaBattle, Verbosity, TEXT("%s	%s"), *ABLOG_CALLINFO(1), *FString::Printf(Format, ##__VA_ARGS__)) // �α� Ÿ�� 2 (,,�ؽ�Ʈ{*ABLOG, ���� ���� TEXT ����})
#define ABCHECK(Expr,...) {if(!(Expr)) { ABLOG_Long(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; }}																																						// ... = ##__VA_ARGS , Format = TEXT()

UENUM(BlueprintType)
enum class ECharacterState : uint8 // ĳ���� ������Ʈ ������
{
	PREINIT,
	LOADING,
	READY,
	DEAD
};

/* 
	Verbosity = �α� ����(����)
	ArenaBAttle = �α� ī�װ�
*/

