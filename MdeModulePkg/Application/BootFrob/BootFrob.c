#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/ShellParameters.h>

static UINTN argc;
static CHAR16 **argv;

EFI_STATUS EFIAPI UefiMain(
	IN EFI_HANDLE		img_handle,
	IN EFI_SYSTEM_TABLE	*system_table)
{
	EFI_STATUS status;
	EFI_SHELL_PARAMETERS_PROTOCOL *shell_params;

	status = gBS->HandleProtocol(gImageHandle,
		&gEfiShellParametersProtocolGuid, (void **) &shell_params);
	if (EFI_ERROR(status)) {
		Print(L"E: cannot get arguments (not running from UEFI Shell?)\n");
		return status;
	}

	argc = shell_params->Argc;
	argv = shell_params->Argv;

	for (UINTN k = 0; k < argc; k++)
		Print(L"argc[%d] = '%s'\n", k, argv[k]);

	if (argc < 2) {
		Print(L"Usage: %s [...]\n", argv[0]);
		return EFI_UNSUPPORTED;
	}

	Print(L"BootFrob!\n");
	return EFI_SUCCESS;
}
