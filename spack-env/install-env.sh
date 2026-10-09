#!/usr/bin/env bash

set -euo pipefail

SPACK_VERSION="v0.23.1"
# spack directory can not be in magix3d source directory
# because src/Core/CMakeLists use ${Python_INCLUDE_DIRS}
SPACK_PATH="../../magix3d-spack"
SPACK_DIR="${SPACK_PATH}/spack"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && /bin/pwd)"

# -------------------------------------------------------------------------
# Download Spack
# -------------------------------------------------------------------------

if [[ ! -d "${SPACK_DIR}" ]]; then
    echo -e "\e[1;32mCloning Spack ${SPACK_VERSION}\e[0m"

    git clone --depth 1 --branch "${SPACK_VERSION}" \
        https://github.com/spack/spack.git \
        "${SPACK_DIR}"
else
    echo -e "\e[1;32mSpack directory already exists: ${SPACK_DIR}\e[0m"
fi

# -------------------------------------------------------------------------
# Configure Spack
# -------------------------------------------------------------------------
echo -e "\e[1;32mGet spack recipes and tweak spack configuration\e[0m"
# adding system opengl as an external package
git clone https://github.com/LIHPC-Computational-Geometry/spack_recipes.git ${SPACK_PATH}/spack_recipes
cp ${SPACK_PATH}/spack_recipes/config/packages.yaml ${SPACK_DIR}/etc/spack/
cp ${SPACK_PATH}/spack_recipes/config/repos.yaml ${SPACK_DIR}/etc/spack/defaults/repos.yaml

echo -e "\e[1;32mInitializing spack environment\e[0m"
export SPACK_DISABLE_LOCAL_CONFIG=true
source "${SPACK_DIR}/share/spack/setup-env.sh"
#spack external find
spack external find cmake
spack external find gcc
spack external find ninja

# -------------------------------------------------------------------------
# Install Magix3d dependencies
# -------------------------------------------------------------------------

echo -e "\e[1;32mInstalling Magix3d dependencies using Spack (could be long...)\e[0m"

cd $SCRIPT_DIR
spack env activate .
spack concretize -f
spack install -j4

echo -e "\e[1;32mDone.\e[0m"
