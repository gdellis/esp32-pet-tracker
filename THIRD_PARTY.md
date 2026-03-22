# Downloads

## Git Submodules

Third-party libraries and resources referenced as git submodules.

| Submodule | URL | Purpose |
|----------|-----|--------|
| OPL_Kicad_Library | `git@github.com:Seeed-Studio/OPL_Kicad_Library.git` | Seeed Studio Open Parts Library for KiCad — specifically [`Seeed Studio Wio SX1262 for XIAO ESP32S3`](https://github.com/Seeed-Studio/OPL_Kicad_Library/tree/master/Seeed%20Studio%20Wio%20SX1262%20for%20XIAO%20ESP32S3) (footprint, symbol, 3D model) |

## Adding Submodules

```bash
git submodule add git@github.com:Seeed-Studio/OPL_Kicad_Library.git third_party/OPL_Kicad_Library
```

## Updating Submodules

```bash
# Initialize and clone
git submodule update --init --recursive

# Pull latest
git submodule update --remote third_party/OPL_Kicad_Library
```

## Removing Submodules

```bash
git submodule deinit third_party/OPL_Kicad_Library
git rm third_party/OPL_Kicad_Library
rm -rf .git/modules/third_party/OPL_Kicad_Library
```
