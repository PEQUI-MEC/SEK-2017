set(CMAKE_C_COMPILER "arm-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "arm-linux-gnueabi-g++")

project(teste)

#diz ao compilador para usar o padrao c++11
add_compile_options("-std=c++0x")

# diz ao compilador e o linker que exite thread no codigo
add_compile_options("-pthread")
link_libraries("-pthread")

# inclui o diretorio para busca dos sources
include_directories(src)

# seta a variavel uma lista com a biblioteca padrao
set(LIBS src/ev3dev.cpp)

# adiciona a variavel todos os sources do nosso codigo
list(APPEND LIBS  src/M_arquivos.cpp)
list(APPEND LIBS  src/Sensor_cor_hsv.cpp)
list(APPEND LIBS  src/Ultrassom_nxt.cpp)
list(APPEND LIBS  src/Ultrassom.cpp)
list(APPEND LIBS  src/Controlador_robo.cpp)
list(APPEND LIBS  src/Mapeamento.cpp)
list(APPEND LIBS  src/Garra.cpp)
list(APPEND LIBS  src/Resgate.cpp)


# adiciona todos os sources e o main para serem compilados juntos
add_executable(binario  ${LIBS} src/main.cpp)
