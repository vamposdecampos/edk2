/** @file
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DriverBinding.h>

#include "BogoFormData.h"

EFI_STATUS
EFIAPI
bogo_get_driver_name(
	IN  EFI_COMPONENT_NAME2_PROTOCOL	*This,
	IN  CHAR8				*Language,
	OUT CHAR16				**DriverName)
{
	*DriverName = L"Bogo driver";
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
bogo_get_controller_name(
	IN  EFI_COMPONENT_NAME2_PROTOCOL	*This,
	IN  EFI_HANDLE				ControllerHandle,
	IN  EFI_HANDLE				ChildHandle		OPTIONAL,
	IN  CHAR8				*Language,
	OUT CHAR16				**ControllerName)
{
	return EFI_UNSUPPORTED;
}

static
EFI_COMPONENT_NAME2_PROTOCOL bogo_component_name2 = {
	bogo_get_driver_name,
	bogo_get_controller_name,
	"eng",
};


EFI_STATUS
EFIAPI
bogo_supported(
	IN EFI_DRIVER_BINDING_PROTOCOL *This,
	IN EFI_HANDLE			ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL	*RemainingDevicePath OPTIONAL) 
{
	DEBUG((DEBUG_INFO, "%a: ctrl %p devpath %p\n", __func__, ControllerHandle, RemainingDevicePath));
	return EFI_UNSUPPORTED;
}

static
EFI_DRIVER_BINDING_PROTOCOL bogo_driver_binding = {
	&bogo_supported,
	NULL, //&bogo_start,
	NULL, //&bogo_stop,
	0x10,
	NULL,
	NULL,
};

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

static EFI_HANDLE_PROTOCOL orig_handle_protocol;
static EFI_SET_VARIABLE orig_set_variable;

EFI_STATUS
EFIAPI
hook_handle_protocol(IN EFI_HANDLE handle, IN EFI_GUID *protocol, OUT VOID **intf)
{
	EFI_STATUS res;

	DEBUG((DEBUG_INFO, "%a: handle %x, protocol %g\n", __func__, handle, protocol));
	res = orig_handle_protocol(handle, protocol, intf);
	DEBUG((DEBUG_INFO, "%a: handle %x, protocol %g, res %r, intf %p\n", __func__, handle, protocol, res, *intf));
	return res;
}

EFI_STATUS
EFIAPI
hook_set_variable(IN CHAR16 *name, IN EFI_GUID *guid, IN UINT32 attr, IN UINTN size, IN VOID *data)
{
	EFI_STATUS res;

	res = orig_set_variable(name, guid, attr, size, data);
	DEBUG((DEBUG_INFO, "%a: '%s'-%g attr 0x%x size %d - %r\n", __func__, name, guid, attr, size, res));
	return res;
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
	EFI_TPL tpl;

	DEBUG((DEBUG_INFO, "%a: called\n", __func__));

	tpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);
	orig_handle_protocol = gBS->HandleProtocol;
	gBS->HandleProtocol = hook_handle_protocol;
	orig_set_variable = gRT->SetVariable;
	gRT->SetVariable = hook_set_variable;
	gBS->RestoreTPL(tpl);

	bogo_driver_binding.ImageHandle = ImageHandle;
	bogo_driver_binding.DriverBindingHandle = ImageHandle;

	status = gBS->InstallMultipleProtocolInterfaces(&drv_handle,
		&gEfiDevicePathProtocolGuid, &bogo_vendor_dp,
		&gEfiComponentName2ProtocolGuid, &bogo_component_name2,
		&gEfiDriverBindingProtocolGuid, &bogo_driver_binding,
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
	EFI_TPL tpl;

	DEBUG((DEBUG_INFO, "%a: called\n", __func__));

	tpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);
	gBS->HandleProtocol = orig_handle_protocol;
	gRT->SetVariable = hook_set_variable;
	gBS->RestoreTPL(tpl);

	return EFI_SUCCESS;
}
