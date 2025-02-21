/** @file
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>

EFI_STATUS
EFIAPI
BogoInit(
	IN	EFI_HANDLE		ImageHandle,
	IN	EFI_SYSTEM_TABLE	*SystemTable)
{
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BogoUnload(
	IN	EFI_HANDLE	ImageHandle)
{
	return EFI_SUCCESS;
}
