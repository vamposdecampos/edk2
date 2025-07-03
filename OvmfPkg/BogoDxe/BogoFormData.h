#ifndef __BOGO_FORM_DATA_H__
#define __BOGO_FORM_DATA_H__

#include <Guid/HiiPlatformSetupFormset.h>
#include <Uefi/UefiMultiPhase.h>

#define BOGO_CONFIGURATION_FORM_ID	0x0001

// 8f80d6e2-0459-4aca-98eb-43bca82ff5e3
#define BOGO_FORMSET_GUID { 0x8f80d6e2, 0x0459, 0x4aca, { 0x98, 0xeb, 0x43, 0xbc, 0xa8, 0x2f, 0xf5, 0xe3 } }

typedef struct {
	BOOLEAN check;
} BOGO_VARSTORE_DATA;

#endif
