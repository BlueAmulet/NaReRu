/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : K2Node for Load Atom Cue Sheet
 * File     : K2Node_LoadAtomCueSheet.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "K2Node_LoadAtomCueSheet.h"

/* CRIWAREプラグインヘッダ */
#include "AtomCueSheetLoaderComponent.h"
#include "CriWareUEBackwardCompatibility.h"

/* Unreal Engine 4関連ヘッダ */
#include "UObject/UnrealType.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_ExecutionSequence.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "K2Node_LoadAtomCueSheet"

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
void UK2Node_LoadAtomCueSheet::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// The immediate continue pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// The delayed completed pin, this used to be called Then
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Completed);

	CreatePin(EGPD_Input, GetInputCategory(), USoundAtomCueSheet::StaticClass(), GetInputPinName());
	CreatePin(EGPD_Output, GetOutputCategory(), USoundAtomCueSheet::StaticClass(), GetOutputPinName());

	Super::AllocateDefaultPins();
}

void UK2Node_LoadAtomCueSheet::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	UEdGraphPin* OldThenPin = nullptr;
	UEdGraphPin* OldCompletedPin = nullptr;

	for (UEdGraphPin* CurrentPin : OldPins)
	{
		if (CurrentPin->PinName == UEdGraphSchema_K2::PN_Then)
		{
			OldThenPin = CurrentPin;
		}
		else if (CurrentPin->PinName == UEdGraphSchema_K2::PN_Completed)
		{
			OldCompletedPin = CurrentPin;
		}
	}

	if (OldThenPin && !OldCompletedPin)
	{
		// This is an old node from when Completed was called then, rename the node to Completed and allow normal rewire to take place
		OldThenPin->PinName = UEdGraphSchema_K2::PN_Completed;
	}
}

bool UK2Node_LoadAtomCueSheet::HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const
{
	UClass* SourceClass = UAtomCueSheetLoaderComponent::StaticClass();
	const bool bResult = (SourceClass != nullptr);
	if (bResult && OptionalOutput) {
		OptionalOutput->AddUnique(SourceClass);
	}
	const bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return bSuperResult || bResult;
}

