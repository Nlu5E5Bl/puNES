#!/usr/bin/env bash
#
# puNES - one click Windows build.
#
# Usage:
#   bash build.sh [options]
#
# Every stage is controlled by its own switch, and the three stages a normal
# build wants are the ones that are on by default:
#
#   stage     on by default   turn it off with   force it on with
#   --------- --------------  -----------------  ---------------
#   mirrors   no              --no-mirrors       --mirrors
#   download  yes             --no-download      --download
#   build     yes             --no-build         --build
#   package   yes             --no-package       --package
#   tests     no              --no-tests         --tests
#
#   --mirrors            rewrite /etc/pacman.d/mirrorlist.{mingw,msys} to use
#                        China mirrors (this machine has no HTTP proxy). The
#                        lists are the same file for every repository, so there
#                        is nothing per platform to write here.
#   --download           install/update the toolchain and the libraries with
#                        pacman. Nothing is ever fetched from a hardcoded URL
#                        and no package name is hardcoded either: the repository
#                        is derived from the platform (see below) and the
#                        package-name prefix is read off that repository's own
#                        index.                     [default: on]
#   --build              configure with CMake/Ninja and compile.  [default: on]
#   --package            assemble ../products/puNES.              [default: on]
#   --tests              run the test suite (see below).         [default: off]
#   --shaders            only the shader regression part of it.
#   --copyout            only the copy-out launch test.
#   -j N                 parallel build jobs (default: cpu count; this machine
#                        has 8 GB and no page file, so -j2 is what fits)
#   --out=DIR            package destination (default ../products/puNES)
#   -h, --help           show this help
#
# What --tests covers:
#
#   1. the unit tests. They are built and run by the normal --build stage (they
#      are a host binary that takes seconds and they are the only guard on the
#      ROM CRC path), so they are not repeated here.
#   2. the shader regression: every built-in shader plus an external .glslp
#      preset, each launched under a watchdog.
#   3. the copy-out test: the package is copied outside the source tree and
#      launched from there with a PATH that contains no MSYS2 at all.
#
# Toolchains: Windows x86_64 builds with CLANG64, Windows ARM64 with
# CLANGARM64. Those are the only two that are supported; UCRT64 and MINGW64 are
# rejected outright with a message rather than half working. The repository is
# taken from MSYSTEM when the shell was started inside one of them, and derived
# from the machine architecture otherwise, so the same script builds either
# package without an argument.
#
# ---------------------------------------------------------------------------
# How the package is assembled, and why
# ---------------------------------------------------------------------------
#
# The goal is a directory that is complete and nothing more: every DLL the
# program needs that Windows does not itself provide must be in there, and no
# DLL that Windows does provide may be in there.
#
# 1. Qt first, by delegation.
#    Qt is the bulk of the dependency tree and it is assembled from a module
#    graph plus runtime loaded plugins that no import table mentions. Writing
#    that knowledge down by hand is a bug farm, so windeployqt does it. It must
#    be run with the toolchain on PATH so it can find the Qt DLLs to copy.
#
# 2. Everything else, by measurement.
#    windeployqt does not see non-Qt libraries, and it over/under-approximates
#    the plugin set. So afterwards we measure: ntldd -R answers "which shared
#    libraries does this PE image import, transitively, and where did each one
#    come from". MSYS2's own ldd cannot be used for this: its output is one
#    level deep and its -r is documented as currently unimplemented, so it never
#    reveals the second level (libicudt78 behind Qt6Core, and so on). A package
#    built from ldd looks complete and then dies at startup with "error while
#    loading shared libraries".
#
# 3. The rule for what to ship.
#    Run ntldd -R on a clean PATH and collect the names it reports as
#    "not found". Those are exactly the libraries this machine cannot supply
#    from Windows itself. For each of them, look in <toolchain>/bin and copy it
#    in if it is there. Nothing else is shipped, and no name is special cased:
#    the rule is "is it in the toolchain", which is why api-ms-* / ext-ms-* and
#    the Windows-only libraries need no mention anywhere.
#
# 4. Clean environment, or the answer is a lie.
#    ntldd resolves imports through the Windows loader, which means it finds
#    whatever is reachable from this MSYS2 install. Two things must be true:
#
#      a) ntldd.exe must live in the staging directory. The Windows loader
#         searches the host process's own directory first, so an ntldd sitting
#         in <toolchain>/bin resolves Qt6Core.dll out of <toolchain>/bin even
#         with PATH=/nonexistent. Copying ntldd.exe into the staging root makes
#         the host directory identical to the application directory, which is
#         what we actually want to measure. It imports only KERNEL32, imagehlp
#         and api-ms-win-crt-*, so it runs fine from any directory.
#
#      b) PATH must contain only C:\Windows\System32 and C:\Windows. Anything
#         the loader can only reach through the toolchain then reports as
#         "not found", which is exactly the set we have to ship.
#
#    With the working directory at the staging root and the target given as a
#    relative path (./punes.exe, ./platforms/qwindows.dll), the staging root is
#    the application directory for every image we examine -- including the
#    plugin DLLs that live in subdirectories. No -D flag is needed and no
#    directory walking is needed: one ntldd -R per image per round.
#
# 5. Iterate only on the unresolved names.
#    A fresh copy supplies new imports, so loop: every unresolved name that
#    exists in <toolchain>/bin is copied in, and the round repeats. When a round
#    stages nothing, the closure is complete. Typically 5 or 6 rounds.
#    Worklist is the roots only (punes.exe plus the Qt plugins); ntldd -R
#    already recursed into everything they pull, so enumerating the staged DLLs
#    one by one would only re-walk the same graph.
#
# 6. What "unresolved" is allowed to mean.
#    Names that exist in <toolchain>/bin are real dependencies that we forgot,
#    and if the loop ever ends with one of those still unresolved that is a hard
#    error. Names that exist nowhere in the toolchain are, by construction, not
#    something this build could ever have supplied: they come out of lazy
#    imports inside Windows components (the audio stack reaches for
#    AzureAttestManager.dll and friends, which live under System32 in private
#    subdirectories and are loaded by the system, not by us). That set is
#    reported and ignored.
#
# 7. Guard rails, because the expensive failure is silent.
#    Shipping a library that shadows a Windows one is the classic trap: once
#    the toolchain's opengl32.dll is in the package it wins the loader search,
#    drags in libgallium_wgl.dll and a ~137 MB libLLVM, and forces software
#    rendering. The checks below assert that it did not happen.
#
set -u

