**How to execute unit tests**

```bash
# Clone magix3d test data dir project and set MAGIX3D_TEST_DATA_DIR variable
git clone --recurse-submodules https://github.com/LIHPC-Computational-Geometry/magix3d_test_data_dir.git`
export MAGIX3D_TEST_DATA_DIR=<directory cloned above>

# Setup your Spack env
source <my_spack_dir>/share/spack/setup-env.sh

# load pytest
spack load py-pytest

# setup PYTHONPATH with magix3d installation dir
export PYTHONPATH="$(spack location -i magix3d)/lib/python3.11/site-packages:${PYTHONPATH}"

# execute tests
cd <my_magix3d_source_dir>/test_link
pytest -v
```