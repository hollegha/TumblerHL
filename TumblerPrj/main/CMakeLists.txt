
idf_component_register( SRCS "RpmTest2.cpp"
                    INCLUDE_DIRS "."
                    "..//inc" )

add_prebuilt_library( prebuilt "libRtEnvHL.a"
                      PRIV_REQUIRES driver esp_timer esp_adc nvs_flash )

target_link_libraries(${COMPONENT_LIB} PRIVATE prebuilt)

