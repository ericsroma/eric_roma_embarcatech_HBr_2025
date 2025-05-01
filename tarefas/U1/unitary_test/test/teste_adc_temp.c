#include "unity.h"
#include <math.h> 
#include "funcao_adc_temp.h"

void setUp(void) {}
void tearDown(void) {}

void test_27_graus(void) 
{
    float target_voltage = 0.706f;
    uint16_t simulated_adc = (uint16_t)(target_voltage / (3.3f / 4096.0f));
    float result = adc_to_celsius(simulated_adc);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 27.0f, result);
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_27_graus);
    return UNITY_END();
}
