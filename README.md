Simple code injector written in C.

Inspired by the [series from Cr0w](https://www.crow.rip/crows-nest/mal/dev/inject).

# Techniques

1.  Allocate memory in victim process with `VirtualAllocEx`
2.  Write DLL path inside allocated memory `WriteProcessMemory`
3.  Get module handle of **Kernel32.dll** with `GetModuleHandle` and read out function addresses of `LoadLibraryA` and `FreeLibrary` with `GetProcAddress`
4.  Use `CreateRemoteThread` to run `LoadLibraryA` and `FreeLibrary` inside the victim process.

# Example

![image](https://github.com/user-attachments/assets/ec9439fc-d94c-4988-a615-e6b8fd8e2617)
