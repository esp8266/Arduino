CXXFLAGS = -mwindows -mno-cygwin -O2 -Wall
#POBJS = launcher.o launcher-rc.o
# don't want to include the p5 icons for the exported feller
AOBJS = launcher.o 

#processing.exe: $(POBJS)
#	$(LINK.cc) $(CXXFLAGS) -o $@ $(POBJS)
#	cp processing.exe ../work/

application.exe: $(AOBJS)
	$(LINK.cc) $(CXXFLAGS) -DEXPORT -o $@ $(AOBJS)
	cp application.exe ../work/lib/export/
	cp application.exe ../../shared/lib/export/

$(POBJS): Makefile 

#launcher-rc.o: launcher.rc
#	windres -i $< -o $@

clean:
	$(RM) $(OBJS) application.exe
#	$(RM) $(OBJS) processing.exe application.exe
