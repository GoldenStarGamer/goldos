#pragma once
#include <efi.h>

void shutdown(EFI_SYSTEM_TABLE* st);

EFI_STATUS getcmd(EFI_SYSTEM_TABLE* st, CHAR16* cmdbuffer);

void freearr(CHAR16* var);

EFI_STATUS cmdline(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* st);