# ---------------------------------------------------------------------------
# locate ourselves and set up the toolchain environment
# ---------------------------------------------------------------------------
SRC="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Bail out with the header above when asked for help.
if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
	awk 'NR > 1 && /^#/ { sub(/^# ?/, ""); print; next } NR > 1 { exit }' "${BASH_SOURCE[0]}"
	exit 0
fi

die() { echo "error: $*" >&2; exit 1; }
step() { printf '\n=== %s ===\n' "$*"; }
note() { printf '  %s\n' "$*"; }

# /etc/profile only prepends the toolchain to PATH when MSYSTEM was already set
# when bash started, so set it here instead of relying on the caller.
#
# REPO is both the pacman repository name and the toolchain directory name. Only
# clang64 (x86_64) and clangarm64 (aarch64) are supported; anything else is an
# error rather than a silent fallback, because a build with the wrong
# toolchain produces a package that looks fine until it is run.
case "${MSYSTEM:-}" in
	CLANG64)    REPO=clang64 ;;
	CLANGARM64) REPO=clangarm64 ;;
	"")
		case "$(uname -m)" in
			aarch64|arm64) REPO=clangarm64 ;;
			x86_64|amd64)  REPO=clang64 ;;
			*)             die "unsupported machine '$(uname -m)'; use an MSYS2 CLANG64 or CLANGARM64 shell" ;;
		esac
		;;
	MINGW64)
		die "the MINGW64 (gcc) toolchain is not supported; start an MSYS2 CLANG64 shell" ;;
	MINGW32)
		die "the MINGW32 (gcc) toolchain is not supported; start an MSYS2 CLANG64 shell" ;;
	UCRT64)
		die "the UCRT64 (gcc) toolchain is not supported; start an MSYS2 CLANG64 shell" ;;
	CLANG32|MSYS|*)
		die "unsupported MSYSTEM '$MSYSTEM'; start an MSYS2 CLANG64 or CLANGARM64 shell" ;;
esac

export MSYSTEM="${MSYSTEM:-$(echo "$REPO" | tr '[:lower:]' '[:upper:]')}"
TARGET_PATH="/${REPO}/bin:/usr/bin"
export PATH="$TARGET_PATH:$PATH"

# ---------------------------------------------------------------------------
# arguments
# ---------------------------------------------------------------------------
DO_MIRRORS=no
DO_DOWNLOAD=yes
DO_BUILD=yes
DO_PACKAGE=yes
DO_TESTS=no
TESTS_ONLY=""
JOBS=""
OUT=""
# Why the test stages are not running, for the message printed when they are
# skipped. A stage that is off because there is no package to test says so,
# instead of claiming a switch was not given.
TESTS_OFF_REASON="--tests not given"

for arg in "$@"; do
	case "$arg" in
		--mirrors)     DO_MIRRORS=yes ;;
		--no-mirrors)  DO_MIRRORS=no ;;
		--download)    DO_DOWNLOAD=yes ;;
		--no-download) DO_DOWNLOAD=no ;;
		--build)       DO_BUILD=yes ;;
		--no-build)    DO_BUILD=no ;;
		--package)     DO_PACKAGE=yes ;;
		--no-package)  DO_PACKAGE=no ;;
		--tests)       DO_TESTS=yes; TESTS_ONLY="" ;;
		--no-tests)    DO_TESTS=no; TESTS_ONLY="" ;;
		--shaders)     DO_TESTS=yes; TESTS_ONLY=shaders ;;
		--copyout)     DO_TESTS=yes; TESTS_ONLY=copyout ;;
		--out=*)       OUT="${arg#*=}" ;;
		-j)            die "-j needs a number, use -jN or -j N (see --help)" ;;
		-j*)           JOBS="${arg#-j}" ;;
		-h|--help)     ;;
		*)             die "unknown option '$arg' (try --help)" ;;
	esac
done

