PYTHON_ENV_DIR=virtualenv
TEST_EXECUTABLE=test/test
PYTHON ?= python3

all: test

install: $(PYTHON_ENV_DIR)

clean:
	rm -rf $(PYTHON_ENV_DIR)
	rm -rf $(TEST_EXECUTABLE)

$(PYTHON_ENV_DIR):
	$(PYTHON) -mvenv $(PYTHON_ENV_DIR)
	$(PYTHON_ENV_DIR)/bin/pip install -r requirements.txt

test: $(TEST_EXECUTABLE) $(PYTHON_ENV_DIR)
	$(PYTHON_ENV_DIR)/bin/python runner.py -e $(TEST_EXECUTABLE) -m test/test.py

$(TEST_EXECUTABLE): test/test.cpp
	g++ -std=c++11 -Isrc -o $@ test/test.cpp

.PHONY: test clean install all
