/** @file
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/DevicePath.h>

#include "BogoFormData.h"

extern UINT8 BogoFormBin[];
extern UINT8 BogoDxeStrings[];

#pragma pack(1)
static struct {
	VENDOR_DEVICE_PATH		vendor;
	EFI_DEVICE_PATH_PROTOCOL	end;
} bogo_vendor_dp = {
	{
		{
			HARDWARE_DEVICE_PATH,
			HW_VENDOR_DP,
			{
				sizeof(VENDOR_DEVICE_PATH) & 0xff,
				(sizeof(VENDOR_DEVICE_PATH) >> 8) & 0xff,
			},
		},
		BOGO_FORMSET_GUID,
	}, {
		END_DEVICE_PATH_TYPE,
		END_ENTIRE_DEVICE_PATH_SUBTYPE,
		{
			sizeof(END_DEVICE_PATH_TYPE) & 0xff,
			(sizeof(END_DEVICE_PATH_TYPE) >> 8) & 0xff,
		},
	},
};
#pragma pack()

STATIC VOID EFIAPI
BogoReadyToBoot(
	IN	EFI_EVENT	Event,
	IN	VOID		*Context)
{
	DEBUG((DEBUG_INFO, "%a: called\n", __func__));
	gST->ConOut->OutputString(gST->ConOut, L"\n*** Ready to boot ***\n");
	gBS->Stall(3000000);
	DEBUG((DEBUG_INFO, "%a: done\n", __func__));
}

EFI_STATUS
EFIAPI
BogoInit(
	IN	EFI_HANDLE		ImageHandle,
	IN	EFI_SYSTEM_TABLE	*SystemTable)
{
	EFI_STATUS status;
	EFI_EVENT evt;
	EFI_HANDLE drv_handle = NULL;
	EFI_HANDLE hii_handle;

	DEBUG((DEBUG_INFO, "%a: called\n", __func__));


	status = gBS->InstallMultipleProtocolInterfaces(&drv_handle,
		&gEfiDevicePathProtocolGuid, &bogo_vendor_dp,
		NULL);
	DEBUG((DEBUG_INFO, "%a: install: %r, drv %p, image %p\n", __func__, status, drv_handle, ImageHandle));

	hii_handle = HiiAddPackages(&gBogoFormSetGuid, drv_handle,
		BogoFormBin,
		BogoDxeStrings,
		NULL);
	DEBUG((DEBUG_INFO, "%a: hii handle %p\n", __func__, hii_handle));

	status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
		BogoReadyToBoot, NULL,
		&gEfiEventReadyToBootGuid,
		&evt);
	if (EFI_ERROR(status))
		DEBUG((DEBUG_ERROR, "%a: CreateEventEx(ReadyToBoot): %r\n", __func__, status));
	return status;
}

EFI_STATUS
EFIAPI
BogoUnload(
	IN	EFI_HANDLE	ImageHandle)
{
	DEBUG((DEBUG_INFO, "%a: called\n", __func__));
	return EFI_SUCCESS;
}
