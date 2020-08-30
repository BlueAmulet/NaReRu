#pragma once

#include "CriWareEditorSettings.generated.h"

UCLASS(config = Editor, defaultconfig)
class UCriWareEditorSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, config, Category = CriWareEditorSettings)
	bool EnableEconomicTickWhenImportingCueSheet;

	UPROPERTY(EditAnywhere, config, Category = CriWareEditorSettings)
	bool EnableCullingWhenImportingCueSheet;
};