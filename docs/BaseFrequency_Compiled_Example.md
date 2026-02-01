# BaseFrequency::Compiled Structure

## Overview

The `BaseFrequency::Compiled` structure is a pre-processed, optimized version of `BaseFrequency` that computes accumulated times and frequencies during construction. This enables efficient O(log n) lookups via binary search instead of O(n) iteration.

## Original Source Points vs Compiled Points

### Source Point (BaseFrequency::Point)
```cpp
struct Point {
    int order;           // Sequence order
    double rate;         // Hz/s - frequency change rate
    double duration;     // seconds (or -1 for instant change)
    bool brake;          // true if braking pattern
    bool powerOn;        // true if power is on
};
```

### Compiled Point (Compiled::Point)
```cpp
struct Point {
    double startTime;        // Accumulated start time (seconds)
    double endTime;          // Accumulated end time (seconds)
    double startFrequency;   // Accumulated frequency at start (Hz)
    double endFrequency;     // Accumulated frequency at end (Hz)
    bool isPowerOn;          // Power state
    bool isAccel;            // true if accelerating, false if braking
};
```

## Compilation Process

The constructor processes source points in order:

1. **Sort by order**: Ensures correct sequence
2. **Handle instant changes**: Points with `duration = -1` instantly change `currentFrequency` without time passage
3. **Accumulate times**: Each point's `startTime` is the previous `endTime`
4. **Accumulate frequencies**: Frequency delta = `rate × duration × (brake ? -1 : 1)`
5. **Skip zero-duration segments**: Points with `duration = 0` are omitted

### Example Transformation

**Source points**:
```cpp
BaseFrequency baseFreq;
baseFreq.points = {
    {.order = 0, .rate = 2.0, .duration = 5.0, .brake = false, .powerOn = true},  // Accel at 2 Hz/s for 5s
    {.order = 1, .rate = 1.5, .duration = 3.0, .brake = false, .powerOn = true},  // Accel at 1.5 Hz/s for 3s
    {.order = 2, .rate = 2.0, .duration = 4.0, .brake = true,  .powerOn = false}  // Brake at -2 Hz/s for 4s
};
```

**Compiled points** (after `getCompiled()`):
```cpp
// Point 0:
startTime = 0.0,  endTime = 5.0
startFrequency = 0.0,  endFrequency = 0.0 + (2.0 * 5.0) = 10.0 Hz
isPowerOn = true,  isAccel = true

// Point 1:
startTime = 5.0,  endTime = 8.0
startFrequency = 10.0,  endFrequency = 10.0 + (1.5 * 3.0) = 14.5 Hz
isPowerOn = true,  isAccel = true

// Point 2:
startTime = 8.0,  endTime = 12.0
startFrequency = 14.5,  endFrequency = 14.5 + (-2.0 * 4.0) = 6.5 Hz
isPowerOn = false,  isAccel = false
```

## Usage Example

```cpp
#include <VvvfSimulator/Data/BaseFrequency.hpp>
#include <iostream>
#include <iomanip>

using namespace VvvfSimulator::Data;

int main() {
    // Create base frequency control pattern
    BaseFrequency baseFreq;
    baseFreq.points = {
        {.order = 0, .rate = 2.0, .duration = 5.0, .brake = false, .powerOn = true},
        {.order = 1, .rate = 1.5, .duration = 3.0, .brake = false, .powerOn = true},
        {.order = 2, .rate = 2.0, .duration = 4.0, .brake = true,  .powerOn = false}
    };

    // Compile for efficient querying
    auto compiled = baseFreq.getCompiled();

    std::cout << "Compiled Points:\n";
    std::cout << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < compiled.points.size(); ++i) {
        const auto& p = compiled.points[i];
        std::cout << "  Point " << i << ": "
                  << "t=[" << p.startTime << ", " << p.endTime << "]s, "
                  << "f=[" << p.startFrequency << ", " << p.endFrequency << "]Hz, "
                  << (p.isAccel ? "ACCEL" : "BRAKE") << ", "
                  << (p.isPowerOn ? "ON" : "OFF") << "\n";
    }

    // Query frequency at specific times
    std::cout << "\nFrequency queries:\n";
    for (double t = 0.0; t <= 12.0; t += 2.0) {
        double freq = compiled.getFreqAt(t, 0.0);
        auto pointOpt = compiled.getPointAtData(t);
        
        std::cout << "  t=" << t << "s: f=" << freq << " Hz";
        if (pointOpt) {
            std::cout << " [" << (pointOpt->isAccel ? "ACCEL" : "BRAKE") 
                      << ", " << (pointOpt->isPowerOn ? "ON" : "OFF") << "]";
        }
        std::cout << "\n";
    }

    // Estimate simulation steps
    double sampleTime = 0.001; // 1ms
    double steps = baseFreq.getEstimatedSteps(sampleTime);
    std::cout << "\nEstimated steps at " << (sampleTime * 1000) << "ms: " << steps << "\n";

    return 0;
}
```