# Asking for one test is asking for that test: --shaders and --copyout are not
# cancelled by --no-package. Without a package there is nothing to run them
# against, so they are skipped below with a message that says exactly that.
case "$JOBS" in
	"") JOBS="$(nproc 2>/dev/null || echo 4)" ;;
	*[!0-9]*) die "-j needs a number, got '$JOBS'" ;;
esac

LOGDIR="$SRC/.build-logs"
mkdir -p "$LOGDIR"

# ---------------------------------------------------------------------------
# step - mirrors
# ---------------------------------------------------------------------------
if [ "$DO_MIRRORS" = "yes" ]; then
	step "mirrors"
	stamp="$(date +%s)"
	for f in /etc/pacman.d/mirrorlist.mingw /etc/pacman.d/mirrorlist.msys; do
		[ -f "$f" ] || continue
		[ -f "$f.bak.$stamp" ] || cp -p "$f" "$f.bak.$stamp"
	done
	cat > /etc/pacman.d/mirrorlist.mingw <<'EOF'
# See https://www.msys2.org/dev/mirrors
#
# China mirrors first: this machine has no HTTP proxy configured.
# $repo is substituted with the repository name and $arch with its
# architecture, so this one file serves every repository.

## China
Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/mingw/$repo/
Server = https://mirrors.ustc.edu.cn/msys2/mingw/$repo/
Server = https://mirror.nju.edu.cn/msys2/mingw/$repo/
Server = https://mirrors.aliyun.com/msys2/mingw/$repo/
Server = https://mirrors.cloud.tencent.com/msys2/mingw/$repo/
Server = https://mirrors.bfsu.edu.cn/msys2/mingw/$repo/
Server = https://mirrors.sjtug.sjtu.edu.cn/msys2/mingw/$repo/
Server = https://mirrors.qlu.edu.cn/msys2/mingw/$repo/
Server = https://mirrors.bit.edu.cn/msys2/mingw/$repo/
Server = https://mirror.iscas.ac.cn/msys2/mingw/$repo/

## Primary (fallback)
Server = https://mirror.msys2.org/mingw/$repo/
Server = https://repo.msys2.org/mingw/$repo/
EOF
	cat > /etc/pacman.d/mirrorlist.msys <<'EOF'
# See https://www.msys2.org/dev/mirrors
#
# China mirrors first: this machine has no HTTP proxy configured.

## China
Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/msys/$arch/
Server = https://mirrors.ustc.edu.cn/msys2/msys/$arch/
Server = https://mirror.nju.edu.cn/msys2/msys/$arch/
Server = https://mirrors.aliyun.com/msys2/msys/$arch/
Server = https://mirrors.cloud.tencent.com/msys2/msys/$arch/
Server = https://mirrors.bfsu.edu.cn/msys2/msys/$arch/
Server = https://mirrors.sjtug.sjtu.edu.cn/msys2/msys/$arch/
Server = https://mirrors.qlu.edu.cn/msys2/msys/$arch/
Server = https://mirrors.bit.edu.cn/msys2/msys/$arch/
Server = https://mirror.iscas.ac.cn/msys2/msys/$arch/

## Primary (fallback)
Server = https://mirror.msys2.org/msys/$arch/
Server = https://repo.msys2.org/msys/$arch/
EOF
	note "wrote mirrorlist.mingw and mirrorlist.msys (backups: *.bak.$stamp)"
else
	step "mirrors"
	note "skipped (--mirrors not given)"
fi

# ---------------------------------------------------------------------------
# step - download (toolchain and libraries)
# ---------------------------------------------------------------------------
# A repository's name and its package-name prefix are not the same string: the
# repo 'clangarm64' publishes 'mingw-w64-clang-aarch64-*' and 'clang64'
# publishes 'mingw-w64-clang-x86_64-*'. Rather than hardcoding a translation
# table that would have to grow with every toolchain, read the prefix off the
# repository's own index: it is the dominant "mingw-w64-...-" stem. This is what
# makes the package list below correct on x86_64 and on ARM64 from one code path.
derive_prefix() {
	LC_ALL=C pacman -Sl "$REPO" 2>/dev/null |
		awk '{print $2}' |
		sed -E 's/^(mingw-w64-[a-z0-9_]+-[a-z0-9_]+)-.*/\1-/' |
		grep -E '^mingw-w64-[a-z0-9_]+-[a-z0-9_]+-$' |
		sort | uniq -c | sort -rn | head -1 |
		sed -E 's/^[[:space:]]*[0-9]+[[:space:]]+//'
}

step "download"
if [ "$DO_DOWNLOAD" = "yes" ]; then
	note "syncing package databases (repo: $REPO)"
	LC_ALL=C pacman -Sy >"$LOGDIR/pacman-sync.log" 2>&1 ||
		note "database sync reported a problem, continuing with what is cached"
else
	note "skipped (--no-download), using what is already installed"
fi

PKG="$(derive_prefix)"
if [ -z "$PKG" ]; then
	note "could not read the package prefix for repo '$REPO', using the repo name"
	PKG="mingw-w64-${REPO}-"
fi

REQUIRED=(
	"${PKG}clang"
	"${PKG}cmake"
	"${PKG}ninja"
	"${PKG}pkgconf"
	"${PKG}qt6-base"
	"${PKG}qt6-tools"      # windeployqt
	"${PKG}ffmpeg"         # recording backend, not optional
	"${PKG}ntldd"          # recursive PE import inspection, see the header
)
OPTIONAL=(
	"${PKG}qt6-svg"        # SVG icon rendering
	"${PKG}mesa"           # OpenGL driver, D3D12 backed
)

