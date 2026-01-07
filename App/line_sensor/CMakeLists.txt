
idf_component_register( SRCS "FullLineSensTest.cpp" 
                             "LineController.cpp"
                             "LineSensor.cpp"
                             
                             
  INCLUDE_DIRS "." 
               "../components/dsp" 
               "../components/RtEnvHL/commu"
               "../components/RtEnvHL/core"
               "../components/RtEnvHL/led_strip"
)

