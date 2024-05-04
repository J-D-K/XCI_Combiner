CC			:= gcc
CXX			:= g++
INCLUDE_DIRS := include
SOURCE_DIRS	:= source
CFLAGS		:= -I$(INCLUDE_DIRS) -Wall -O2
CXXFLAGS	:=	$(CFLAGS) -std=c++17
LIBS		:= 
CFILES		:= $(foreach dir,$(SOURCE_DIRS), $(wildcard $(dir)/*.c))
CPPFILES	:= $(foreach dir,$(SOURCE_DIRS), $(wildcard $(dir)/*.cpp))
OFILES		:= $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
TARGET		:= "xci_combiner"

$(TARGET) : $(OFILES)
	$(CXX) -o $@ $(OFILES) $(LIBS)

%.o: $(SOURCE_DIRS)/%.c 
	$(CC) $(CFLAGS) $< -c -o $@

%.o: %.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	@rm -rf $(OFILES) $(TARGET)

echo:
	@echo "CFILES: " $(CFILES) 
	@echo "CPPFILES: " $(CPPFILES)
	@echo "OFILES: " $(OFILES)