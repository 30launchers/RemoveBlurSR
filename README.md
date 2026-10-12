# RemoveBlurSR
RemoveBlur for StarRail

## Build Steps

Use PowerShell.

```powershell
New-Item -ItemType Directory -Force -Path build; Set-Location build; cmake .. -G "Visual Studio 17 2022" -A x64
```

Then return to the parent directory and run:

```powershell
cmake --build build --config Release
```

## Usage

Inject the DLL using a DLL injector. Toggle the feature on/off with F8.
