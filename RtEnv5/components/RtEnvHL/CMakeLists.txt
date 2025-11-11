idf_component_register( SRCS "RtEnvHL.cpp"
	                     "TimerHL.c" 
                             "EspMotor.cpp"
	                     "SvProtocol3.cpp"
                             "MPU_Esp.cpp"
                             "ImuAlgo.cpp"
                             "NodeLock.cpp"
			     "softap_hl.c"
                             "station_hl2.c"
                        INCLUDE_DIRS "."  
                                     "..//dsp"
                        PRIV_REQUIRES driver esp_timer esp_adc nvs_flash esp_wifi )

 