#include "main.h"

void shutdown(EFI_SYSTEM_TABLE* st)
{
	st->ConOut->ClearScreen(st->ConOut);

	st->ConOut->OutputString(st->ConOut, L"Shutting Down");

	st->BootServices->Stall(1000000);

	st->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
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

	// Display a simple message indicating that GoldOS has been booted and it's running what version
	ConOut->OutputString(ConOut, L"GoldOS Genesis\n\r");

	// Display the firmware vendor information
	ConOut->OutputString(ConOut, L"Vendor: ");
	ConOut->OutputString(ConOut, st->FirmwareVendor);
	ConOut->OutputString(ConOut, L"\n\r");

	// We need to do extra stuff to display the revision
	ConOut->OutputString(ConOut, L"Revision: ");

	// Convert hexadecimal value to decimal string
	CHAR16 revisionString[11];  // Assuming a 32-bit hexadecimal value
	UINT32 value = st->FirmwareRevision;
	int index = 10;

	do {
		revisionString[--index] = L'0' + (value % 10);
		value /= 10;
	} while (value != 0);

	// Output the string
	ConOut->OutputString(ConOut, revisionString + index);
	ConOut->OutputString(ConOut, L"\n\r");


	// Tell that they can shutdown at any time, this is kind of a lie, it only works where the program reacts to esc but anyway
	ConOut->OutputString(ConOut, L"Press esc at any time to shutdown\n\r");

	// Prompt the user to press a key to continue
	ConOut->OutputString(ConOut, L"Press a key to continue.\n\r");

	// Wait for a key press
	st->BootServices->WaitForEvent(1, &(st->ConIn->WaitForKey), NULL);

	// Read the key pressed by the user
	st->ConIn->ReadKeyStroke(st->ConIn, &finalthingkey);

	// Check if the pressed key is the Esc key
	if (finalthingkey.ScanCode == SCAN_ESC)
	{

		// Perform a system shutdown
		shutdown(st);
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
