idf_component_register(
    SRCS "auth_storage.c"
    INCLUDE_DIRS "."
    REQUIRES 
        nvs_flash
)