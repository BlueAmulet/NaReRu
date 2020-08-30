/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2018 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Platform
* File     : CriWarePlatform.h
*
****************************************************************************/
#pragma once

#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
#include "ManaComponent.h"
#include "ManaTextureResource.h"
#endif	/* </cri_delete_if_LE> */

//---------------------------------------------------------
// Platform specific module interfaces
//---------------------------------------------------------

class ICriWarePlatformAtom {
protected:
	virtual ~ICriWarePlatformAtom() {}
};

#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
class ICriWarePlatformMana {
protected:
	virtual ~ICriWarePlatformMana() {}
public:
	virtual void InitializeManaLibrary() {}
	virtual void FinalizeManaLibrary() {}
	virtual void InitializeManaComponent(UManaComponent& ManaComponent) {}
	virtual void UninitializeManaComponent(UManaComponent& ManaComponent) {}
	virtual FRHITexture2D* UpdateManaTexture(const UManaTexture& Owner, const CriManaTextureBuffer& ManaTextureBuffer,
		EManaComponentTextureType ComponentType, const FIntPoint& Dimension, FRHITexture2D *Texture) 
	{ return Texture; }
};
#endif	/* </cri_delete_if_LE> */

class FCriWarePlatform
{
public:
#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
	FCriWarePlatform() : IAtom(nullptr), IMana(nullptr) {}
#else	/* </cri_delete_if_LE> */
	FCriWarePlatform() : IAtom(nullptr) {}
#endif

	void RegisterPlatformAtomInterface(ICriWarePlatformAtom* AtomInterface) {
		IAtom = AtomInterface;
	}
	void UnregisterPlatformAtomInterface() { IAtom = nullptr; }
	ICriWarePlatformAtom* Atom() { return IAtom; }
private:
	ICriWarePlatformAtom* IAtom;

#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
public:
	void RegisterPlatformManaInterface(ICriWarePlatformMana* ManaInterface) {
		IMana = ManaInterface;
	}
	void UnregisterPlatformManaInterface() { IMana = nullptr; }
	ICriWarePlatformMana* Mana() { return IMana; }

private:
	ICriWarePlatformMana* IMana;
#endif	/* </cri_delete_if_LE> */
};