void UK2Node_LoadAtomCueSheet::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(Schema);
	bool bIsErrorFree = true;

	// Sequence node, defaults to two output pins
	UK2Node_ExecutionSequence* SequenceNode = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
	SequenceNode->AllocateDefaultPins();

	// connect to input exe
	{
		UEdGraphPin* InputExePin = GetExecPin();
		UEdGraphPin* SequenceInputExePin = SequenceNode->GetExecPin();
		bIsErrorFree &= InputExePin && SequenceInputExePin && CompilerContext.MovePinLinksToIntermediate(*InputExePin, *SequenceInputExePin).CanSafeConnect();
	}

	// Create LoadAtomCueSheet function call
	UK2Node_CallFunction* CallLoadAtomCueSheetNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallLoadAtomCueSheetNode->FunctionReference.SetExternalMember(NativeFunctionName(), UAtomCueSheetLoaderComponent::StaticClass());
	CallLoadAtomCueSheetNode->AllocateDefaultPins();

	// connect load to first sequence pin
	{
		UEdGraphPin* CallFunctionInputExePin = CallLoadAtomCueSheetNode->GetExecPin();
		UEdGraphPin* SequenceFirstExePin = SequenceNode->GetThenPinGivenIndex(0);
		bIsErrorFree &= SequenceFirstExePin && CallFunctionInputExePin && Schema->TryCreateConnection(CallFunctionInputExePin, SequenceFirstExePin);
	}

	// connect then to second sequence pin
	{
		UEdGraphPin* OutputThenPin = FindPin(UEdGraphSchema_K2::PN_Then);
		UEdGraphPin* SequenceSecondExePin = SequenceNode->GetThenPinGivenIndex(1);
		bIsErrorFree &= OutputThenPin && SequenceSecondExePin && CompilerContext.MovePinLinksToIntermediate(*OutputThenPin, *SequenceSecondExePin).CanSafeConnect();
	}

	// Create Local Variable
	UK2Node_TemporaryVariable* TempVarOutput = CompilerContext.SpawnInternalVariable(this, GetOutputCategory(), NAME_None, USoundAtomCueSheet::StaticClass());

	// Create assign node
	UK2Node_AssignmentStatement* AssignNode = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph);
	AssignNode->AllocateDefaultPins();

	UEdGraphPin* LoadedObjectVariablePin = TempVarOutput->GetVariablePin();

	// connect local variable to assign node
	{
		UEdGraphPin* AssignLHSPPin = AssignNode->GetVariablePin();
		bIsErrorFree &= AssignLHSPPin && LoadedObjectVariablePin && Schema->TryCreateConnection(AssignLHSPPin, LoadedObjectVariablePin);
	}

	// connect local variable to output
	{
		UEdGraphPin* OutputObjectPinPin = FindPin(GetOutputPinName());
		bIsErrorFree &= LoadedObjectVariablePin && OutputObjectPinPin && CompilerContext.MovePinLinksToIntermediate(*OutputObjectPinPin, *LoadedObjectVariablePin).CanSafeConnect();
	}


	// connect to asset
	UEdGraphPin* CallFunctionAssetPin = CallLoadAtomCueSheetNode->FindPin(GetInputPinName());
	{
		UEdGraphPin* AssetPin = FindPin(GetInputPinName());
		ensure(CallFunctionAssetPin);

		if (AssetPin && CallFunctionAssetPin)
		{
			if (AssetPin->LinkedTo.Num() > 0)
			{
				bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*AssetPin, *CallFunctionAssetPin).CanSafeConnect();
			}
			else
			{
				// Copy literal value
				CallFunctionAssetPin->DefaultValue = AssetPin->DefaultValue;
			}
		}
		else
		{
			bIsErrorFree = false;
		}
	}

	// Create OnLoadEvent
	const FName DelegateOnLoadedParamName(TEXT("OnLoaded"));
	UK2Node_CustomEvent* OnLoadEventNode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(this, CallFunctionAssetPin, SourceGraph);
	OnLoadEventNode->CustomFunctionName = *FString::Printf(TEXT("OnLoaded_%s"), *CompilerContext.GetGuid(this));
	OnLoadEventNode->AllocateDefaultPins();
	{
		UFunction* LoadAtomCueSheetFunction = CallLoadAtomCueSheetNode->GetTargetFunction();
		CriUEProperty::FDelegateProperty* OnLoadDelegateProperty = LoadAtomCueSheetFunction ? CriUEProperty::FindFProperty<CriUEProperty::FDelegateProperty>(LoadAtomCueSheetFunction, DelegateOnLoadedParamName) : nullptr;
		UFunction* OnLoadedSignature = OnLoadDelegateProperty ? OnLoadDelegateProperty->SignatureFunction : nullptr;
		ensure(OnLoadedSignature);
		for (TFieldIterator<CriUEProperty::FProperty> PropIt(OnLoadedSignature); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			const CriUEProperty::FProperty* Param = *PropIt;
			if (!Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm))
			{
				FEdGraphPinType PinType;
				bIsErrorFree &= Schema->ConvertPropertyToPinType(Param, /*out*/ PinType);
				bIsErrorFree &= (nullptr != OnLoadEventNode->CreateUserDefinedPin(Param->GetFName(), PinType, EGPD_Output));
			}
		}
	}

	// connect delegate
	{
		UEdGraphPin* CallFunctionDelegatePin = CallLoadAtomCueSheetNode->FindPin(DelegateOnLoadedParamName);
		ensure(CallFunctionDelegatePin);
		UEdGraphPin* EventDelegatePin = OnLoadEventNode->FindPin(UK2Node_CustomEvent::DelegateOutputName);
		bIsErrorFree &= CallFunctionDelegatePin && EventDelegatePin && Schema->TryCreateConnection(CallFunctionDelegatePin, EventDelegatePin);
	}

	// connect loaded object from event to assign
	{
		UEdGraphPin* LoadedAssetEventPin = OnLoadEventNode->FindPin(TEXT("Loaded"));
		ensure(LoadedAssetEventPin);
		UEdGraphPin* AssignRHSPPin = AssignNode->GetValuePin();
		bIsErrorFree &= AssignRHSPPin && LoadedAssetEventPin && Schema->TryCreateConnection(LoadedAssetEventPin, AssignRHSPPin);
	}

	// connect assign exec input to event output
	{
		UEdGraphPin* OnLoadEventOutputPin = OnLoadEventNode->FindPin(UEdGraphSchema_K2::PN_Then);
		UEdGraphPin* AssignInputExePin = AssignNode->GetExecPin();
		bIsErrorFree &= AssignInputExePin && OnLoadEventOutputPin && Schema->TryCreateConnection(AssignInputExePin, OnLoadEventOutputPin);
	}

	// connect assign exec output to output
	{
		UEdGraphPin* OutputCompletedPin = FindPin(UEdGraphSchema_K2::PN_Completed);
		UEdGraphPin* AssignOutputExePin = AssignNode->GetThenPin();
		bIsErrorFree &= OutputCompletedPin && AssignOutputExePin && CompilerContext.MovePinLinksToIntermediate(*OutputCompletedPin, *AssignOutputExePin).CanSafeConnect();
	}

	if (!bIsErrorFree)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "K2Node_LoadAtomCueSheet: Internal connection error. @@").ToString(), this);
	}

	BreakAllNodeLinks();
}

