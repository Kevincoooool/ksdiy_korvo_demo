if(ESP_PLATFORM)

file(GLOB SOURCES *.c)


idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS .
                       REQUIRES lvgl)
# Required for `lv_lib_png`
# target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_PNG_USE_LV_FILESYSTEM")

endif()