install_with_retry() {
	local tries=15 err="$LOGDIR/pacman.err" n
	for n in $(seq 1 "$tries"); do
		# LC_ALL=C keeps the "already installed" notices in English so the
		# filter below works on every locale; without it a re-run of the
		# script looks like a failure.
		if LC_ALL=C pacman -S --needed --noconfirm "$@" 2>"$err"; then
			grep -v 'is up to date -- skipping' "$err" >&2 || true
			return 0
		fi
		echo "pacman attempt $n/$tries failed, retrying (mirror may be slow)" >&2
		sleep 3
	done
	echo "--- last pacman stderr ---" >&2
	cat "$err" >&2
	return 1
}

if [ "$DO_DOWNLOAD" = "yes" ]; then
	note "installing"
	install_with_retry "${REQUIRED[@]}" || die "could not install the required packages"
	install_with_retry "${OPTIONAL[@]}" || note "optional packages unavailable, continuing"
fi

command -v clang >/dev/null 2>&1 || die "clang not on PATH ($TARGET_PATH)"
command -v cmake >/dev/null 2>&1 || die "cmake not on PATH ($TARGET_PATH)"
command -v ninja >/dev/null 2>&1 || die "ninja not on PATH ($TARGET_PATH)"
NTLDD="$(command -v ntldd || true)"
[ -n "$NTLDD" ] || die "ntldd not found; install ${PKG}ntldd"
# Resolve to absolute paths now. The dependency rounds below run with a
# deliberately crippled PATH, and a command prefix assignment such as
#   PATH="$CLEAN" timeout ...
# makes the shell look up 'timeout' in the crippled PATH and fail silently,
# which looks exactly like "no dependencies found".
TIMEOUT_BIN="$(command -v timeout || true)"
[ -n "$TIMEOUT_BIN" ] || die "coreutils timeout not found on PATH"

TRIPLE="$(clang -dumpmachine)"
case "$TRIPLE" in
	aarch64-*) ARCH=arm64 ;;
	x86_64-*)  ARCH=x86_64 ;;
	*)         ARCH="$TRIPLE" ;;
esac
note "repo      : $REPO"
note "pkg prefix: $PKG"
note "toolchain : $TRIPLE"
note "arch      : $ARCH"

TOOLCHAIN_BIN="/${REPO}/bin"

# ---------------------------------------------------------------------------
# step - build (configure + compile + unit tests)
# ---------------------------------------------------------------------------
BUILDDIR="$SRC/build"
[ -n "$OUT" ] || OUT="$(dirname "$SRC")/products/puNES"

# The one environment two separate stages have to agree on.
#
# CLEAN_PATH is what a machine with no MSYS2 installed has on PATH. It is set
# here, at the top, because both the packaging stage (every ntldd measurement
# runs under it) and the test stages (every launch of the packaged program runs
# under it) depend on it. Setting it inside one of them and reading it from the
# other is exactly the kind of mistake that makes every measurement come back
# empty and every test fail for a reason that has nothing to do with the code.
#
# TIMEOUT_TERM/TIMEOUT_KILL belong with it: a GUI program never exits on its
# own, so "still running when the watchdog fires" is what success looks like,
# and timeout reports that as 124 (TERM) or 137 (KILL after the grace period).
# Any other status means the process exited by itself, which for this program
# means it crashed. Testing kill -0 instead would be wrong: the shell reaps the
# child and its pid can stay visible, so a crash would score as a pass.
CLEAN_PATH='C:\Windows\System32;C:\Windows'
TIMEOUT_TERM=124
TIMEOUT_KILL=137
run_watchdog() {
	# $1 = seconds to let it live, $2 = log file, rest = command
	local secs="$1" log="$2"; shift 2
	: > "$log"
	( PATH="$CLEAN_PATH" "$TIMEOUT_BIN" -k 5 "$secs" "$@" ) >"$log" 2>&1
	local rc=$?
	case "$rc" in
		"$TIMEOUT_TERM"|"$TIMEOUT_KILL") return 0 ;;
		*) return 1 ;;
	esac
}

step "configure"
note "source    : $SRC"
note "build dir : $BUILDDIR"
note "output    : $OUT"
note "jobs      : $JOBS"

# The unit tests are part of the build: they are a host binary that takes
# seconds and they guard the ROM CRC path.
cmake -S "$SRC" -B "$BUILDDIR" -G Ninja \
	-DCMAKE_BUILD_TYPE=Release \
	-DENABLE_RELEASE=ON \
	-DENABLE_TESTS=ON \
	>"$LOGDIR/configure.log" 2>&1 || {
		tail -40 "$LOGDIR/configure.log" >&2
		die "cmake configure failed (see $LOGDIR/configure.log)"
	}
grep -E 'Target arch|Found Qt|FFMPEG|ffmpeg' "$LOGDIR/configure.log" | sed 's/^/  /' || true

