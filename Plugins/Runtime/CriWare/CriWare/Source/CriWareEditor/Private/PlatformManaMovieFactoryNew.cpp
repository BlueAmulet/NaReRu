
#include "PlatformManaMovieFactoryNew.h"
#include "AssetTypeCategories.h"
#include "PlatformManaMovie.h"


/* UPlatformMediaSourceFactoryNew structors
 *****************************************************************************/

UPlatformManaMovieFactoryNew::UPlatformManaMovieFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPlatformManaMovie::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}


/* UFactory interface
 *****************************************************************************/

UObject* UPlatformManaMovieFactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UPlatformManaMovie>(InParent, InClass, InName, Flags);
}

bool UPlatformManaMovieFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}