FText UK2Node_LoadAtomCueSheet::GetTooltipText() const
{
	return FText(LOCTEXT("UK2Node_LoadAtomCueSheetGetTooltipText", "Async Load Atom Cue Sheet"));
}

FText UK2Node_LoadAtomCueSheet::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText(LOCTEXT("UK2Node_LoadAtomCueSheetGetNodeTitle", "Async Load Atom Cue Sheet"));
}

bool UK2Node_LoadAtomCueSheet::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	bool bIsCompatible = false;
	// Can only place events in ubergraphs and macros (other code will help prevent macros with latents from ending up in functions), and basicasync task creates an event node:
	EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	if (GraphType == EGraphType::GT_Ubergraph || GraphType == EGraphType::GT_Macro)
	{
		bIsCompatible = true;
	}
	return bIsCompatible && Super::IsCompatibleWithGraph(TargetGraph);
}

FName UK2Node_LoadAtomCueSheet::GetCornerIcon() const
{
	return TEXT("Graph.Latent.LatentIcon");
}

void UK2Node_LoadAtomCueSheet::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that
	// actions might have to be updated (or deleted) if their object-key is
	// mutated (or removed)... here we use the node's class (so if the node
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_LoadAtomCueSheet::GetMenuCategory() const
{
	return FText(LOCTEXT("UK2Node_LoadAtomCueSheetGetMenuCategory", "Atom Cue Sheet"));
}

const FName& UK2Node_LoadAtomCueSheet::GetInputCategory() const
{
	return UEdGraphSchema_K2::PC_SoftObject;
}

const FName& UK2Node_LoadAtomCueSheet::GetOutputCategory() const
{
	return UEdGraphSchema_K2::PC_Object;
}

const FName& UK2Node_LoadAtomCueSheet::GetInputPinName() const
{
	static const FName InputAssetPinName("Asset");
	return InputAssetPinName;
}

const FName& UK2Node_LoadAtomCueSheet::GetOutputPinName() const
{
	static const FName OutputObjectPinName("Object");
	return OutputObjectPinName;
}

FName UK2Node_LoadAtomCueSheet::NativeFunctionName() const
{
	return GET_FUNCTION_NAME_CHECKED(UAtomCueSheetLoaderComponent, LoadAtomCueSheet);
}

/***************************************************************************
*      関数定義
*      Function Definition
***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
