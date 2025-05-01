add_executable(test_adc_temp
teste_adc_temp.c
    ../src/funcao_adc_temp.c
    ../lib/Unity/src/unity.c
)

target_include_directories(test_adc_temp PRIVATE
    ../src
    ../lib/Unity/src
)

target_link_libraries(test_adc_temp pico_stdlib)

pico_add_extra_outputs(test_adc_temp)
