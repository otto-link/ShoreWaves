# ShoreWaves
A minimalist GUI to generate shore Gerstner waves.

https://github.com/otto-link/ShoreWaves/assets/121820229/cfd6f00d-f214-498a-b91c-9de5b9419149

# License

This project is licensed under the terms of the GPL-3.0 license.

# Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

Example:
``` bash
mkdir build && cd build
cmake ..
make
```
The binary is generated in `build/bin`: 
```
cd build
bin/./shorewaves
```

# References

- Fournier, A. and Reeves, W.T. 1986. A simple model of ocean
  waves. ACM SIGGRAPH Computer Graphics 20, 4,
  75–84, [DOI](https://doi.org/10.1145/15886.15894).

- Peachey, D.R. 1986. Modeling waves and surf. Proceedings of the 13th
  annual conference on Computer graphics and interactive techniques -
  SIGGRAPH 86, ACM Press, [DOI](https://doi.org/10.1145/15922.15893).

# Dependencies
- Dear ImGui: https://github.com/ocornut/imgui
- stb_image: https://github.com/nothings/stb
- Macro-Logger: https://github.com/dmcrodrigues/macro-logger
- libInterpolate: https://github.com/CD3/libInterpolate
- FastNoiseLite: https://github.com/Auburn/FastNoiseLite
