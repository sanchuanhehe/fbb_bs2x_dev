#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(cipher)
add_subdirectory_if_exist(hash)
add_subdirectory_if_exist(km)
add_subdirectory_if_exist(pke)
add_subdirectory_if_exist(trng)
