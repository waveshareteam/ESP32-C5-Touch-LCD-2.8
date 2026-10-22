#!/bin/sh

# Default values
DEFAULT_TOOLCHAIN="/opt/fsl-imx-xwayland/6.12-walnascar/sysroots/x86_64-pokysdk-linux/usr/share/cmake/armv8a-poky-linux-toolchain.cmake"
USE_G2D="OFF"

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -t, --toolchain PATH          Specify toolchain file path"
    echo "  -g, --g2d                     Enable G2D (default: OFF)"
    echo "  -h, --help                    Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 -t /path/to/toolchain.cmake -g"
    echo "  $0 --toolchain /path/to/toolchain.cmake --g2d"
}

# Parse command line arguments
while [ $# -gt 0 ]; do
    case $1 in
        -t|--toolchain)
            if [ -n "$2" ]; then
                toolchain="$2"
                shift 2
            else
                echo "ERROR: --toolchain requires a path argument"
                exit 1
            fi
            ;;
        -g|--g2d)
            USE_G2D="ON"
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        *)
            # Support old style positional argument for toolchain
            if [ -z "$toolchain" ]; then
                toolchain="$1"
                shift
            else
                echo "ERROR: Unknown option: $1"
                show_usage
                exit 1
            fi
            ;;
    esac
done

# Set default toolchain if not provided
if [ -z "$toolchain" ];then
    toolchain="$DEFAULT_TOOLCHAIN"
fi

# Validate toolchain
toolchain_path=$(echo $toolchain |sed -E 's,^(.*)/sysroots/.*,\1,')
toolchain_arch=$(basename $toolchain |sed -e 's,-toolchain.cmake$,,')
if [ ! -r $toolchain -o ! -r "$toolchain_path/environment-setup-$toolchain_arch" ];then
    echo "ERROR: Yocto Toolchain not installed or not accessible"
    echo "Toolchain: $toolchain"
    echo "Environment setup: $toolchain_path/environment-setup-$toolchain_arch"
    exit 1
fi

# Determine root directory
if [ -n "$BASH_SOURCE" ]; then
    ROOTDIR="`readlink -f $BASH_SOURCE | xargs dirname`"
elif [ -n "$ZSH_NAME" ]; then
    ROOTDIR="`readlink -f $0 | xargs dirname`"
else
    ROOTDIR="`readlink -f $PWD | xargs dirname`"
fi

# Set build directory
BUILDDIR="$ROOTDIR/../../build"

# Clean and create build directory
rm -fr "$BUILDDIR"
mkdir -p "$BUILDDIR"

# Source the environment setup
echo "Setting up environment: $toolchain_path/environment-setup-$toolchain_arch"
. "$toolchain_path/environment-setup-$toolchain_arch"

# Change to build directory and run cmake and ninja build
cd $BUILDDIR
cmake -G 'Ninja' .. \
    -DCMAKE_TOOLCHAIN_FILE="$toolchain" \
    -Wno-dev \
    -DLV_BUILD_CONF_DIR="$ROOTDIR/../../lvgl" \
    -DCONFIG_LV_BUILD_DEMOS=ON \
    -DCONFIG_LV_BUILD_EXAMPLES=OFF \
    -DCONFIG_LV_USE_THORVG_INTERNAL=OFF \
    -DCONFIG_GG_USE_G2D="$USE_G2D"

ninja

# Check build result
if [ -e "$BUILDDIR/gui_guider" ]; then
    echo ""
    echo "✓ Build successful!"
    echo "Binary location: $BUILDDIR/gui_guider"
    ls -lh gui_guider
else
    echo "✗ Build failed - gui_guider binary not found"
    exit 1
fi
