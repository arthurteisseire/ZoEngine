{ pkgs ? import <nixpkgs> {} }:

let
  vma = pkgs.fetchFromGitHub {
    owner = "GPUOpen-LibrariesAndSDKs";
    repo = "VulkanMemoryAllocator";
    rev = "a6bfc23";  # Commit hash
    sha256 = "1hpzjwl5bgqv9hmf1fdldihfllcbdg515f391a200klg0rnixdds";
  };

  stb = pkgs.fetchFromGitHub {
    owner = "nothings";
    repo = "stb";
    rev = "f4a71b1";
    sha256 = "11mm69313i9gbxm90lamic3xlfdx5zyhjwnhpp0aqym5b2pf6w9i";
  };
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    # Vulkan libraries
    mesa
    vulkan-loader
    vulkan-validation-layers
    vulkan-tools
    vulkan-headers
    shaderc
    vk-bootstrap
    glm

    # Clang compiler and tools
    clang_10
    cmake
    pkg-config

    # Other dependencies like GLFW for windowing
    glfw3
  ];

  VMA_INCLUDE_PATH = "${vma}/include";
  STB_INCLUDE_PATH = "${stb}/";

  # Environment variables and other shell setup
  shellHook = ''
    export VK_LAYER_PATH=${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d
    export VK_ICD_FILENAMES="/run/opengl-driver/share/vulkan/icd.d/nvidia_icd.x86_64.json"

    # Set CC and CXX for Clang
    export CC="${pkgs.clang_10}/bin/clang"
    export CXX="${pkgs.clang_10}/bin/clang++"

    # Set CXXFLAGS for C++ standard version
    export CXXFLAGS="-std=c++20"
  '';
}
