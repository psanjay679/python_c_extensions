# How to use

Install locally

```bash
pip install -e .
```

Get Loaded DLLS from PEB

```python
dll_names = _peb.get_dll_names()
```

Get DLL Base

```python
dll_base = _peb.get_dll_base(dll_name)
```

Get DLL Exports

```python
dll_exports = _peb.get_dll_exports(dll_base)
```

