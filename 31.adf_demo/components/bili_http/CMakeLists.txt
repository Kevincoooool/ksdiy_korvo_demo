idf_component_register(SRCS "bili_http.c"
                    INCLUDE_DIRS "." "../read_nvs"
                    REQUIRES json nvs_flash spiffs esp_http_client)
