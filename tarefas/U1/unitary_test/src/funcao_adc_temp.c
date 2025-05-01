#include "funcao_adc_temp.h"

float adc_to_celsius(uint16_t adc_val) 
{
    const float conversion_factor = 3.3f / (1 << 12); // 3.3V / 4096
    float voltage = adc_val * conversion_factor;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}