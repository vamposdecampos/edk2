#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

EFI_STATUS EFIAPI UefiMain(
	IN EFI_HANDLE		img_handle,
	IN EFI_SYSTEM_TABLE	*system_table)
{
	Print(L"BootFrob!\n");
	return EFI_SUCCESS;
}
