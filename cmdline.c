#include "cmdline.h"

void shutdown(EFI_SYSTEM_TABLE* st)
{
	st->ConOut->ClearScreen(st->ConOut);

	st->ConOut->OutputString(st->ConOut, L"Shutting Down");

	st->BootServices->Stall(1000000);

	st->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
}

EFI_STATUS getcmd(EFI_SYSTEM_TABLE* st, CHAR16* cmdbuffer)
{
	EFI_INPUT_KEY keystroke = { SCAN_NULL, CHAR_NULL };
	UINTN index = 0;

	st->BootServices->AllocatePool(EfiConventionalMemory, 4096, &cmdbuffer);

	while (keystroke.UnicodeChar != CHAR_CARRIAGE_RETURN)
	{
		st->BootServices->WaitForEvent(1, &(st->ConIn->WaitForKey), NULL);
		st->ConIn->ReadKeyStroke(st->ConIn, &keystroke);

		st->ConOut->OutputString(st->ConOut, &keystroke.UnicodeChar);

		cmdbuffer[index] = keystroke.UnicodeChar;
		index++;

		if (keystroke.ScanCode == SCAN_ESC)
		{
			shutdown(st);
			// "Oh but what about the command variable, you have to free it."
			// The computer is going to shutdown, the memory is gonna be whiped either way.
		}
		else if (keystroke.UnicodeChar == CHAR_BACKSPACE)
		{
			if (index > 0)
			{
				index--;
				cmdbuffer[index] = CHAR_NULL;
				st->ConOut->OutputString(st->ConOut, L" ");
			}
		}
	}

	cmdbuffer[index] = L'\0';
	st->ConOut->OutputString(st->ConOut, L"\n\r");

	keystroke.ScanCode = SCAN_NULL;
	keystroke.UnicodeChar = CHAR_NULL;

	if (index > sizeof(cmdbuffer)) return 1;
	else return EFI_SUCCESS;
}

void freearr(CHAR16* var)
{
	for (int i = 0; i <= sizeof(var); i++)
	{
		var[i] = CHAR_NULL;
	}
}

EFI_STATUS cmdline(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* st)
{
	CHAR16* command;
	st->BootServices->AllocatePool(EfiConventionalMemory, 4096, &command);
	SIMPLE_TEXT_OUTPUT_INTERFACE* ConOut = st->ConOut;
	ConOut->ClearScreen(ConOut);
	ConOut->OutputString(ConOut, L"0:");

	while (1)
	{
		getcmd(st, command);
		ConOut->OutputString(ConOut, L"You typed: ");
		for (int i = 0; command[i] != L'\0' || i >= 4096; i++)
		{
			CHAR16 charString[2] = { command[i], L'\0' };
			ConOut->OutputString(ConOut, charString);
		}

		ConOut->OutputString(ConOut, L"\n\r0:");
		freearr(command);
	}

	return EFI_SUCCESS;
}