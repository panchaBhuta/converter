
#define MACH_PRE_MACOS14_GNU        0   // also default when OS not macos (i.e ubuntu or windows)
#define MACH_PRE_MACOS14_CLANG      1
#define MACH_POST_MACOS14_ARM_GNU   2
#define MACH_POST_MACOS14_ARM_CLANG 3

#define UBUNTU_X86_64  0
#define UBUNTU_ARM64   1

// https://sourceforge.net/p/predef/wiki/Architectures/
// https://sourceforge.net/p/predef/wiki/OperatingSystems/
// https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
  const unsigned indexOS = 2;
  #define  TEMPLATE_UID  103
  #define MACH_MACOS_ARRAY_IDX     MACH_PRE_MACOS14_GNU
  #define UBUNTU_ARRAY_IDX UBUNTU_X86_64
#elif defined(__APPLE__) && defined(__MACH__)
  const unsigned indexOS = 1;
  #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) &&  \
      __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 140000
    //  POST ARM64 era (macos-14 and above)
    #if defined(__aarch64__) || defined(__arm__)
      //////  github :  macos-14
      #if defined(__clang_major__)
        // when compiler is AppleClang.
        #define  TEMPLATE_UID  3
        #define MACH_MACOS_ARRAY_IDX     MACH_POST_MACOS14_ARM_CLANG
      #else
        // when compiler is GNU.
        // macOS does not support 'std::from_chars()' and 'std::to_chars()'.
        // The fall back functions induces variations in results when compared to other OS's.
        #define  TEMPLATE_UID  103
        #define MACH_MACOS_ARRAY_IDX     MACH_POST_MACOS14_ARM_GNU
      #endif
    #else
      //////  github :  macos-14-large   :::: NOT supported on github
      #if defined(__clang_major__)
        // when compiler is AppleClang.
        #define  TEMPLATE_UID  3
        #define MACH_MACOS_ARRAY_IDX     MACH_PRE_MACOS14_CLANG
      #else
        // when compiler is GNU.
        // macOS does not support 'std::from_chars()' and 'std::to_chars()'.
        // The fall back functions induces variations in results when compared to other OS's.
        #define  TEMPLATE_UID  103
        //const unsigned indexOS = 0;   ?????
        #define MACH_MACOS_ARRAY_IDX     MACH_PRE_MACOS14_GNU
      #endif
    #endif
  #else
    //  PRE ARM64 era (macos-13  and below)
    #if defined(__clang_major__)
      // when compiler is AppleClang.
      #define  TEMPLATE_UID  3
      #define MACH_MACOS_ARRAY_IDX     MACH_PRE_MACOS14_CLANG
    #else
      // when compiler is GNU.
      // macOS does not support 'std::from_chars()' and 'std::to_chars()'.
      // The fall back functions induces variations in results when compared to other OS's.
      #define  TEMPLATE_UID  103
      //const unsigned indexOS = 0;
      #define MACH_MACOS_ARRAY_IDX     MACH_PRE_MACOS14_GNU
    #endif
  #endif
  #define UBUNTU_ARRAY_IDX UBUNTU_X86_64
#elif defined(__linux__) // ubuntu
  const unsigned indexOS = 0;
  #define  TEMPLATE_UID  103
  #define MACH_MACOS_ARRAY_IDX     MACH_PRE_MACOS14_GNU
  #if defined(__aarch64__)
    // ARM64
    #define UBUNTU_ARRAY_IDX UBUNTU_ARM64
  #else
    #define UBUNTU_ARRAY_IDX UBUNTU_X86_64
  #endif
#else // other OS's
  const unsigned indexOS = 0;
  #define  TEMPLATE_UID  103
  #define MACH_MACOS_ARRAY_IDX     MACH_PRE_MACOS14_GNU
  #define UBUNTU_ARRAY_IDX UBUNTU_X86_64
#endif
