{ lib
, stdenv
, callPackage
, cmake
, sdl3
, ctrSource ? ./../..
, withDebug ? true
, trustCompiler ? false
}:

let
  isWindows = stdenv.hostPlatform.uname.system == "Windows";

  mainProgram = if isWindows then "CrashTeamRacingPC.exe" else "CrashTeamRacingPC";

  sdlTweaked = sdl3.override {
    openglSupport = true;
    # On Linux, only ALSA for now
    jackSupport = false;
    pipewireSupport = false;
    pulseaudioSupport = false;
    # unused
    dbusSupport = false;
    ibusSupport = false;
  };
in
stdenv.mkDerivation (finalAttrs: {
  pname = "CTR-native";
  version = "0.0.0-dev";

  src = ctrSource;

  nativeBuildInputs = sdlTweaked.nativeBuildInputs;
  buildInputs = sdlTweaked.buildInputs;

  # Disables incompatible hardening
  hardeningDisable = [ "format" ];

  # Config
  cmakeFlags =
    lib.optionals withDebug [ "-DCMAKE_BUILD_TYPE=Debug" ]
    ++ lib.optionals trustCompiler [ "-DCMAKE_C_COMPILER_WORKS=1" "-DCMAKE_CXX_COMPILER_WORKS=1" ];

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    cp ${mainProgram} $out/bin/

    runHook postInstall
  '';

  # Keep debug symbols
  dontStrip = withDebug;

  # Shows the proper compile date in the logs
  env.SOURCE_DATE_EPOCH = (builtins.currentTime or ctrSource.lastModified or "0");

  meta = {
    description = " A Crash Team Racing Decompilation Project ";
    homepage = "https://github.com/CTR-tools/CTR-native";
    license = lib.licenses.publicDomain;
    maintainers = with lib.maintainers; [ pedrohlc ];
    inherit mainProgram;
    platforms = [ "i686-linux" "arvm7l-linux" "i686-windows" ];
  };
})
