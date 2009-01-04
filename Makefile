include configuration

cellar : build/life.o build/grid.o build/sound.o build/game.o build/main.o build/texture.o build/glerror.o build/machine.o
	$(CXX) $(CFLAGS) $(LDFLAGS) $^ -o $@
	
build/%.o : src/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

