# Compiler and flags
CXX = g++
CXXFLAGS = -I src/include -L src/lib

# Libraries
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Source files
SRCS = main.cpp Game.cpp \
       scene/MainMenuScene.cpp scene/GamePlayScene.cpp scene/GamePlayWithAIScene.cpp scene/SettingsScene.cpp \
       objects/object.cpp objects/player.cpp objects/ball.cpp objects/ground.cpp

# Output
TARGET = main

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	del $(TARGET).exe
