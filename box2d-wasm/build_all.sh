#!/usr/bin/env bash
set -eo pipefail
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# This script just does what the README says, except with some extra validation and interactivity.
# If you're more interested in going through step-by-step, and avoiding rebuilds of files you've 
# already built: you'll probably prefer to cherry-pick lines from the README.

Red='\033[0;31m'
Purple='\033[0;35m'
NC='\033[0m' # No Color

if ! [[ "$PWD" -ef "$DIR" ]]; then
  >&2 echo -e "${Red}This script is meant to be run from <repository_root>/box2d-wasm${NC}"
  exit 1
fi

mkdir -p "build/common"
pushd "build/common"

#set env variables since WSL keeps resetting them
export PYTHON3=/usr/bin/python3
export WEBIDL_BINDER=/usr/share/emscripten/tools/webidl_binder.py
export TARGET_TYPE=Debug

# use Box2D.idl to create ./box2d_glue.{js,cpp} for invoking functionality from libbox2d
echo -e "${Purple}executing build_idl_bindings.sh${NC}"
set -x
"$DIR/build_idl_bindings.sh"  # run build_idl_bindings.sh
{ set +x; } 2>&-
>&2 echo

echo -e "${Purple}executing build_typings.sh${NC}"
set -x
"$DIR/build_typings.sh"
{ set +x; } 2>&-

popd

FLAVOURS=(standard simd)

for i in {0..1}
do
  FLAVOUR="${FLAVOURS["$i"]}"

  # use JUST_SIMD=1 to save time when prototyping
  if [[ "$JUST_SIMD" = "1" && "$FLAVOUR" != "simd" ]]; then
    >&2 echo -e "\nSkipping '$FLAVOUR' flavour build because JUST_SIMD is set."
    continue
  fi


  FLAVOUR_DIR="build/flavour/$FLAVOUR"
  >&2 echo -e "\nMaking '$FLAVOUR' flavour in ./$FLAVOUR_DIR directory"
  mkdir -p "$FLAVOUR_DIR"
  pushd "$FLAVOUR_DIR"

  echo -e "${Purple}executing build_makefile.sh${NC}"
  set -x
  FLAVOUR="$FLAVOUR" "$DIR/build_makefile.sh"
  { set +x; } 2>&-

  >&2 echo -e "${Purple}Compiling C++ to LLVM IR (creates ./box2d/bin/libbox2d.a archive)${NC}"
  set -x
  pushd ../../../../box2d
  emmake make #TODO fix dir
  popd
  { set +x; } 2>&-
  >&2 echo

  # generate Box2D_*.{wasm,js} from glue code + libbox2d.a
  echo -e "${Purple}executing build_wasm.sh${NC}"
  set -x
  FLAVOUR="$FLAVOUR" "$DIR/build_wasm.sh"
  { set +x; } 2>&-

  >&2 echo -e "${Purple}Completed '$FLAVOUR' flavour${NC}"

  popd
done    