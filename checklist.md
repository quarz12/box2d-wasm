
TODOS:
- [x] 1 pressure sensor
- [x] find source of random friction on edge -> adhesion + damping
- [x] 1 forcefield
- [x] 1 gate
  - [x] 1 valve, forcefield
  - [x] 2 inlets | pump
- [ ] 1 all observers in one Solve() to only loop through sensorcontacts once to fill multiple maps
- [ ] 1 velocity change add to totalforcebuffer 
- [x] 3 layerchange revision
  - [x] pressure calculation
- [ ] scale down to cm
- [ ] 1 black particle in droplet
- [ ] 3 building from xml
- [ ] 4 renderer
- [x] 1 arc use radians instead of start/endpoint
- [x] Sensor must tick even without contacts


attribute not changing:
- inherit from shape -> clone override vergessen
- incomplete type -> add to box2d.h

vec.length=force in newton