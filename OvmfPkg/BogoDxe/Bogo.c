/** @file
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
BogoInit(
	IN	EFI_HANDLE		ImageHandle,
	IN	EFI_SYSTEM_TABLE	*SystemTable)
{
	DEBUG((DEBUG_INFO, "%a: called\n", __func__));
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BogoUnload(
	IN	EFI_HANDLE	ImageHandle)
{
	DEBUG((DEBUG_INFO, "%a: called\n", __func__));
	return EFI_SUCCESS;
}
