# Soft Body Physics Sample

A simple soft body physics simulation implementation **without using a physics engine**.

**Created by:** [PAMinerva](https://github.com/PAMinerva)

## Overview

This project demonstrates Position-Based Dynamics (PBD) for soft body simulation using tetrahedral meshes. The physics is computed directly in the application without relying on external physics libraries like Bullet, PhysX, or Havok.

## Key Features

- **No Physics Engine** - Pure constraint-based simulation
- **Tetrahedral Mesh** - Volumetric representation using tetrahedrons
- **PBD Approach** - Position-based dynamics with edge and volume constraints
- **Interactive** - Click and drag to grab and manipulate soft bodies

## Performance Warning

⚠️ **This implementation is O(n) per soft body and does NOT scale well!**

Performance will degrade significantly after **3-5 soft bodies** depending on mesh complexity. This is because:
- Each soft body is solved independently without spatial optimization
- No broad-phase collision detection
- Constraint solving is done serially
- No parallel processing or GPU acceleration

**Recommended use:** Educational purposes and small-scale simulations only.

## How It Works

The simulation loop follows the PBD pattern:

1. **PreSolve** - Apply gravity and explicit integration
2. **Solve** - Enforce edge length and volume constraints
3. **PostSolve** - Update velocities based on position changes

Each frame is subdivided into multiple substeps for stability.

## Controls

- **WASD** - Move camera
- **Right Mouse** - Rotate camera
- **Left Mouse** - Grab and drag soft bodies
- **GUI Buttons** - Run/Stop, Restart, Squash, Add Body