# Install script for directory: F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/esp-rtc")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "F:/myesp/.espressif/tools/xtensa-esp32s3-elf/esp-2021r2-patch3-8.4.0/xtensa-esp32s3-elf/bin/xtensa-esp32s3-elf-objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/aesni.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/arc4.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/blowfish.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/bn_mul.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/certs.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/cipher_internal.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/compat-1.3.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/config.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ecp_internal.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/entropy_poll.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/havege.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/md2.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/md4.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/md_internal.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/net.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/padlock.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/pk_internal.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/pkcs11.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/rsa_internal.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ssl_internal.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/mbedtls/xtea.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

