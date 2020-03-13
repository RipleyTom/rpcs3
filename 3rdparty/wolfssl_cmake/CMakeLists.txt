# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

# Add preprocessor defines to CFLAGS
add_definitions(-DHAVE_FFDHE_2048 -DTFM_TIMING_RESISTANT -DECC_TIMING_RESISTANT 
                -DWC_RSA_BLINDING -DHAVE_AESGCM -DWOLFSSL_SHA512 -DWOLFSSL_SHA384 -DNO_DSA -DHAVE_ECC -DTFM_ECC256 -DECC_SHAMIR 
                -DNO_RC4 -DNO_HC128 -DNO_RABBIT -DWOLFSSL_SHA224 -DWOLFSSL_SHA3 -DWOLFSSL_SHAKE256 -DHAVE_POLY1305 
                -DHAVE_ONE_TIME_AUTH -DHAVE_CHACHA -DHAVE_HASHDRBG -DHAVE_TLS_EXTENSIONS -DHAVE_SNI -DHAVE_TLS_EXTENSIONS -DHAVE_SUPPORTED_CURVES 
                -DHAVE_EXTENDED_MASTER -DNO_RC4 -DHAVE_ENCRYPT_THEN_MAC -DNO_PSK -DNO_MD4 -DNO_PWDBASED
                -DWC_NO_ASYNC_THREADING -DNO_DES3
                )

set(wolfssl_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../wolfssl)

# Add header directories to include paths
include_directories(${wolfssl_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Add wolfSSL library source files, to be compiled as STATIC library
add_library(wolfssl-3-static STATIC
            ${wolfssl_DIR}/wolfcrypt/src/aes.c
            ${wolfssl_DIR}/wolfcrypt/src/arc4.c
            ${wolfssl_DIR}/wolfcrypt/src/asm.c
            ${wolfssl_DIR}/wolfcrypt/src/asn.c
            ${wolfssl_DIR}/wolfcrypt/src/blake2b.c
            ${wolfssl_DIR}/wolfcrypt/src/camellia.c
            ${wolfssl_DIR}/wolfcrypt/src/chacha.c
            ${wolfssl_DIR}/wolfcrypt/src/chacha20_poly1305.c
            ${wolfssl_DIR}/wolfcrypt/src/cmac.c
            ${wolfssl_DIR}/wolfcrypt/src/coding.c
            ${wolfssl_DIR}/wolfcrypt/src/compress.c
            ${wolfssl_DIR}/wolfcrypt/src/cpuid.c
            ${wolfssl_DIR}/wolfcrypt/src/curve25519.c
            ${wolfssl_DIR}/wolfcrypt/src/des3.c
            ${wolfssl_DIR}/wolfcrypt/src/dh.c
            ${wolfssl_DIR}/wolfcrypt/src/dsa.c
            ${wolfssl_DIR}/wolfcrypt/src/ecc.c
            ${wolfssl_DIR}/wolfcrypt/src/ecc_fp.c
            ${wolfssl_DIR}/wolfcrypt/src/ed25519.c
            ${wolfssl_DIR}/wolfcrypt/src/error.c
            ${wolfssl_DIR}/wolfcrypt/src/fe_low_mem.c
            ${wolfssl_DIR}/wolfcrypt/src/fe_operations.c
            ${wolfssl_DIR}/wolfcrypt/src/ge_low_mem.c
            ${wolfssl_DIR}/wolfcrypt/src/ge_operations.c
            ${wolfssl_DIR}/wolfcrypt/src/hash.c
            ${wolfssl_DIR}/wolfcrypt/src/hc128.c
            ${wolfssl_DIR}/wolfcrypt/src/hmac.c
            ${wolfssl_DIR}/wolfcrypt/src/idea.c
            ${wolfssl_DIR}/wolfcrypt/src/integer.c
            ${wolfssl_DIR}/wolfcrypt/src/logging.c
            ${wolfssl_DIR}/wolfcrypt/src/md2.c
            ${wolfssl_DIR}/wolfcrypt/src/md4.c
            ${wolfssl_DIR}/wolfcrypt/src/md5.c
            ${wolfssl_DIR}/wolfcrypt/src/memory.c
            ${wolfssl_DIR}/wolfcrypt/src/pkcs12.c
            ${wolfssl_DIR}/wolfcrypt/src/pkcs7.c
            ${wolfssl_DIR}/wolfcrypt/src/poly1305.c
            ${wolfssl_DIR}/wolfcrypt/src/pwdbased.c
            ${wolfssl_DIR}/wolfcrypt/src/rabbit.c
            ${wolfssl_DIR}/wolfcrypt/src/random.c
            ${wolfssl_DIR}/wolfcrypt/src/ripemd.c
            ${wolfssl_DIR}/wolfcrypt/src/rsa.c
            ${wolfssl_DIR}/wolfcrypt/src/sha.c
            ${wolfssl_DIR}/wolfcrypt/src/sha256.c
            ${wolfssl_DIR}/wolfcrypt/src/sha3.c
            ${wolfssl_DIR}/wolfcrypt/src/sha512.c
            ${wolfssl_DIR}/wolfcrypt/src/signature.c
            ${wolfssl_DIR}/wolfcrypt/src/srp.c
            ${wolfssl_DIR}/wolfcrypt/src/tfm.c
            ${wolfssl_DIR}/wolfcrypt/src/wc_encrypt.c
            ${wolfssl_DIR}/wolfcrypt/src/wc_port.c
            ${wolfssl_DIR}/wolfcrypt/src/wolfevent.c
            ${wolfssl_DIR}/wolfcrypt/src/wolfmath.c
            ${wolfssl_DIR}/src/crl.c
            ${wolfssl_DIR}/src/internal.c
            ${wolfssl_DIR}/src/wolfio.c
            ${wolfssl_DIR}/src/keys.c
            ${wolfssl_DIR}/src/ocsp.c
            ${wolfssl_DIR}/src/sniffer.c
            ${wolfssl_DIR}/src/ssl.c
            ${wolfssl_DIR}/src/tls.c
            ${wolfssl_DIR}/src/tls13.c
           )

target_include_directories(wolfssl-3-static PUBLIC $<BUILD_INTERFACE:${wolfssl_DIR}>)

set_target_properties(wolfssl-3-static PROPERTIES
            PREFIX "lib"
            OUTPUT_NAME "wolfssl-3"
            CLEAN_DIRECT_OUTPUT 1
                     )

list(APPEND WOLFSSL_LIBTARGETS wolfssl-3-static)

install(TARGETS ${WOLFSSL_LIBTARGETS} EXPORT wolfssl-3
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION lib
       )

install(EXPORT wolfssl-3 DESTINATION lib/wolfssl-3)
