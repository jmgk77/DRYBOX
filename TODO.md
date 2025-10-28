Future Feature Ideas & Improvements

### 1. Enhanced Monitoring & Feedback
- [ ] Filament Weight Sensor Integration: Add a load cell to verify weight loss (of water), and potentially end the drying cycle earlier if efficiency drops.


### 2. Advanced Control & Automation
- [ ] Filament Profile Management: Allow users to create, save, and select different drying profiles (PLA, PETG, ABS, etc.) via the web UI.
- [ ] Adaptive Drying Logic: Make the drying cycle end based on reaching a target humidity level, not just a fixed time.
- [ ] Ventilation Control: Fully implement the VENT_OPEN/VENT_CLOSE commands to control a physical servo/damper for the exhaust.
- [ ] Desiccant Regeneration Cycle: Implement a special high-heat cycle to regenerate reusable desiccant.

### 3. User Experience & Integration
- [ ] Improved Web UI: Enhance the web interface with more dynamic, real-time graphs and controls.
- [ ] Physical Controls: Add a single button for essential local control. Short press wakes the OLED to show status; long press starts/stops the default drying cycle.

### 4. Robustness & Safety
- [ ] Over-Temperature Protection: Implement a software cutoff and a hardware thermal fuse as a failsafe against overheating.
