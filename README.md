<p align="center">
  <img src="https://user-images.githubusercontent.com/14859058/132302943-a466d3d5-75c2-4bac-b0b2-7f0aeb8c058d.png" alt="puNES"/><br>
</p>
<h3 align="center">Qt-based Nintendo Entertainment System emulator and NSF/NSF2/NSFe Music Player</h3>

> **This project is a successor maintenance fork of puNES, maintained primarily by
> [deepseek-ai](https://github.com/deepseek-ai).** It tracks the same emulator
> core but moves the build to a modern, fully 64-bit toolchain: MSYS2
> CLANG64/CLANGARM64 with Qt6 and FFmpeg from pacman, OpenGL as the only
> renderer, and Windows on ARM64 as a first class target alongside x86_64. The
> ancient Cg runtime and the whole Direct3D path are gone, and so are the Linux,
> FreeBSD and OpenBSD backends.

<p align="center">
  <a href="https://github.com/punesemu/puNES/releases/latest">
    <img src="https://img.shields.io/github/release/punesemu/puNES.svg?label=latest%20release" alt="GitHub release"/>
  </a>
  <a href="https://github.com/punesemu/puNES/blob/master/COPYING">
    <img src="https://img.shields.io/github/license/punesemu/puNES.svg" alt="License"/>
  </a>
  <a href="https://crowdin.com/project/punes">
    <img src="https://badges.crowdin.net/punes/localized.svg" alt="Crowdin"/>
  </a>
  <a href="https://github.com/punesemu/puNES">
    <img src="https://img.shields.io/github/languages/code-size/punesemu/puNES?style=flat" alt="GitHub code size in bytes"/>
  </a>
  <a href="https://www.codefactor.io/repository/github/punesemu/punes/overview/master">
    <img src="https://www.codefactor.io/repository/github/punesemu/punes/badge/master" alt="CodeFactor"/>
  </a>
  <a href="https://repology.org/project/punes/versions">
    <img src="https://repology.org/badge/tiny-repos/punes.svg" alt="Packaging status"/>
  </a>
</p>
<p align="center">
  <a href='https://flathub.org/apps/details/io.github.punesemu.puNES'>
    <img width='180' height='60' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.svg'/>
  </a>
</p>

## :floppy_disk: Downloads

Windows packages are produced by [`build.sh`](build.sh) for the two supported
toolchains:

- Windows x86_64 : MSYS2 **CLANG64** (`mingw-w64-clang-x86_64-*`)
- Windows ARM64 : MSYS2 **CLANGARM64** (`mingw-w64-clang-aarch64-*`)

Notes:

- WARNING save states of version 0.110 or earlier are no longer compatible.
- 32 bit builds, the Cg renderer and the Direct3D renderer are not supported any
  more; neither are Linux, FreeBSD and OpenBSD.

## :beer: Support

If you want buy me a beer :

[![GitHub Sponsors](https://img.shields.io/badge/GitHub-Donate-EA4AAA?style=for-the-badge&logo=githubsponsors)](https://github.com/sponsors/punesemu)
[![PayPal](https://img.shields.io/badge/PayPal-Donate-blue?style=for-the-badge&logo=paypal)](https://paypal.me/punesemu)
[![kofi](https://img.shields.io/badge/Ko--Fi-Donate-orange?style=for-the-badge&logo=ko-fi)](https://ko-fi.com/punesemu)

## Multilingual Support

A big thank you to everyone who contributed to the translations:

- Arabic
- Chinese
- English
- French
- German
- Hungarian
- Italian
- Polish
- Portuguese
- Russian
- Spanish
- Turkish

### Help with Translations [here](https://crowdin.com/project/punes)

## :camera: Screenshots

<p align="center">
  <img src="https://github.com/user-attachments/assets/801b85b5-78e7-47b7-9430-4b0904f876a8" width="400" alt="puNES main window"/>
  <img src="https://github.com/user-attachments/assets/a05a88de-1feb-41e7-a531-13a4e6ba6937" width="400" alt="puNES NSF2 Player"/>
  <img src="https://github.com/user-attachments/assets/4da61eb7-349c-43ab-bc88-85f993e86cca" width="400" alt="puNES Slot Preview"/>
  <img src="https://github.com/user-attachments/assets/3a1a0a82-d3b5-4213-9f73-ec28461ce188" width="400" alt="puNES General Settings"/>
  <img src="https://github.com/user-attachments/assets/eb135545-a3d4-4ca1-91f9-4b49aafe3cef" width="400" alt="puNES Video Filters Settings"/>
  <img src="https://github.com/user-attachments/assets/9358391c-4a16-4eff-9610-d56cc18c647b" width="400" alt="puNES Cheat Editor"/>
  <img src="https://github.com/user-attachments/assets/61a152b9-3d82-46a5-8b85-322d0a908ade" width="400" alt="puNES Xbox360 Standard Controller Settings"/>
  <img src="https://github.com/user-attachments/assets/d4c9dccb-1394-44fe-9f7e-8736b4c9f548" width="400" alt="puNES PS4 Standard Controller Settings"/>
  <img src="https://github.com/user-attachments/assets/3f7e4550-a9cf-4319-83e5-a126cb079b37" width="800" alt="puNES Family BASIC Virtual Keyboard"/>
  <img src="https://github.com/user-attachments/assets/4df74b2a-7889-4cb5-948e-66360fd64707" width="800" alt="puNES Subor Virtual Keyboard"/>
</p>

## :keyboard: Configuration

To run in portable mode there is 3 distinct ways:

1. If the executable is in a folder containing the puNES.cfg file.
2. Rename the executable by adding the suffix `_p`.
   - Examples: `punes.exe -> punes_p.exe` or `punes64.exe -> punes64_p.exe`
3. Run the emulator with the "--portable" option.

To see a list of available command-line options, start puNES with the `-h` argument.

## :electric_plug: Supported Mappers

| 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  |
| 22  | 23  | 24  | 25  | 26  | 27  | 28  | 29  | 30  | 31  | 32  |
| 33  | 34  | 35  | 36  | 37  | 38  |     | 40  | 41  | 42  | 43  |
| 44  | 45  | 46  | 47  | 48  | 49  | 50  | 51  | 52  | 53  |     |
| 55  | 56  | 57  | 58  | 59  | 60  | 61  | 62  | 63  | 64  | 65  |
| 66  | 67  | 68  | 69  | 70  | 71  | 72  | 73  | 74  | 75  | 76  |
| 77  | 78  | 79  | 80  | 81  | 82  | 83  |     | 85  | 86  | 87  |
| 88  | 89  | 90  | 91  | 92  | 93  | 94  | 95  | 96  | 97  |     |
| 99  | 100 | 101 |     | 103 | 104 | 105 | 106 | 107 | 108 |     |
|     | 111 | 112 | 113 | 114 | 115 | 116 | 117 | 118 | 119 | 120 |
| 121 | 122 | 123 |     | 125 | 126 |     |     |     |     |     |
| 132 | 133 | 134 |     | 136 | 137 | 138 | 139 | 140 | 141 | 142 |
| 143 | 144 | 145 | 146 | 147 | 148 | 149 | 150 | 151 | 152 | 153 |
| 154 | 155 | 156 | 157 | 158 | 159 |     |     | 162 | 163 | 164 |
| 165 | 166 | 167 | 168 |     | 170 | 171 | 172 | 173 |     | 175 |
| 176 | 177 | 178 | 179 | 180 |     | 182 | 183 | 184 | 185 | 186 |
| 187 | 188 | 189 | 190 | 191 | 192 | 193 | 194 | 195 | 196 | 197 |
| 198 | 199 | 200 | 201 | 202 | 203 | 204 | 205 | 206 | 207 | 208 |
| 209 | 210 | 211 | 212 | 213 | 214 | 215 | 216 | 217 | 218 | 219 |
|     | 221 | 222 |     | 224 | 225 | 226 | 227 | 228 | 229 | 230 |
| 231 | 232 | 233 | 234 | 235 | 236 | 237 | 238 |     | 240 | 241 |
| 242 | 243 | 244 | 245 | 246 |     | 248 | 249 | 250 |     | 252 |
| 253 | 254 | 255 | 256 |     | 258 | 259 | 260 | 261 | 262 | 263 |
| 264 | 265 | 266 | 267 | 268 | 269 |     | 271 | 272 |     | 274 |
|     |     |     |     |     |     | 281 | 282 | 283 | 284 | 285 |
| 286 | 287 | 288 | 289 | 290 | 291 | 292 |     |     | 295 |     |
| 297 | 298 | 299 | 300 | 301 | 302 | 303 | 304 | 305 | 306 | 307 |
| 308 | 309 |     | 311 | 312 | 313 | 314 | 315 |     |     |     |
| 319 | 320 |     | 322 | 323 | 324 | 325 |     | 327 | 328 | 329 |
|     | 331 | 332 | 333 | 334 | 335 | 336 | 337 | 338 | 339 | 340 |
| 341 | 342 | 343 | 344 | 345 | 346 | 347 | 348 | 349 | 350 | 351 |
| 352 | 353 | 354 | 355 | 356 | 357 | 358 | 359 | 360 | 361 | 362 |
|     |     |     | 366 |     | 368 | 369 | 370 |     | 372 |     |
| 374 | 375 |     | 377 |     |     | 380 | 381 | 382 |     | 384 |
|     | 386 | 387 | 388 | 389 | 390 |     |     | 393 | 394 | 395 |
| 396 | 397 | 398 | 399 | 400 | 401 |     | 403 | 404 |     | 406 |
|     |     | 409 | 410 | 411 | 412 | 413 | 414 | 415 | 416 | 417 |
|     |     | 420 | 421 | 422 |     |     |     |     |     | 428 |
| 429 |     | 431 | 432 | 433 | 434 |     | 436 | 437 | 438 |     |
|     |     | 442 |     |     |     | 446 | 447 |     |     |     |
| 451 | 452 |     |     | 455 | 456 | 457 |     |     |     |     |
|     |     |     |     |     |     |     |     |     | 471 |     |
|     |     |     |     |     |     |     |     | 481 |     |     |
|     |     |     |     |     |     |     |     |     |     |     |
|     |     |     |     |     |     |     |     |     |     |     |
|     |     |     |     |     |     | 512 | 513 |     |     | 516 |
| 517 | 518 | 519 |     | 521 | 522 |     | 524 | 525 | 526 | 527 |
| 528 | 529 | 530 |     | 532 |     | 534 |     | 536 | 537 | 538 |
| 539 | 540 | 541 |     | 543 |     |     |     | 547 |     |     |
| 550 | 551 | 552 |     | 554 | 555 | 556 | 557 | 558 | 559 | 560 |
| 561 | 562 |     | 564 |     |     |     |     |     |     |     |

## :electric_plug: UNIF boards

1. 3D-BLOCK
2. 8-IN-1
3. 10-24-C-A1
4. 12-IN-1
5. 13in1JY110
6. 42in1ResetSwitch
7. 64in1NoRepeat
8. 70in1
9. 70in1B
10. 150in1A
11. 158B
12. 190in1
13. 212-HONG-KONG
14. 603-5052
15. 8157
16. 8237
17. 8237A
18. 11160
19. 22026
20. 22211
21. 43272
22. 60311C
23. 80013-B
24. 82112C
25. 411120-C
26. 810544-C-A1
27. 820561C
28. 830118C
29. 830134C
30. 830425C-4391T
31. 830752C
32. 831128C
33. 891227
34. 900218
35. A60AS
36. A65AS
37. AC08
38. AMROM
39. ANROM
40. AOROM
41. AX5705
42. AX-40G
43. BB
44. BJ-56
45. BOY
46. BS-5
47. BS-400R
48. BS-4040R
49. CC-21
50. CHINA_ER_SAN2
51. CITYFIGHT
52. CNROM
53. COOLBOY
54. COOLGIRL
55. CTC-09
56. CTC-12IN1
57. D1038
58. DANCE
59. DANCE2000
60. DRAGONFIGHTER
61. DREAMTECH01
62. DRIPGAME
63. EDU2000
64. EH8813A
65. F-15
66. FARID_SLROM_8-IN-1
67. FARID_UNROM_8-IN-1
68. FC-28-5027
69. FK23C
70. FK23CA
71. FS304
72. G-146
73. Ghostbusters63in1
74. GKCXIN1
75. GN-26
76. GS-2004
77. GS-2013
78. H2288
79. HP898F
80. HP2018-A
81. HPXX
82. JC-016-2
83. K-3006
84. K-3010
85. K-3033
86. K-3036
87. K-3046
88. K-3071
89. K-3088
90. KOF97
91. KONAMI-QTAI
92. KS106C
93. KS7012
94. KS7013B
95. KS7016
96. KS7017
97. KS7021A
98. KS7030
99. KS7031
100. KS7032
101. KS7037
102. KS7057
103. L6IN1
104. LH09
105. LH10
106. LH32
107. LH51
108. M2C52A
109. MALISB
110. MARIO1-MALEE2
111. MINDKIDS
112. N49C-300
113. N625092
114. NEWSTAR-GRM070-8IN1
115. NovelDiamond9999999in1
116. NROM
117. NROM-128
118. NROM-256
119. NTBROM
120. NTD-03
121. OneBus
122. RESET-TXROM
123. RESETNROM-XIN1
124. RT-01
125. S-2009
126. SA005-A
127. SA-0036
128. SA-0037
129. SA-016-1M
130. SA-9602B
131. SA-72007
132. SA-72008
133. SA-NROM
134. Sachen-74LS374N
135. Sachen-8259A
136. Sachen-8259B
137. Sachen-8259C
138. Sachen-8259D
139. SB-5013
140. SC-127
141. SHERO
142. SL1632
143. SLROM
144. SMB2J
145. STREETFIGTER-GAME4IN1
146. Super24in1SC03
147. SuperHIK8in1
148. Supervision16in1
149. T3H53
150. T4A54A
151. T-230
152. T-262
153. TBROM
154. TC-U01-1.5M
155. TEK90
156. TF1201
157. TFROM
158. TH2131-1
159. TJ-03
160. TKROM
161. TLROM
162. Transformer
163. UNROM
164. UOROM
165. VRC7
166. WAIXING-FS005
167. WAIXING-FW01
168. WS
169. YOKO

## :information_source: How to Compile

- :computer: [Windows](#computer-windows)

## CMake Options

| CMake Option          | Description                                       | Default |
|-----------------------|---------------------------------------------------| ------- |
| ENABLE_RELEASE        | Build release version                             | ON      |
| ENABLE_GIT_INFO       | Include the Git revision in the version string    | OFF     |
| DISABLE_PORTABLE_MODE | Disable portable mode handling                    | OFF     |
| ENABLE_TESTS          | Build the unit test harness in `tests/`           | OFF     |

FFmpeg, the fullscreen resolution/frequency support and the OpenGL renderer are
not optional any more, so they have no CMake option: Qt6 and the FFmpeg
libraries are located with `find_package`/`pkg-config` and the build fails with a
clear message if they are missing. Qt5 is not supported.

Release builds (`ENABLE_RELEASE=ON`) are compiled with aggressive optimisation flags:

| Flag                                        | Purpose                                                    |
|---------------------------------------------|------------------------------------------------------------|
| `-O3`                                       | maximum optimisation level                                  |
| `-pipe`                                     | use pipes instead of temporary files                        |
| `-flto=auto`                                | link time optimisation                                      |
| `-fdata-sections -ffunction-sections`       | one section per symbol                                      |
| `-Wl,--gc-sections`                         | let the linker drop unreferenced code and data              |
| `-Wl,--strip-all`                           | strip every symbol from the executable                      |

`-march=native` is deliberately *not* used, so the release binaries stay portable.
`-ffast-math` is not part of this set either: it changes the floating point
semantics the audio resampling path depends on, so `src/CMakeLists.txt` re-stamps
the vendored subprojects with this flag set to make sure none of them can
introduce it behind our back.

## :computer: Windows

<details>
<summary>Expand</summary>

#### Dependencies

- [MSYS2](https://www.msys2.org), **CLANG64** on x86_64 and **CLANGARM64** on
  ARM64. Those two are the supported toolchains; UCRT64 and MINGW64 are rejected
  by the build script.
- [Qt6](https://www.qt.io) with OpenGL support (`qt6-base`, `qt6-svg`; modules
  qtcore, qtgui, qtwidgets, qtnetwork, qtsvg, qtopenglwidgets, plus `qt6-tools`
  for `windeployqt`)
- [FFmpeg](https://ffmpeg.org) libraries >= 4.0 (libavcodec, libavformat,
  libavutil, libswresample and libswscale) - see [notes](#movie_camera-ffmpeg)
- [ntldd](https://github.com/LRN/ntldd) for the packaging step
- [CMake >= 3.15](https://cmake.org) and [Ninja](https://ninja-build.org)

#### One click build

Install MSYS2, open the **CLANG64** (or **CLANGARM64**) shell, and run

```bash
git clone https://github.com/punesemu/puNES
cd puNES
bash build.sh -j2
```

`build.sh` takes care of everything: it points pacman at a mirror that is
reachable, installs/updates the toolchain and the libraries, configures and
compiles, runs the unit tests and packages the result in `../products/puNES`
(the directory is wiped first, so nothing stale can survive into a package).
Nothing is downloaded from a hardcoded URL and no package name is hardcoded:
the repository comes from the platform and the package prefix is read off that
repository's own index.

Every stage has its own switch. The three a normal build wants are on by
default, the tests are not:

| Stage    | on by default | turn it off with | force it on with |
|----------|---------------|------------------|------------------|
| mirrors  | no            | `--no-mirrors`   | `--mirrors`      |
| download | yes           | `--no-download`  | `--download`     |
| build    | yes           | `--no-build`     | `--build`        |
| package  | yes           | `--no-package`   | `--package`      |
| tests    | no            | `--no-tests`     | `--tests`        |

Other options:

| Option         | Description                                                              |
|----------------|--------------------------------------------------------------------------|
| `--mirrors`    | rewrite `/etc/pacman.d/mirrorlist.{mingw,msys}` to use China mirrors      |
| `--tests`      | run the shader regression and the copy-out launch test                    |
| `--shaders`    | only the shader regression                                                |
| `--copyout`    | only the copy-out launch test                                             |
| `--out=DIR`    | package destination (default `../products/puNES`)                         |
| `-j N`         | parallel build jobs (default: cpu count; 8 GB without a page file wants `-j2`) |
| `-h`           | show the same summary from the script itself                              |

The unit tests are not behind `--tests`: they are a host binary that runs in
seconds and they are the only guard on the ROM CRC path, so they run with every
`--build`.

#### How the package is assembled

Nothing is copied by hand and no DLL list is maintained anywhere, because a
hand written list is wrong the moment a dependency changes.

1. `windeployqt` stages Qt, including the plugins that no import table mentions.
2. `ntldd.exe` is copied into the output directory and every measurement is
   taken from there, with the working directory inside it and a `PATH` holding
   only `C:\Windows\System32` and `C:\Windows`. The Windows loader searches the
   host executable's own directory first, so this is what makes ntldd answer
   "what does this package need" instead of "what can this MSYS2 install find".
   MSYS2's own `ldd` cannot be used: it goes one level deep and its `-r` is
   unimplemented.
3. `ntldd -R` reports the libraries it cannot find. Those are exactly the ones
   Windows does not supply. Each one that exists in the toolchain is copied in
   from there, and the round repeats until a round stages nothing.
4. The finished package is measured again, file by file, and the build fails if
   anything the toolchain provides is missing.

There are no name based special cases: the rule is "is it in the toolchain",
which is why `api-ms-*`/`ext-ms-*` and the Windows-only libraries need no
mention anywhere.

#### Windows Debug version

Replace the `build.sh` invocation with a manual configure:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_RELEASE=OFF
cmake --build build -j2
```

The executable is `build/src/punes.exe`, with a console window attached while
`ENABLE_RELEASE=OFF`.

</details>

---

#### :movie_camera: FFmpeg

FFmpeg is the recording backend and it is required: there is no longer a way to
build puNES without it.

Supported audio recording formats:

- WAV Audio
- MP3 Audio ([lame](https://xiph.org/vorbis/)) (\*)
- AAC Audio
- Flac Audio
- Ogg Audio ([vorbis](https://xiph.org/vorbis/)) (\*)
- Opus Audio ([libopus](https://www.opus-codec.org)) (\*)

Supported video recording formats:

- MPEG 1 Video
- MPEG 2 Video
- MPEG 4 Video
- MPEG H264 Video ([libx264](https://www.videolan.org/developers/x264.html)) (\*)
- High Efficiency Video Codec ([libx265](https://www.videolan.org/developers/x265.html)) (\*)
- WebM Video ([libvpx](https://www.webmproject.org/code)) (\*)
- Windows Media Video
- AVI FF Video
- AVI Video

(\*) if compiled in FFmpeg.
