cmake_minimum_required(VERSION 3.13)

# Importa pico_sdk
include(pico_sdk_import.cmake)

# Define o projeto
project(test_adc_temp C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Inicializa o SDK
pico_sdk_init()

# Inclui os testes
add_subdirectory(test)