# Flag census: confirms the release flags actually reached the compile lines,
# and that -ffast-math has not crept back in (it is not part of standard -O3
# and it changes the floating point semantics the audio path relies on). The
# vendored subprojects are re-stamped by src/CMakeLists.txt for the same reason.
# The trailing boundary keeps -pipe from matching inside substrings of longer
# tokens; -O3 needs no boundary because no longer flag starts with it.
NINJA_FILE="$BUILDDIR/build.ninja"
[ -f "$NINJA_FILE" ] || die "no build.ninja, configure did not complete"
count_flag() {
	if [ "$1" = "-O3" ]; then
		grep -o -- "-O3" "$NINJA_FILE" | wc -l | tr -d ' '
	else
		grep -oE -- "$1([ =]|\$)" "$NINJA_FILE" | wc -l | tr -d ' '
	fi
}
note "flag census in build.ninja:"
for f in -O3 -pipe -flto=auto -fdata-sections -ffunction-sections -ffast-math; do
	printf '    %-22s %s\n' "$f" "$(count_flag "$f")"
done
[ "$(count_flag -O3)" -gt 0 ] || die "the release flags never reached the compile lines"
[ "$(count_flag -ffast-math)" = "0" ] || die "-ffast-math is switched on somewhere, it must not be"

if [ "$DO_BUILD" = "yes" ]; then
	step "build"
	if cmake --build "$BUILDDIR" --parallel "$JOBS" >"$LOGDIR/build.log" 2>&1; then
		note "ok"
	else
		grep -nE 'error|Error' "$LOGDIR/build.log" | head -40 >&2
		tail -20 "$LOGDIR/build.log" >&2
		die "build failed (see $LOGDIR/build.log)"
	fi
	note "errors   : $(grep -c 'error:' "$LOGDIR/build.log" || true)"
	note "warnings : $(grep -c 'warning:' "$LOGDIR/build.log" || true)"
else
	step "build"
	note "skipped (--no-build)"
fi

EXE="$BUILDDIR/src/punes.exe"
[ -f "$EXE" ] || EXE="$(find "$BUILDDIR" -name punes.exe -print -quit)"
[ -f "$EXE" ] || die "punes.exe was not produced"
note "exe      : $(stat -c '%s' "$EXE") bytes"

step "unit tests"
CRCTEST="$BUILDDIR/tests/crc_test.exe"
[ -f "$CRCTEST" ] || CRCTEST="$(find "$BUILDDIR" -name 'crc_test.exe' -print -quit)"
if [ -n "$CRCTEST" ] && [ -f "$CRCTEST" ]; then
	if "$CRCTEST" >"$LOGDIR/crc_test.log" 2>&1; then
		sed 's/^/  /' "$LOGDIR/crc_test.log"
	else
		sed 's/^/  /' "$LOGDIR/crc_test.log" >&2
		die "crc_test failed"
	fi
else
	die "crc_test was not built (ENABLE_TESTS=ON should have produced it)"
fi

# ---------------------------------------------------------------------------
# step - package
# ---------------------------------------------------------------------------
step "package"

DEST="$OUT"
if [ "$DO_PACKAGE" != "yes" ]; then
	note "skipped (--no-package)"
	# Without a staging directory there is nothing to measure or to test.
	TESTS_OFF_REASON="no package (--no-package)"
elif [ ! -f "$EXE" ]; then
	die "no punes.exe to package"
fi

if [ "$DO_PACKAGE" = "yes" ]; then

# The destination is wiped first: a stale file left over from a previous build
# makes the package look complete when it is not, and the whole point of the
# measurement below is that what is in there is exactly what is needed.
rm -rf "$DEST"
mkdir -p "$DEST"

cp "$EXE" "$DEST/punes.exe"
for extra in dip.cfg nes20db.xml; do
	f="$SRC/misc/$extra"
	[ -f "$f" ] && cp "$f" "$DEST/$extra"
done
note "staged punes.exe, dip.cfg, nes20db.xml"

# Qt, by delegation.
WINDEPLOYQT="$(command -v windeployqt || true)"
[ -n "$WINDEPLOYQT" ] || WINDEPLOYQT="$(command -v windeployqt6 || true)"
[ -n "$WINDEPLOYQT" ] || die "windeployqt not found; install ${PKG}qt6-tools"

QTPLUGINS="$(pkgconf --variable=plugindir Qt6Core 2>/dev/null || true)"
[ -n "$QTPLUGINS" ] && [ -d "$QTPLUGINS" ] || QTPLUGINS="$TOOLCHAIN_BIN/../share/qt6/plugins"
[ -d "$QTPLUGINS" ] || die "could not locate the Qt plugin directory"
note "windeployqt: $WINDEPLOYQT"
note "Qt plugins : $QTPLUGINS"

# Run from the staging root so relative output lands next to punes.exe. The
# toolchain stays on PATH for this one call: windeployqt has to be able to read
# the Qt DLLs it is deciding to copy.
( cd "$DEST" && "$WINDEPLOYQT" \
	--release --no-translations --no-system-d3d-compiler --no-opengl-sw \
	--dir . punes.exe >"$LOGDIR/windeployqt.log" 2>&1 ) || {
		tail -20 "$LOGDIR/windeployqt.log" >&2
		die "windeployqt failed (see $LOGDIR/windeployqt.log)"
	}
