/** @file
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>

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

	DEBUG((DEBUG_INFO, "%a: called\n", __func__));
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
