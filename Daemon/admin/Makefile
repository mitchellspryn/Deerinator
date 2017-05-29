#######
#### University of Alabama 2014 Lunabot Autonomy Makefile
#######
##
## Uses CMake in an out-of-source build.
## All make targets must be run while in this current admin/ directory.
##
#######

#FUCK YOU, I'M CHANGING IT. BITCH!
#WHAT NOW?!?!?!?!??!?!??!??!??!?!??!?

# Project name
PROJECTNAME	= lunabotautonomyd2014
# Current version
VERSION		= 0.5.1


TARNAME = $(PROJECTNAME)-$(VERSION)



# Directory to store the temporary build files and executables.
BINDIR		= bin
BUILDDIR	= build
ADMINDIR	= admin
CMAKEFILE	= CMakeLists.txt
CMAKEMODULESDIR	= cmake



# Build the project
.PHONY: all
all: cmakemake



# Execute the CMake generated Makefile
.PHONY: make
cmakemake: cmake
	cd ../$(BUILDDIR); make



# Generate the CMake
.PHONY: cmake
cmake:
	mkdir -p ../$(BUILDDIR)
	mkdir -p ../$(BUILDDIR)/$(ADMINDIR)
	cp -f ../$(CMAKEFILE) ../$(BUILDDIR)/
	cp -Rf $(CMAKEMODULESDIR) ../$(BUILDDIR)/$(ADMINDIR)/
	cd ../$(BUILDDIR); cmake ..; ln -fs $(BUILDDIR)/$(BINDIR) $(BINDIR)


# Install the autonomy daemon on the system
.PHONY: install
install: all
	echo "install command not yet implemented"



# Uninstall the autonomy daemon from the system
.PHONY: uninstall
uninstall:
	echo "uninstall command not yet implemented"



# Clean up files from build and temporary backup files
.PHONY: dist-clean clean
dist-clean clean:
	rm -fr ../$(BUILDDIR)
	find ../ -name "*~" -print0 | xargs -0 --no-run-if-empty rm



# Create a distributable archive
.PHONY: dist
dist:
	rm -f ../../$(TARNAME)
	ln -s $(shell dirname $(shell pwd)) ../../$(TARNAME)
	cd ../../; tar --exclude='*build*' --exclude='*doc*' --exclude='*Test_Files*' --exclude='*.svn' -zcvh -f $(TARNAME).tar.gz $(TARNAME)
	rm ../../$(TARNAME)



# Create documentation
.PHONY: install-html install
install-html html:
	doxygen
