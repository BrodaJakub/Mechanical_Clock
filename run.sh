#! /bin/bash

g++ -g \
  /home/jakub/Pobrane/Mechanical_Clock/main_file.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/shaderprogram.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/model.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/torus.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/cube.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/gearwheel.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/granny_clock.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/granny_clock_face.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/granny_clock_hand.cpp \
  /home/jakub/Pobrane/Mechanical_Clock/pendulum.cpp \
  -o /home/jakub/Pobrane/Mechanical_Clock/main_file \
  -lGL -lGLEW -lglfw
./main_file
