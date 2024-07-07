import _peb

kernel32_base = _peb.get_dll_base("KERNEL32.DLL")

dll_names = _peb.get_dll_names()

for dll_name in dll_names:

    dll_base = _peb.get_dll_base(dll_name)
    print(f"{dll_name}:")
    dll_exports = _peb.get_dll_exports(dll_base)
    if len(dll_exports):
        for export in dll_exports:
            print(f"\t{export}")
        
        break
