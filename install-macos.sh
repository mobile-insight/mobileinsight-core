#!/bin/bash
# Installation script for mobileinsight-core on macOS
# It installs package under /usr/local folder

echo "** Installer Script for mobileinsight-core on macOS **"
echo " "
echo "  Author : Zengwen Yuan (zyuan [at] cs.ucla.edu), Yuanjie Li (yuanjiel [at] tsinghua.edu.cn), Yunqi Guo (luckiday [at] cs.ucla.edu) "
echo "  Date   : 2020-11-13"
echo "  Rev    : 4.1"
echo "  Usage  : ./install-macos.sh"
echo " "

echo "Upgrading MobileInsight..."
yes | ./uninstall.sh

# Wireshark version to install
ws_ver=3.4.0

# Use local library path
PREFIX=/usr/local
MOBILEINSIGHT_PATH=$(pwd)
WIRESHARK_SRC_PATH=${MOBILEINSIGHT_PATH}/wireshark-${ws_ver}

PYTHON=python3
PIP=pip3

GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Install up-to-date Homebrew and use default prefixes
echo -e "${GREEN}[INFO]${NC} Checking if you have Homebrew installed..."
if [[ $(which -s brew) ]] ; then
    echo -e "${YELLOW}[WARNING]${NC} It appears that Homebrew is not installed on your computer, installing..."
    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
else
    echo "Updating Homebrew"
    brew update
fi

echo -e "${GREEN}[INFO]${NC} Consulting Homebrew doctor for potential issues..."
echo -e "Output from Homebrew doctor:\n"
brew doctor
if [[ $? != 0 ]] ; then
    echo -e "${YELLOW}[WARNING]${NC} MobileInsight relies on correctly configured Python environment by Homebrew (not the Python that comes with macOS)."
    echo "It appears that Homebrew reported some issues in your environment. Please carefully review and fix them if necessary before proceeding."
    echo "If the Python environment is not correctly configured, MobileInsight may not work properly."
    echo "If you feel that the warning reported by Homebrew is not related, you may ignore it."
    echo ""

    read -r -p "Are you sure that you want to ignore the warnings reported by Homebrew and continue? [y/N] " response
    case "$response" in
        [yY][eE][sS]|[yY])
            echo "Proceed to MobileInsight installation ignoring Homebrew reported warning."
            ;;
        *)
            echo "MobileInsight installation canceled due to Homebrew reported warning. Please fix those issues and run MobileInsight installation again."
            echo "Exiting with status 2."
            exit 2
            ;;
    esac
else
    echo "All clear."
fi

echo -e "${GREEN}[INFO]${NC} Checking Python 3 environment..."
if [[ $(brew ls --versions ${PYTHON}) ]] ; then
    echo "Python 3 is installed by Homebrew and ready."
else
    echo -e "${YELLOW}[WARNING]${NC} It appears that you do not have a Python 3 version installed via Homebrew."
    echo "Installing formulae python@2 via Homebrew."
    brew install python@2
fi

if [[ $(which -s ${PIP}) ]] ; then
    echo -e "${YELLOW}[WARNING]${NC} It appears that pip2 is not installed on your computer, installing it..."
    brew install python
fi

# Check if Wireshark is installed
brew_ws_ver=$(brew ls --versions wireshark)
if [[ $? != 0 ]]; then
    echo -e "${YELLOW}[WARNING]${NC} You don't have a Wireshark installed by Homebrew, installing..."
    # Wireshark is not installed, install dependencies
    brew install pkg-config cmake
    brew install glib gnutls libgcrypt dbus
    brew install geoip c-ares
    # Install Wireshark stable version 2.0.x using our own formulae
    # brew install ./wireshark.rb
elif [[ $brew_ws_ver == *${ws_ver}* ]]; then
    echo -e "${GREEN}[INFO]${NC} You have a Wireshark version ${ws_ver} installed, continuing..."
else
    echo -e "${YELLOW}[WARNING]${NC} You have a Wireshark other than current version installed."
    echo "Installing Wireshark version ${ws_ver}..."
    brew remove wireshark
    brew install ./wireshark.rb
    brew link --overwrite wireshark
fi

echo -e "${GREEN}[INFO]${NC} Installing dependencies for compiling Wireshark libraries"
brew install wget gettext libffi qt lua

echo -e "${GREEN}[INFO]${NC} Checking Wireshark sources to compile ws_dissector"
if [ ! -d "${WIRESHARK_SRC_PATH}" ]; then
    echo -e "${GREEN}[INFO]${NC} You do not have source codes for Wireshark version ${ws_ver}, downloading..."
    # wget https://www.wireshark.org/download/src/all-versions/wireshark-${ws_ver}.tar.xz
    wget http://www.mobileinsight.net/wireshark-${ws_ver}-rbc-dissector.tar.xz -O wireshark-${ws_ver}.tar.xz
    tar xf wireshark-${ws_ver}.tar.xz
    rm wireshark-${ws_ver}.tar.xz
fi

