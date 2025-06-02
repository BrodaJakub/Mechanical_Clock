#! /bin/bash

g++ -g \
  /home/jakub/Pobrane/cgv_st_04_win/main_file.cpp \
  /home/jakub/Pobrane/cgv_st_04_win/shaderprogram.cpp \
  /home/jakub/Pobrane/cgv_st_04_win/model.cpp \
  /home/jakub/Pobrane/cgv_st_04_win/torus.cpp \
  /home/jakub/Pobrane/cgv_st_04_win/cube.cpp \
  /home/jakub/Pobrane/cgv_st_04_win/objModel.cpp \
  -o /home/jakub/Pobrane/cgv_st_04_win/main_file \
  -lGL -lGLEW -lglfw
