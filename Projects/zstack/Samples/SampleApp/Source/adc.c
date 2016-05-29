#include "adc.h"

void AdcInit()
{
   HalAdcSetReference(HAL_ADC_REF_AVDD);
   HalAdcInit ();
}

int ReadAd()
{
  int v = 0;

  v = HalAdcRead ( HAL_ADC_CHN_AIN5, HAL_ADC_RESOLUTION_8 );

  return v;
}