#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>

#include <Protocol/ShellParameters.h>

static UINTN argc;
static CHAR16 **argv;

static EFI_STATUS usage(void)
{
	Print(L"Usage:\n"
		"%s set <boot-var> <attributes> <description> <optional-data> <device-path>...\n",
		argv[0]);
	return EFI_UNSUPPORTED;
}

static void hex_dump(const char *label, const void *data, UINTN size)
{
	const char *p = (const char *) data;

	Print(L"%a (0x%x / %uB): ", label, size, size);
	while (size--)
		Print(L" %02x", (*p++) & 0xff);
	Print(L"\n");
}

static EFI_STATUS do_set(void)
{
	EFI_DEVICE_PATH_PROTOCOL *devpath;

	if (argc < 7)
		return usage();
//	boot_var = argv[2];
//	boot_attr_str = argv[3];
//	boot_desc = argv[4];
//	boot_optdata = argv[5];
	devpath = ConvertTextToDevicePath(argv[6]);
	if (!devpath) {
		Print(L"unable to convert devpath\n");
		return EFI_OUT_OF_RESOURCES;
	}
	hex_dump("DevPath", devpath, GetDevicePathSize(devpath));
	FreePool(devpath);
	return EFI_SUCCESS;
}

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

	if (argc < 2)
		return usage();

	if (!StrCmp(argv[1], L"set"))
		return do_set();
	return usage();
}
