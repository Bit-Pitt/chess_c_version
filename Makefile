CXX = g++
CXXFLAGS = -std=c++17  #-Wall -Wextra

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRC = main.cpp \
      chessboard/scacchiera.cpp \
      pieces/Pezzo.cpp \
      pieces/Pedone.cpp \
      pieces/Torre.cpp \
      pieces/Re.cpp \
      pieces/Alfiere.cpp \
      pieces/Cavallo.cpp \
      pieces/Regina.cpp \
      utils/graphicUtils.cpp \
      utils/logicUtils.cpp \
      gameLogic/GameClass.cpp \
      gameNoGui/game.cpp

TARGET = scacchi

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)