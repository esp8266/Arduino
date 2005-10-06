CXXFLAGS = -mwindows -mno-cygwin -O2 -Wall
OBJS = launcher.o launcher-rc.o

arduino.exe: $(OBJS)
	$(LINK.cc) $(CXXFLAGS) -o $@ $(OBJS)
	cp arduino.exe ../work/

$(OBJS): Makefile

launcher-rc.o: launcher.rc
	windres -i $< -o $@

clean:
	$(RM) $(OBJS) arduino.exe