note "windeployqt staged $(find "$DEST" -name '*.dll' | wc -l | tr -d ' ') dll(s)"

# ntldd.exe goes into the staging root: the loader searches the host
# executable's directory first, so this is what makes ntldd answer the question
# "what does this package need", instead of "what can this MSYS2 install find".
# It stays there until the end of the script; removing it early would leave the
# rounds and the verification pass with nothing to run, and every measurement
# would silently come back empty.
cp "$NTLDD" "$DEST/ntldd.exe" || die "could not copy ntldd into the staging root"

# Every shippable library, keyed by lower case name. The case matters: lookups
# use ${name,,}, so a mixed case key silently misses and the library is dropped
# (Qt6Core.dll and libSvtAv1Enc-4.dll both disappear that way).
declare -A LIBIDX=()
while IFS= read -r f; do
	[ -n "$f" ] || continue
	b="${f##*/}"
	LIBIDX["${b,,}"]="$b"
done < <(find "$TOOLCHAIN_BIN" -maxdepth 1 -name '*.dll' 2>/dev/null)

# The roots of the measurement: the executable plus every DLL windeployqt
# staged (by definition the Qt plugins, which live in subdirectories and which
# no import table mentions). ntldd -R recurses from each root into its whole
# import graph, so the libraries we stage during the rounds are not roots: they
# are reached through the plugin or executable that needs them.
#
# This list is captured *before* the rounds on purpose. If it were recomputed
# afterwards it would contain all ~110 staged libraries, and the verify pass
# below would re-walk the same graph 110 times, once per library, instead of
# twice per plugin.
mapfile -t ROOTS < <(
	cd "$DEST" && find . -name '*.dll' | sed 's|^\./||' | sort -u
)
mapfile -t _exe < <( cd "$DEST" && find . -name 'punes.exe' | sed 's|^\./||' )
ROOTS=("${_exe[@]}" "${ROOTS[@]}")
note "measurement roots: ${#ROOTS[@]} (punes.exe + $((${#ROOTS[@]} - 1)) Qt plugin(s))"

# What clean-PATH ntldd reports as not found is, by definition, what Windows
# does not supply. Those are the names we have to go and find in the toolchain.
unresolved_of() {
	# $1 = path relative to the staging root. Run from the staging root with
	# only the Windows directories on PATH, so that anything only reachable
	# through the toolchain correctly reports as "not found".
	( cd "$DEST" && PATH="$CLEAN_PATH" "$TIMEOUT_BIN" -k 5 90 \
		"$DEST/ntldd.exe" -R "./$1" 2>/dev/null ) \
		| sed -n 's/^[[:space:]]*\([^=]*[^= ]\)[[:space:]]*=>[[:space:]]*not found.*/\1/p' \
		| sed 's/[[:space:]]*$//' | sort -u
}

# Report what is unresolved about one root, split into "the toolchain has this
# and it is not in the package" (a hole) and "nothing on this machine could
# supply this" (the system's business).
measure_root() {
	# $1 = root. Accumulates into HOLE_LIST / HIDDEN_NAMES.
	while IFS= read -r name; do
		[ -n "$name" ] || continue
		if [ -n "${LIBIDX[${name,,}]:-}" ]; then
			[ -f "$DEST/${LIBIDX[${name,,}]}" ] && continue
			HOLE_LIST+=("$name  <- $1")
		else
			HIDDEN_NAMES["${name,,}"]="$name"
		fi
	done < <(unresolved_of "$1")
}

HOLE_LIST=()
declare -A HIDDEN_NAMES=()

round=0
CANDIDATES=("${ROOTS[@]}")
while : ; do
	round=$((round + 1))
	[ "$round" -le 30 ] || die "dependency closure did not settle after 30 rounds"

	staged=0
	NEXT=()
	for root in "${CANDIDATES[@]}"; do
		before=$staged
		# Every name ntldd -R reports as not found, that exists in the
		# toolchain: copy it in from there.
		while IFS= read -r name; do
			[ -n "$name" ] || continue
			canon="${LIBIDX[${name,,}]:-}"
			[ -n "$canon" ] || continue
			[ -f "$DEST/$canon" ] && continue
			cp "$TOOLCHAIN_BIN/$canon" "$DEST/$canon" || die "could not copy $canon"
			staged=$((staged + 1))
		done < <(unresolved_of "$root")
		# A root that supplied nothing has a saturated closure: the next round
		# would copy nothing from it again, because resolving other roots'
		# dependencies cannot add names to its own import graph. Retire it.
		[ "$staged" -gt "$before" ] && NEXT+=("$root")
	done
	note "round $round: staged $staged"
	[ "$staged" -eq 0 ] && break
	CANDIDATES=("${NEXT[@]}")
done

# verify - the package must be closed, and it must not shadow Windows.
step "verify"

# Re-measure the finished package. Unlike the rounds above, this walks every
# single file in the package, not just the plugins: a hole inside one of the
# libraries we staged is only visible by measuring that library as a root.
# It is slower (one ntldd per file, ~120 of them) and that is the point -- this
# is the check that says the package is closed.
VERIFY_ROOTS=("${ROOTS[@]}")
declare -A _isroot=()
for r in "${ROOTS[@]}"; do _isroot["$r"]=1; done
while IFS= read -r f; do
	[ -n "$f" ] || continue
	[ -n "${_isroot[$f]:-}" ] && continue
	VERIFY_ROOTS+=("$f")
