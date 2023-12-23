#include <efi.h>
#include <efilib.h>

EFI_STATUS getcmd(EFI_SYSTEM_TABLE* st, CHAR16* cmdbuffer)
{
	EFI_INPUT_KEY keystroke = {SCAN_NULL, CHAR_NULL};
	UINTN index = 0;

	st->BootServices->AllocatePool(EfiConventionalMemory, 4096, &cmdbuffer);

	while (keystroke.UnicodeChar != CHAR_CARRIAGE_RETURN)
	{
		st->BootServices->WaitForEvent(1, &(st->ConIn->WaitForKey), NULL);
		st->ConIn->ReadKeyStroke(st->ConIn, &keystroke);

		st->ConOut->OutputString(st->ConOut, &keystroke.UnicodeChar);

		cmdbuffer[index] = keystroke.UnicodeChar;
		index++;
	}
	
	cmdbuffer[index] = L'\0';
	st->ConOut->OutputString(st->ConOut, L"\n\r");

	keystroke.ScanCode = SCAN_NULL;
	keystroke.UnicodeChar = CHAR_NULL;

	if (index > sizeof(cmdbuffer)) return 1;
	else return EFI_SUCCESS;
}

void freeit(CHAR16* var)
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
		//st->BootServices->FreePool(command); //You may think that this creates a memory leak, maybe, but for some reason it makes the program halt, so...
		ConOut->OutputString(ConOut, L"\n\r0:");
		freeit(command);
	}

	return EFI_SUCCESS;
}

// Entry point for the EFI application
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* st)
{
	// Obtain a reference to the Console Output Interface
	SIMPLE_TEXT_OUTPUT_INTERFACE* ConOut = st->ConOut;

	// Declare a variable to store the key pressed by the user
	EFI_INPUT_KEY finalthingkey;

	// Clear the screen
	ConOut->ClearScreen(ConOut);

	// Display a simple message indicating that the program is running
	ConOut->OutputString(ConOut, L"It Boots\n\r");

	// Display the firmware vendor information
	ConOut->OutputString(ConOut, L"Vendor: ");
	ConOut->OutputString(ConOut, st->FirmwareVendor);
	ConOut->OutputString(ConOut, L"\n\r");

	// Prompt the user to press a key to exit or press Esc to shut down
	ConOut->OutputString(ConOut, L"Press a key to continue, or esc to shutdown\n\r");

	// Wait for a key press
	st->BootServices->WaitForEvent(1, &(st->ConIn->WaitForKey), NULL);

	// Read the key pressed by the user
	st->ConIn->ReadKeyStroke(st->ConIn, &finalthingkey);

	// Check if the pressed key is the Esc key
	if (finalthingkey.ScanCode == SCAN_ESC)
	{
		// Display a message indicating that the system is shutting down
		ConOut->OutputString(ConOut, L"Shutting Down");

		// Stall for 3 seconds (in microseconds) before shutting down
		st->BootServices->Stall(3 * 1000000);

		// Perform a system shutdown
		st->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
	}
	else
	{
		ConOut->OutputString(ConOut, L"Continuing\n\r");
		st->BootServices->Stall(1000000);
		cmdline(ImageHandle, st);
	}

	// Return EFI_SUCCESS to indicate successful execution
	return EFI_SUCCESS;
}
