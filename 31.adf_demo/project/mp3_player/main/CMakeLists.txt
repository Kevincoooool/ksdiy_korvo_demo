set(COMPONENT_SRCS 
    "app_main.c"
    "app_wifi.c"
    "file_manager.c"

    "mp3_player.c"
    )
set(COMPONENT_ADD_INCLUDEDIRS  "." "button" )

# set(COMPONENT_EMBED_FILES
#     "www/index.html.gz"
#     # "www/adf_music.mp3"
#     )

register_component()

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)