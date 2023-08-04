set(COMPONENT_SRCS 
    "app_main.c"
    "button/button.c"
    "button/bsp_button.c"
    
    )
set(COMPONENT_ADD_INCLUDEDIRS  "." "button" )


register_component()

