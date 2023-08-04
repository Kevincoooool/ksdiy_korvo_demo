idf_component_register(SRCS "bsp_adc.c"
                    INCLUDE_DIRS "." 
                    REQUIRES esp_adc_cal driver)
