CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -g

TARGET = campus_guard
SRCS = AccessControlSystem.h \
       Commands.cpp \
       CommsCentre.cpp \
       Coordinator.cpp \
       EmergencyOpsFacade.cpp \
       EmergencyStrategy.cpp \
       Incident.cpp \
       IncidentObserver.cpp \
       LegacyAccessAdapter.cpp \
       LegacyAccessPanel.cpp \
       OperatorConsole.cpp \
       ResponseUnits.cpp \
       main.cpp

OBJS = $(filter %.o,$(SRCS:.cpp=.o))

.PHONY: all run valgrind clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