done < <( cd "$DEST" && find . -name '*.dll' | sed 's|^\./||' | sort -u )
note "verifying ${#VERIFY_ROOTS[@]} image(s)"
if [ ! -f "$DEST/ntldd.exe" ]; then
	die "ntldd.exe is missing from the staging root, every measurement would be a lie"
fi
for root in "${VERIFY_ROOTS[@]}"; do
	measure_root "$root"
done
if [ "${#HOLE_LIST[@]}" -gt 0 ]; then
	printf '  %s\n' "${HOLE_LIST[@]}" >&2
	die "${#HOLE_LIST[@]} dependenc(ies) in the toolchain never made it into the package"
fi

# Of the names left unresolved, the api-ms-* and ext-ms-* families are API set
# contracts: they are not files on disk at all, the loader resolves them
# through the API set schema. A handful of real names remain, all of them
# lazily loaded by Windows components (the audio and attestation stacks reach
# into System32 subdirectories that are not on the search path). Both are the
# system's business, and the split is printed so the claim stays checkable.
api_set=0
other=()
for k in "${!HIDDEN_NAMES[@]}"; do
	case "$k" in
		api-ms-*|ext-ms-*) api_set=$((api_set + 1)) ;;
		*) other+=("${HIDDEN_NAMES[$k]}") ;;
	esac
done
note "closure complete: every library the toolchain provides is packaged"
note "unresolved names: ${#HIDDEN_NAMES[@]} distinct, unavailable from the toolchain"
note "  ${api_set} API set contract(s) (api-ms-* / ext-ms-*), not files on disk"
if [ "${#other[@]}" -gt 0 ]; then
	sorted_other="$(printf '%s\n' "${other[@]}" | sort -u | tr '\n' ' ')"
	note "  ${#other[@]} lazy system load(s): $sorted_other"
fi

# Guard rails. The first is the expensive one: the toolchain ships an
# opengl32.dll, and if it lands in the package it wins the loader search over
# the system one and pulls in libgallium_wgl.dll plus a ~137 MB libLLVM, while
# forcing software rendering.
step "guard rails"
SHADOWED=0
for d in "$DEST"/*.dll; do
	b="${d##*/}"
	if [ -f "/c/Windows/System32/$b" ] || [ -f "/c/Windows/$b" ]; then
		echo "  ships $b, which Windows also provides" >&2
		SHADOWED=$((SHADOWED + 1))
	fi
done
[ "$SHADOWED" -eq 0 ] || die "$SHADOWED packaged librar(ies) shadow a Windows one"
note "no packaged library shadows a Windows one"

# Belt and braces on the renderer: these three are the software rendering stack
# and must never be in a package that is supposed to use the system GL.
for banned in opengl32.dll libgallium_wgl.dll libLLVM-22.dll; do
	[ -f "$DEST/$banned" ] && die "$banned is in the package (software rendering stack)"
done
note "no software rendering stack in the package"

fi  # DO_PACKAGE

# The tests drive the packaged executable, so both switches have to agree: the
# test switch has to be on and there has to be a package to point it at. When
# the packaging stage is off, DEST does not even exist, which is why the file
# test is here rather than inside each stage. --shaders and --copyout are not
# cancelled by --no-package; they are what asks for the test, and without a
# package there is simply nothing to run them against.
if [ ! -f "$DEST/punes.exe" ]; then
	TESTS_OFF_REASON="no package"
	DO_TESTS=no
fi

