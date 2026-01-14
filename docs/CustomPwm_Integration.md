# Custom PWM Switch Angle Tables - Resource Integration

## Structure v1.10.0.0

The `.bin` files containing switch angle lookup tables for CHM/SHE modes must be integrated as Qt resources.

### Binary File Location

Upstream: `.upstream/VVVF-Simulator/VvvfSimulator/Vvvf/SwitchAngle/*.bin`

### Project Integration

1. **Create resources directory:**
   ```bash
   mkdir -p resources/switchangle
   ```

2. **Copy .bin files from upstream:**
   ```bash
   cp .upstream/VVVF-Simulator/VvvfSimulator/Vvvf/SwitchAngle/*.bin resources/switchangle/
   ```

3. **Create Qt resource file (`resources.qrc`):**
   ```xml
   <RCC>
       <qresource prefix="/switchangle">
           <file>switchangle/L2Chm3Default.bin</file>
           <file>switchangle/L2Chm3Alt1.bin</file>
           <file>switchangle/L2Chm3Alt2.bin</file>
           <!-- ... all other presets ... -->
       </qresource>
   </RCC>
   ```

4. **Add to CMakeLists.txt:**
   ```cmake
   qt6_add_resources(VvvfSimulator "switchangle_resources"
       PREFIX "/switchangle"
       FILES
           resources/switchangle/L2Chm3Default.bin
           resources/switchangle/L2Chm3Alt1.bin
           # ... etc
   )
   ```

### Code Usage

```cpp
#include "Vvvf/CustomPwm.hpp"

using namespace VvvfSimulator::Vvvf::CustomPwm;

// Load embedded preset
auto table = CustomPwmPresets::getPreset(CustomPwmPresets::PresetId::L2Chm7Default);
if (table && table->isValid()) {
    int pwmLevel = table->getPwm(modulationIndex, phaseAngle);
}

// Or load from custom file
auto customTable = CustomPwmTable::loadFromFile("custom_chm.bin");
if (customTable) {
    int pwmLevel = customTable->getPwm(M, X);
}
```

### Binary Format

Structure of `.bin` file:
- **Byte 0**: `SwitchCount` (uint8)
- **Bytes 1-8**: `ModulationIndexDivision` (double, little-endian)
- **Bytes 9-16**: `MinimumModulationIndex` (double, little-endian)
- **Bytes 17-20**: `BlockCount` (uint32, little-endian)
- For each block (total: `BlockCount`):
  - **Byte n**: `StartLevel` (uint8)
  - For each switch (total: `SwitchCount`):
    - **Byte n+1**: `Output` (uint8)
    - **Bytes n+2 to n+9**: `SwitchAngle` (double, little-endian)

### TODO

- [ ] Copy .bin files from upstream to `resources/switchangle/`
- [ ] Create complete `resources.qrc`
- [ ] Update CMakeLists.txt with qt6_add_resources
- [ ] Implement loading via QFile/QResource in `CustomPwmTable::loadFromResource()`
- [ ] Add preset loading tests
