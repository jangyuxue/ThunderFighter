# Makefile for Thunder Fighter (雷霆战机)
# Compiler: g++ (MinGW-w64)
# Target: Windows native executable

CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -O2
LDFLAGS   := -lgdiplus -lgdi32 -luser32 -lkernel32 -mwindows

SRCDIR    := src
OBJDIR    := obj
TARGET    := ThunderFighter.exe

# 手动列出所有源文件（避免 **/*.cpp 兼容性问题）
SOURCES   := src/main.cpp \
             src/Game.cpp \
             src/core/Window.cpp \
             src/core/GameTimer.cpp \
             src/core/InputManager.cpp \
             src/core/Renderer.cpp \
             src/core/ResourceManager.cpp \
             src/entity/Player.cpp \
             src/entity/Bullet.cpp \
             src/entity/Enemy.cpp \
             src/entity/PowerUp.cpp \
             src/entity/Particle.cpp \
             src/system/StarField.cpp \
             src/system/LevelManager.cpp \
             src/system/WeaponSystem.cpp \
             src/system/PowerUpSystem.cpp \
             src/system/ParticleSystem.cpp \
             src/system/ScoreManager.cpp \
             src/ui/UIManager.cpp

OBJECTS   := $(patsubst src/%.cpp, obj/%.o, $(SOURCES))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful: $(TARGET)"

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)
