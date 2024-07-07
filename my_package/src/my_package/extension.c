#include <Python.h>
#include <Windows.h>
#include <winternl.h>


static PyObject *_get_ntdll_base(PyObject *self) {
    
    PPEB peb = (PPEB)__readgsqword(0x60);
    PLIST_ENTRY entry = peb->Ldr->InMemoryOrderModuleList.Flink;
    
    while (TRUE) {
        
        PDWORD64 dll_base = (PDWORD64)((unsigned char*)entry + 0x20);
        if (dll_base == NULL) {
            break;
        }

        PUNICODE_STRING base_dll_name = (PUNICODE_STRING)((unsigned char*)entry + 0x48);
        if ((wcscmp(base_dll_name->Buffer, L"ntdll.dll") == 0) ||
         (wcscmp(base_dll_name->Buffer, L"NTDLL.DLL") == 0)) {
            return PyLong_FromLongLong(*dll_base);
        }
        entry = entry->Flink;
    }
    return PyLong_FromLongLong(0);

}

static PyListObject *_get_dll_exports(PyObject *self, PyObject *args, PyObject *kwds) {

    static char* argnames[] = { "dll_base", NULL};
    unsigned char *dll_base;
    PyObject *result = PyList_New(0);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "L", argnames, &dll_base)) {
        return NULL;
    }

    PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)dll_base;
    PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)(dos_header->e_lfanew + dll_base);

    PIMAGE_EXPORT_DIRECTORY export_directory = (PIMAGE_EXPORT_DIRECTORY)(dll_base + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    if (export_directory == dll_base) {
        return result;
    }

    PDWORD names = (PDWORD)(dll_base + export_directory->AddressOfNames);
    PCHAR _api_name;
    DWORD api_len;

    for (int i = 0; i < export_directory->NumberOfNames; i++) {
        _api_name = (PCHAR)(dll_base + names[i]);
        PyObject *api_name = PyUnicode_FromStringAndSize(_api_name, strlen(_api_name));
        PyList_Append(result, api_name);
    }

    return result;
}

static PyObject *_get_dll_base(PyObject *self, PyObject *args) {
    
    char *name = NULL;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    PPEB peb = (PPEB)__readgsqword(0x60);
    PLIST_ENTRY entry = peb->Ldr->InMemoryOrderModuleList.Flink;
    
    while (TRUE) {
        
        PDWORD64 dll_base = (PDWORD64)((unsigned char*)entry + 0x20);
        if (*dll_base == NULL) {
            break;
        }

        PUNICODE_STRING base_dll_name = (PUNICODE_STRING)((unsigned char*)entry + 0x48);

        PyObject *left = PyUnicode_FromStringAndSize(name, strlen(name));
        PyObject *right = PyUnicode_FromWideChar((wchar_t*)base_dll_name->Buffer, wcslen(base_dll_name->Buffer));

        if (PyUnicode_Compare(left, right) == 0) {
            return PyLong_FromLongLong(*dll_base);
        }

        entry = entry->Flink;
    }
    return PyLong_FromLongLong(0);

}

static PyListObject *_get_dll_names(PyObject *self, PyObject *args, PyObject kwds) {

    PPEB peb = (PPEB)__readgsqword(0x60);
    PLIST_ENTRY entry = peb->Ldr->InMemoryOrderModuleList.Flink;

    PyObject *result = PyList_New(0);

    while (TRUE) {

        PDWORD64 dll_base = (PDWORD64)((unsigned char*)entry + 0x20);

        if (*dll_base == NULL) {
            break;
        }

        PUNICODE_STRING base_dll_name = (PUNICODE_STRING)((unsigned char*)entry + 0x48);

        PyList_Append(result, PyUnicode_FromWideChar((wchar_t*)base_dll_name->Buffer, wcslen(base_dll_name->Buffer)));
        entry = entry->Flink;
    }

    return result;

}

static struct PyMethodDef methods[] = {
    {"get_ntdll_base", (PyCFunction)_get_ntdll_base, METH_NOARGS},
    {"get_dll_base", (PyCFunction)_get_dll_base, METH_VARARGS},
    {"get_dll_exports", (PyCFunction)_get_dll_exports, METH_VARARGS},
    {"get_dll_names", (PyCFunction)_get_dll_names, METH_NOARGS},
    {NULL, NULL, NULL, NULL}
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "_peb",
    NULL,
    -1,
    methods
};

PyMODINIT_FUNC PyInit__peb(void) {
    return PyModule_Create(&module);
}