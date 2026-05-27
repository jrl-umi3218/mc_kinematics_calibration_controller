{
  description = "Flake for mc_kinematics_calibration_controller";

  inputs = {
    mc-rtc-nix.url = "github:mc-rtc/nixpkgs";
    flake-parts.follows = "mc-rtc-nix/flake-parts";
    systems.follows = "mc-rtc-nix/systems";
    mc-panda-lirmm.url = "github:jrl-umi3218/mc_panda_lirmm/pull/16/head";
    mc-panda.url = "github:jrl-umi3218/mc_panda/pull/17/head";
    mc-franka.url = "github:jrl-umi3218/mc_franka/pull/12/head";
    mc-franka.flake = false;
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } (
      { ... }:
      {
        systems = import inputs.systems;
        imports = [
          # available flakeModules modules are:
          # - flakeModule : default module, with overlays for all public repositories in mc-rtc ecosystem
          # - flakeModules.default : same as flakeModule
          # - flakeModules.ccache : same as default but built with ccache
          # - flakeModules.private : private module, with overlays for all public and private repositories in mc-rtc ecosystem.
          #   Please note that some repositories and the private cache require permission
          #   If you have sufficient access, this is provided with through your SSH key / binary cache token
          # - flakeModules.private-ccache : same as private, but with ccache support
          inputs.mc-rtc-nix.flakeModules.default
          {
            flakoboros = {
              packages = {
                mc-kinematics-calibration-controller =
                  {
                    stdenv,
                    lib,
                    cmake,
                    mc-rtc,
                    ...
                  }:
                  stdenv.mkDerivation {
                    name = "mc-kinematics-calibration-controller";
                    src = lib.cleanSource ./.;
                    nativeBuildInputs = [ cmake ];
                    propagatedBuildInputs = [
                      mc-rtc
                    ];

                    cmakeFlags = [ (lib.cmakeBool "MC_RTC_HONOR_INSTALL_PREFIX" true) ];

                    meta = with lib; {
                      description = "";
                      homepage = "https://github.com/arntanguy/mc-kinematics-calibration-controller";
                      license = licenses.bsd2;
                      platforms = platforms.all;
                    };
                  };
              };

              overrideAttrs.mc-franka = {
                src = inputs.mc-franka;
              };

              overrideAttrs.mc-panda = {
                src = inputs.mc-panda;
              };

              overrideAttrs.mc-panda-lirmm = {
                src = inputs.mc-panda-lirmm;
              };

              # Define a custom superbuild configuration
              # This will make all
              overrides.mc-rtc-superbuild =
                { pkgs-prev, pkgs-final, ... }:
                let
                  cfg-prev = pkgs-prev.mc-rtc-superbuild.superbuildArgs;
                in
                {
                  superbuildArgs = cfg-prev // {
                    pname = "mc-rtc-superbuild-override";
                    # # for example, override any runtime dependency (robots, controllers, etc)
                    # # extend robots
                    robots = with pkgs-final; [
                      mc-panda
                      mc-panda-lirmm
                    ];
                    # # override controllers
                    controllers = [ pkgs-final.mc-kinematics-calibration-controller ];
                    # configs = [ "${pkgs-final.polytopeController}/lib/mc_controller/etc/mc_rtc.yaml" ];
                    # plugins = [ pkgs-final.mc-force-shoe-plugin ];
                    # observers = [ pkgs-final.mc-state-observation ];
                    apps = with pkgs-final; [
                      mc-franka
                      mc-rtc-ticker
                      mc-rtc-magnum
                    ];
                  };
                };

              # # Override all dependencies
              # # They are locked in flake.lock to the latest commit available at the time
              # # To update to all inputs' latest commit, use
              # # nix flake update
              # overrideAttrs.your-repository = {
              #   src = inputs.your-repository;
              # };
            };
          }
        ];
        perSystem =
          { pkgs, ... }:
          {
            packages.default = pkgs.mc-kinematics-calibration-controller;
            # define a default devShell called with the superbuild configuration above
            # you can also override attributes to add additional shell functionality
            devShells.default =
              (pkgs.callPackage "${inputs.mc-rtc-nix}/shell.nix" {
                inherit (pkgs) mc-rtc-superbuild;
              }).overrideAttrs
                (old: {
                  shellHook = ''
                    ${old.shellHook or ""}

                    echo "Welcome to the ${pkgs.mc-rtc-superbuild.pname} devShell with the overridden mc-rtc-superbuild configuration!"
                  '';
                });
          };
      }
    );
}
