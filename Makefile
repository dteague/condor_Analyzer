##############################################################
# Author: Andres Florez, Universidad de los Andes, Colombia. #
##############################################################

ObjSuf = o
SrcSuf = cc
ExeSuf =
DllSuf = so
OutPutOpt = -o
HeadSuf = h

ROOTCFLAGS = $(shell root-config --cflags) -O2
ROOTLIBS = $(shell root-config --libs) -O2

# Linux with egcs

CXX = g++ -std=c++11
CXXFLAGS += $(ROOTCFLAGS) -I./src/ -Wall -Werror 

LD = g++ -std=c++11
LDFLAGS += $(ROOTLIBS) -Wall -Werror -l TreePlayer -lboost_program_options

SOFLAGS = -shared
LIBS =

SRCDIR = src
OBJDIR = obj

#------------------------------------------------------------------------------
SOURCES = $(wildcard $(SRCDIR)/*.cc)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)
#------------------------------------------------------------------------------

all: Analyzer


Analyzer: $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)	

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) -o $@  -c $< $(CXXFLAGS)

%: $(OBJDIR)/%.o
	$(LD) -o $@ $< $(LIBS) $(LDFLAGS)

clean:
	@echo "Cleaning..."
	@ls $(OBJDIR)
	@rm -f $(OBJECTS)

# job: Plotter
# 	rm *root -f
# 	./Plotter config/2018_WZ_YR.config

.SUFFIXES: .$(SrcSuf) .cc .o .so