**Output**:
```
Compiled Points:
  Point 0: t=[0.00, 5.00]s, f=[0.00, 10.00]Hz, ACCEL, ON
  Point 1: t=[5.00, 8.00]s, f=[10.00, 14.50]Hz, ACCEL, ON
  Point 2: t=[8.00, 12.00]s, f=[14.50, 6.50]Hz, BRAKE, OFF

Frequency queries:
  t=0.0s: f=0.00 Hz [ACCEL, ON]
  t=2.0s: f=4.00 Hz [ACCEL, ON]
  t=4.0s: f=8.00 Hz [ACCEL, ON]
  t=6.0s: f=11.50 Hz [ACCEL, ON]
  t=8.0s: f=14.50 Hz [BRAKE, OFF]
  t=10.0s: f=10.50 Hz [BRAKE, OFF]
  t=12.0s: f=6.50 Hz [BRAKE, OFF]

Estimated steps at 1.00ms: 12000
```

## Special Case: Instant Frequency Change

Use `duration = -1` to instantly change frequency without time passage:

```cpp
baseFreq.points = {
    {.order = 0, .rate = 5.0, .duration = -1, .brake = false, .powerOn = true},   // Set initial freq to 5 Hz
    {.order = 1, .rate = 2.0, .duration = 10.0, .brake = false, .powerOn = true}  // Accel from 5 Hz
};

auto compiled = baseFreq.getCompiled();
// Compiled will start at startFrequency=5.0 Hz (instant change is processed but not stored as point)
```

## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Construction | O(n log n) | Sorting + linear pass |
| `getPointAtNum()` | O(log n) | Binary search |
| `getPointAtData()` | O(log n) | Uses `getPointAtNum()` |
| `getFreqAt()` | O(log n) | Search + linear interpolation |

**Recommendation**: Compile once during initialization, reuse for entire simulation.

## Integration with Domain (Future)

The `checkForFreqChange()` method (TODO) will integrate with `Vvvf::Model::Domain` for real-time control:

```cpp
// Future implementation (requires Domain structure)
bool checkForFreqChange(Domain& control, const Compiled& data, 
                        const Vvvf& soundData, double timeDelta) {
    double currentTime = control.getTime();
    auto pointOpt = data.getPointAtData(currentTime);
    
    if (!pointOpt) return false;
    
    // Update control state
    control.setBraking(!pointOpt->isAccel);
    control.setPowerOff(!pointOpt->isPowerOn);
    
    // Apply frequency changes with jerk settings
    double newFreq = data.getFreqAt(currentTime, 0.0);
    control.setBaseWaveAngleFrequency(newFreq * 2.0 * M_PI);
    
    // Process with jerk limiting
    control.processControlParameter(timeDelta, soundData.jerkSetting);
    control.addTimeAll(timeDelta);
    
    return true;
}
```

## Comparison with v1.9.1.1

**Old (v1.9.1.1)**:
```cpp
// Static analysis functions
auto freq = YamlMasconAnalyze::getFreqAt(time, initial, masconData);
```

**New (v1.10.0.0)**:
```cpp
// Instance methods with compiled optimization
auto compiled = baseFreq.getCompiled();
auto freq = compiled.getFreqAt(time, initial);
```

## See Also

- [BaseFrequency.hpp](../src/VvvfSimulator/Data/BaseFrequency.hpp) - Header definition
- [BaseFrequency.cpp](../src/VvvfSimulator/Data/BaseFrequency.cpp) - Implementation
- [Analyze.cs](../.upstream/VVVF-Simulator/VvvfSimulator/Data/BaseFrequency/Analyze.cs) - Original .NET reference
- [StructCompiled.cs](../.upstream/VVVF-Simulator/VvvfSimulator/Data/BaseFrequency/StructCompiled.cs) - Original compiled structure
