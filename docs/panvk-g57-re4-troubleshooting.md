# PanVK on Mali-G57 (JM kbase): RE4 briefcase rendering / FPS

Symptoms: wrong rendering on the Resident Evil 4 briefcase (attaché case)
screen, FPS stable in some scenes but not in the game.

## Fixed in this tree

- **Forced early ZS** (`PANVK_PROFILE=re4` used to switch it on). It only
  changes behaviour for shaders that `discard` or write depth/stencil/coverage,
  and for those it is incorrect: depth is updated for fragments the shader
  then kills. The briefcase UI is alpha-tested, so it shows holes/garbage.
  It is now only enabled by an explicit `PANVK_FORCE_EARLYZS=1`.
- **Pass overlap vs. the libpoly heap.** With overlap on (default), the
  vertex/tiler work of pass N+1 runs while the fragment job of pass N is still
  running. A batch that resets/uses the device-global libpoly heap
  (tessellation) could overwrite data the previous fragment job was reading.
  Such batches now use the fully serialized dependency rule.

## Isolating what is left (set in the container's environment variables)

Test in this order, one change at a time, on the briefcase screen:

| Variable            | Effect                                                    |
|---------------------|-----------------------------------------------------------|
| `PANVK_OVERLAP=0`   | Turn off vertex/tiler overlap with the previous pass.     |
| `PANVK_AFBC=1`      | Re-enable AFBC on JM (off by default since v67b).         |
| `PANVK_TILER_HEAP_MB=1024` | Larger tiler heap (default 512, each overlap half is 256). |
| `PANVK_FORCE_EARLYZS=1` | Debug only: force early ZS (expected to break discard). |

If the glitch disappears with `PANVK_OVERLAP=0`, the cause is a missing
dependency in the overlap rule (see `v67_ov` in
`src/panfrost/vulkan/jm/panvk_vX_gpu_queue.c`); report which variable fixed it.
If the FPS drop only happens with overlap on, compare with it off to see
whether it is stalls from the shared tiler heap halves.
