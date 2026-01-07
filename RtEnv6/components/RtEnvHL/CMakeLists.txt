
idf_component_register( SRCS "./core/RtEnvHL.cpp"
	                     "./core/TimerHL.c" 
                             "./core/EspMotor.cpp"
	                     "./commu/SvProtocol3.cpp"
                             "./commu/TmbMenues.cpp"
                             "./core/MPU_Esp.cpp"
                             "./core/ImuAlgo.cpp"
                             "./core/NodeLock.cpp"
			     "./commu/softap_hl.c"
                             "./commu/station_hl2.c"
                             "./led_strip/led_strip_api.c"
                             "./led_strip/led_strip_rmt_dev.c"
                             "./led_strip/led_strip_rmt_encoder.c"
                             "./led_strip/LedStripHL.cpp"
                             
     INCLUDE_DIRS "../dsp" "./core" "./commu" "./led_strip"
                        
     PRIV_REQUIRES driver esp_timer esp_adc nvs_flash esp_wifi )

 