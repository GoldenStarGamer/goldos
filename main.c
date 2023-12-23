#include <efi.h>

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

    // Display the firmware revision information
    ConOut->OutputString(ConOut, L"Revision: ");
    ConOut->OutputString(ConOut, &st->FirmwareRevision);
    ConOut->OutputString(ConOut, L"\n\r");

    // Display the firmware vendor information
    ConOut->OutputString(ConOut, L"Vendor: ");
    ConOut->OutputString(ConOut, st->FirmwareVendor);
    ConOut->OutputString(ConOut, L"\n\r");

    // Prompt the user to press a key to exit or press Esc to shut down
    ConOut->OutputString(ConOut, L"Press a key to exit, or esc to shutdown\n\r");

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
        // Display a message indicating that the program is exiting
        ConOut->OutputString(ConOut, L"Exiting");

        // Stall for 3 seconds (in microseconds) before exiting
        st->BootServices->Stall(3 * 1000000);

        // Exit the EFI application
        st->BootServices->Exit(ImageHandle, EFI_SUCCESS, 0, NULL);
    }

    // Return EFI_SUCCESS to indicate successful execution
    return EFI_SUCCESS;
}
