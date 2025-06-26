# Install script for directory: G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/esp_brookesia_demo")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "G:/ESPIDF/TOOL/tools/riscv32-esp-elf/esp-14.2.0_20241119/riscv32-esp-elf/bin/riscv32-esp-elf-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/block_cipher.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/build_info.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/compat-2.x.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_crypto.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_from_psa.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_from_legacy.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_superset_legacy.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_ssl.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_x509.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/lms.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs7.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/private_access.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha3.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/build_info.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_auto_enabled.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_dependencies.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_key_pair_types.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_synonyms.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_key_derivation.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_key_derivation.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_legacy.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "G:/ESPIDF/CODE/v5.4.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

