#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE img_handle,
			   IN EFI_SYSTEM_TABLE * system_table)
{
	EFI_STATUS status;
	EFI_HII_HANDLE *hii_handles, hii;

	hii_handles = HiiGetHiiHandles(NULL);
	if (!hii_handles) {
		Print(L"E: cannot get HII handles\n");
		return EFI_OUT_OF_RESOURCES;
	}

	for (unsigned idx = 0; (hii = hii_handles[idx]); idx++) {
		EFI_HANDLE drv;

		Print(L"#%d %p: ", idx, hii);
		status = gHiiDatabase->GetPackageListHandle(gHiiDatabase, hii, &drv);
		if (!EFI_ERROR(status)) {
			CHAR16 *dp = ConvertDevicePathToText(DevicePathFromHandle(drv), FALSE, FALSE);
			Print(L"[drv %p] %s", drv, dp);
			if (dp)
				FreePool(dp);
		}
		Print(L"\n");
	}

	FreePool(hii_handles);
	return EFI_SUCCESS;
}