if [ "$DO_TESTS" = "yes" ] && { [ "$TESTS_ONLY" = "" ] || [ "$TESTS_ONLY" = "shaders" ]; }; then
	step "shader regression"

	# Only puNES.cfg is touched, not the whole config directory: that directory
	# also holds the user's bios/, save/, screenshot/ and input.cfg, and moving
	# those out from under a running emulator to run a test would be reckless.
	# The file is restored afterwards and the checksum is compared to prove it.
	CFGDIR="$(cygpath -u "${LOCALAPPDATA:-}" 2>/dev/null)/puNES"
	[ -n "${LOCALAPPDATA:-}" ] || CFGDIR="$HOME/puNES"
	CFG="$CFGDIR/puNES.cfg"
	CFGBAK=""
	CFGSUM=""
	if [ -f "$CFG" ]; then
		CFGBAK="$(mktemp "${TMPDIR:-/tmp}/punes-cfg.XXXXXX")"
		cp -p "$CFG" "$CFGBAK"
		CFGSUM="$(md5sum "$CFG" | cut -d' ' -f1)"
		note "puNES.cfg backed up, md5 $CFGSUM"
	fi
	restore_cfg() {
		if [ -n "$CFGBAK" ] && [ -f "$CFGBAK" ]; then
			cp -p "$CFGBAK" "$CFG"
			rm -f "$CFGBAK"
			local now
			now="$(md5sum "$CFG" | cut -d' ' -f1)"
			if [ "$now" = "$CFGSUM" ]; then
				echo "  puNES.cfg restored, md5 $now (verified)"
			else
				echo "  WARNING: puNES.cfg md5 is $now, expected $CFGSUM" >&2
			fi
		fi
	}
	trap restore_cfg EXIT

	mkdir -p "$CFGDIR"

	rc=0
	# 1. every built in shader, driven by name.
	for s in none crtdotmask crtscanlines crtcurve emboss noise ntsc2phcomp oldtv; do
		if run_watchdog 15 "$LOGDIR/shader-$s.log" \
			"$DEST/punes.exe" --shader "$s"; then
			printf '  %-14s alive=yes errors=%s\n' "$s" \
				"$(grep -ci 'error' "$LOGDIR/shader-$s.log" || true)"
		else
			printf '  %-14s alive=NO  errors=%s\n' "$s" \
				"$(grep -ci 'error' "$LOGDIR/shader-$s.log" || true)"
			head -5 "$LOGDIR/shader-$s.log" | sed 's/^/      /'
			rc=1
		fi
	done

	# 2. an external .glslp preset, which exercises a different code path:
	#    the preset parser, the relative shader0= resolution and loading a
	#    .glsl file off disk. The preset lives in tests/shaders.
	PRESET_DIR="$DEST/.shadertest"
	rm -rf "$PRESET_DIR"
	mkdir -p "$PRESET_DIR"
	cp "$SRC/tests/shaders/test.glsl" "$SRC/tests/shaders/test.glslp" "$PRESET_DIR/"
	printf 'shader=file\nshader file=%s\n' "$PRESET_DIR/test.glslp" > "$CFG"
	if run_watchdog 15 "$LOGDIR/shader-preset.log" "$DEST/punes.exe"; then
		printf '  %-14s alive=yes errors=%s\n' "glslp-preset" \
			"$(grep -ci 'error' "$LOGDIR/shader-preset.log" || true)"
	else
		printf '  %-14s alive=NO  errors=%s\n' "glslp-preset" \
			"$(grep -ci 'error' "$LOGDIR/shader-preset.log" || true)"
		head -5 "$LOGDIR/shader-preset.log" | sed 's/^/      /'
		rc=1
	fi
	rm -rf "$PRESET_DIR"

	restore_cfg
	trap - EXIT
	[ "$rc" -eq 0 ] || die "shader regression failed"
else
	step "shader regression"
	note "skipped ($TESTS_OFF_REASON)"
fi

# ntldd.exe was only ever in the package to be the measuring instrument, and
# the last measurement is behind us. Drop it here, before any test that copies
# or counts the package, so those see the real package.
if [ -f "$DEST/ntldd.exe" ]; then
	rm -f "$DEST/ntldd.exe"
fi

# ---------------------------------------------------------------------------
# standalone launch
# ---------------------------------------------------------------------------
# The package has to run with the source tree and the toolchain both irrelevant.
# Cheap, uses the real package, and it is the check that catches a packaging
# mistake that the static analysis above cannot see. Always on when a package
# was produced.
if [ "$DO_PACKAGE" = "yes" ] && [ -f "$DEST/punes.exe" ]; then
	step "standalone launch"
	if run_watchdog 15 "$LOGDIR/standalone.log" "$DEST/punes.exe"; then
		note "process alive: yes"
	else
		note "process alive: NO"
		head -10 "$LOGDIR/standalone.log" | sed 's/^/    /'
		die "the package does not start on its own"
	fi
fi

if [ "$DO_TESTS" = "yes" ] && { [ "$TESTS_ONLY" = "" ] || [ "$TESTS_ONLY" = "copyout" ]; }; then
	step "copy-out test"
	TMPDIR_WIN="$(cygpath -u "${TEMP:-C:\\Windows\\Temp}" 2>/dev/null || echo /tmp)"
	D="$(mktemp -d "$TMPDIR_WIN/punes-accept.XXXXXX")" || die "mktemp failed"
	cp -r "$DEST/." "$D"
	note "copied $(find "$D" -type f | wc -l | tr -d ' ') files to $D"
	# Run from the filesystem root with a PATH that has no MSYS2 in it at all:
	# the closest thing to a user double clicking it on a machine that has
	# never seen this build tree.
	if run_watchdog 15 "$LOGDIR/accept.log" "$D/punes.exe"; then
		note "process alive: yes"
		rc=0
	else
		note "process alive: NO"
		head -10 "$LOGDIR/accept.log" | sed 's/^/    /'
		rc=1
	fi
	rm -rf "$D"
	[ "$rc" -eq 0 ] || die "the copy-out test failed"
	note "ok: runs standalone outside the source tree"
else
	step "copy-out test"
	note "skipped ($TESTS_OFF_REASON)"
fi

# ---------------------------------------------------------------------------
# done
# ---------------------------------------------------------------------------
step "result"
if [ "$DO_PACKAGE" = "yes" ]; then
	note "package : $DEST"
	note "files   : $(find "$DEST" -type f | wc -l | tr -d ' ')"
	note "size    : $(du -sh "$DEST" | cut -f1)"
	note "plugins : $(cd "$DEST" && find . -mindepth 2 -name '*.dll' -printf '%h\n' 2>/dev/null | sort -u | tr '\n' ' ')"
else
	note "package : not produced (--no-package)"
	note "exe     : $EXE"
fi
note "logs    : $LOGDIR"

exit 0