echo -e "${GREEN}[INFO]${NC} Configuring Wireshark sources for ws_dissector compilation..."
cd ${WIRESHARK_SRC_PATH}
cmake -DBUILD_wireshark=OFF . > /dev/null 2>&1
if [[ $? != 0 ]]; then

    echo -e "${YELLOW}[WARNING]${NC} Error when executing '${WIRESHARK_SRC_PATH}/cmake --disable-wireshark .'."
    echo "You need to manually fix it before continuation. Exiting with status 3."
    exit 3
fi


echo "Compiling wireshark-${ws_ver} from source code, it may take a few minutes..."
make -j $(grep -c ^processor /proc/cpuinfo)
if [ $? != 0 ]; then
    echo "Error when compiling wireshark-${ws_ver} from source code'."
    echo "You need to manually fix it before continuation. Exiting with status 2"
    exit 2
fi
echo "Installing wireshark-${ws_ver}"
sudo make install > /dev/null 2>&1
if [ $? != 0 ]; then
    echo "Error when installing wireshark-${ws_ver} compiled from source code'."
    echo "You need to manually fix it before continuation. Exiting with status 2"
    exit 2
fi


echo -e "${GREEN}[INFO]${NC} Compiling Wireshark dissector for MobileInsight..."
cd ${MOBILEINSIGHT_PATH}/ws_dissector
if [[ -f "ws_dissector" ]]; then
    rm ws_dissector
fi
g++ ws_dissector.cpp packet-aww.cpp -o ws_dissector `pkg-config --libs --cflags glib-2.0` \
    -I"${WIRESHARK_SRC_PATH}" -L"${PREFIX}/lib" -lwireshark -lwsutil -lwiretap
strip ws_dissector

echo -e "${GREEN}[INFO]${NC} Installing Wireshark dissector to ${PREFIX}/bin"
if [[ $(cp ws_dissector ${PREFIX}/bin) ]] ; then
    chmod 755 ${PREFIX}/bin/ws_dissector
else
    sudo mkdir -p ${PREFIX}/bin
    sudo cp ws_dissector ${PREFIX}/bin
    sudo chmod 755 ${PREFIX}/bin/ws_dissector
fi

echo -e "${GREEN}[INFO]${NC} Installing mobileinsight-core..."
cd ${MOBILEINSIGHT_PATH}
if [[ $(${PYTHON} setup.py install) ]] ; then
    echo "Congratulations! mobileinsight-core is successfully installed!"
else
    echo "Installing mobileinsight-core using sudo, your password may be required..."
    sudo ${PYTHON} setup.py install
fi

echo -e "${GREEN}[INFO]${NC} Installing GUI for MobileInsight..."
cd ${MOBILEINSIGHT_PATH}

sudo mkdir -p ${PREFIX}/share/mobileinsight/
sudo cp -r gui/* ${PREFIX}/share/mobileinsight/
sudo ln -s ${PREFIX}/share/mobileinsight/mi-gui ${PREFIX}/bin/mi-gui

# if [[ $(mkdir -p ${PREFIX}/share/mobileinsight/) ]] ; then
#     cp -r gui/* ${PREFIX}/share/mobileinsight/
#     ln -s ${PREFIX}/share/mobileinsight/mi-gui ${PREFIX}/bin/mi-gui
# else
#     sudo mkdir -p ${PREFIX}/share/mobileinsight/
#     sudo cp -r gui/* ${PREFIX}/share/mobileinsight/
#     sudo ln -s ${PREFIX}/share/mobileinsight/mi-gui ${PREFIX}/bin/mi-gui
# fi

echo -e "${GREEN}[INFO]${NC} Installing dependencies for MobileInsight GUI..."
echo -e "${GREEN}[INFO]${NC} Installing wxPython..."
# brew install wxpython
sudo pip3 install wxpython

if [[ ! -f ${PREFIX}/bin/${PYTHON} ]] ; then
    echo -e "${YELLOW}[WARNING]${NC} Cannot find python installed by Homebrew under /usr/local"
    echo "Your Homebrew prefix is:"
    echo $(brew config | grep -i prefix)
    echo -e "${YELLOW}[WARNING]${NC} The MobileInsight GUI will likely not functioning if you do not use the default /usr/local prefix."
fi

if [[ $(${PIP} install pyserial matplotlib) ]] ; then
    echo "PySerial and matplotlib are successfully installed!"
else
    echo "Installing PySerial and matplotlib using sudo, your password may be required..."
    sudo ${PIP} install pyserial matplotlib
fi

echo -e "${GREEN}[INFO]${NC} Testing the MobileInsight offline analysis example."
cd ${MOBILEINSIGHT_PATH}/examples
${PYTHON} offline-analysis-example.py
if [[ $? == 0 ]] ; then
    echo "Successfully ran the offline analysis example!"
else
    echo -e "${YELLOW}[WARNING]${NC} Failed to run offline analysis example!"
    echo "Exiting with status 4."
    exit 4
fi

if [ x$DISPLAY != x ] ; then
    echo -e "${GREEN}[INFO]${NC} Testing MobileInsight GUI (you need to be in a graphic session)..."
    mi-gui
    if [[ $? == 0 ]] ; then
        echo "Successfully ran MobileInsight GUI!"
        echo "The installation of mobileinsight-core is finished!"
    else
        echo "There are issues running MobileInsight GUI, you need to fix them manually"
    fi
else
  echo "GUI Disabled"
fi

echo "The installation of mobileinsight-core is finished!"