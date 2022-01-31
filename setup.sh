#!/bin/sh

test() {
    if ! [ -x "$(command -v "$1")" ]; then
        echo 'Error: git is not installed.' >&2
        exit 1
    else
        echo "[YES] $1"
    fi
}
# check needed tools
for want in "cmake" "git"; do
    test "$want"
done

# download vcpkg
if ! [ -d "./.vcpkg/" ]; then
    echo "Download vcpkg..."
    git clone https://github.com/microsoft/vcpkg .vcpkg >/dev/null
    echo "Bootstrap vcpkg..."
    sh ./.vcpkg/bootstrap-vcpkg.sh >/dev/null
fi

# install dependency
vcpkg="./.vcpkg/vcpkg"
test "$vcpkg"
echo "Download dependencies..."
"$vcpkg" install gtest fmt >/dev/null

echo "Done"
