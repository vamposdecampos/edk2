#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>

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
	CHAR16 *boot_desc;
	UINT32 boot_attr = 0;
	UINTN optdata_size = 0;
	EFI_DEVICE_PATH_PROTOCOL *devpath;
	EFI_STATUS status = EFI_SUCCESS;

	if (argc < 7)
		return usage();
//	boot_var = argv[2];
//	boot_attr_str = argv[3];
	boot_desc = argv[4];
//	boot_optdata = argv[5];
	devpath = ConvertTextToDevicePath(argv[6]);
	if (!devpath) {
		Print(L"unable to convert devpath\n");
		return EFI_OUT_OF_RESOURCES;
	}

	UINTN devpath_size_n = GetDevicePathSize(devpath);
	hex_dump("DevPath", devpath, devpath_size_n);
	if (devpath_size_n > 0xffff) {
		Print(L"devpath too long (%u)\n", devpath_size_n);
		status = EFI_INVALID_PARAMETER;
		goto out_dp;
	}

	// TODO: size-check boot_desc
	UINT16 desc_size = StrLen(boot_desc) * 2 + 2;
	hex_dump("desc", boot_desc, desc_size);

	UINT16 devpath_size = devpath_size_n;
	UINTN loadopt_size = sizeof(boot_attr) + sizeof(devpath_size) +
		sizeof(desc_size) + desc_size +
		devpath_size + optdata_size;
	char *loadopt = AllocateZeroPool(loadopt_size);
	if (!loadopt) {
		status = EFI_OUT_OF_RESOURCES;
		goto out_dp;
	}
	char *dp = loadopt;
	CopyMem(dp, &boot_attr, sizeof(boot_attr));
	dp += sizeof(boot_attr);
	CopyMem(dp, &devpath_size, sizeof(devpath_size));
	dp += sizeof(devpath_size);
	CopyMem(dp, &desc_size, sizeof(desc_size));
	dp += sizeof(desc_size);
	CopyMem(dp, boot_desc, desc_size);
	dp += desc_size;
	CopyMem(dp, devpath, devpath_size);
	dp += devpath_size;
	// TODO: optional data
	dp += optdata_size;
	ASSERT(dp <= (loadopt + loadopt_size));

	UINTN loadopt_size_actual = dp - loadopt;
	hex_dump("loadopt", loadopt, loadopt_size);
	hex_dump("loadopt", loadopt, loadopt_size_actual);

	FreePool(loadopt);
out_dp:
	FreePool(devpath);
	return status;
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
