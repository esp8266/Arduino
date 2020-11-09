# Testing Arduino ESP8266 Core

## Testing on host

Some features of this project can be tested by compiling and running the code on the PC, rather than running it on the ESP8266. Tests and testing infrastructure for such features is located in `tests/host` directory of the project.

Some hardware features, such as Flash memory and HardwareSerial, can be emulated on the PC. Others, such as network, WiFi, and other hardware (SPI, I2C, timers, etc) are not yet emulated. This limits the amount of features which can be tested on the host.

### Adding a test case

Tests are written in C++ using [Catch framework](https://github.com/catchorg/Catch2).

See .cpp files under tests/host/core/ for a few examples how to write test cases.

When adding new test files, update `TEST_CPP_FILES` variable in tests/host/Makefile to compile them.

If you want to add emulation of a certain feature, add it into tests/host/common/ directory.


### Running test cases

**NOTE!** The test-on-host environment is dependent on some submodules. Make sure to run `git submodule update --init` before running any test.

To run test cases, go to tests/host/ directory and run `make`. This will compile and run the tests. 

If all tests pass, you will see "All tests passed" message and the exit code will be 0.

Additionally, test coverage info will be generated using `gcov` tool. You can use some tool to analyze coverage information, for example `lcov`:
    
    lcov -c -d . -d ../../cores/esp8266 -o test.info
    genhtml -o html test.info

This will generate an HTML report in `html` directory. Open html/index.html in your browser to see the report.

**Note to macOS users:** you will need to install GCC using Homebrew or MacPorts. Before running `make`, set `CC`, `CXX`, and `GCOV` variables to point to GCC tools you have installed. For example, when installing gcc-5 using Homebrew:

    export CC=gcc-5
    export CXX=g++-5
    export GCOV=gcov-5

When running `lcov` (which you also need to install), specify `gcov` binary using `--gcov-tool $(which $GCOV)` (assuming you have already set `GCOV` environment variable).

## Testing on device

Most features and libraries of this project can not be tested on host. Therefore testing on an ESP8266 device is required. Such tests and the test infrastructure are located in tests/device directory of this project.

### Test cases

Tests are written in the form of Arduino sketches, and placed into tests/device/test_xxx directories. These tests are compiled using Arduino IDE, so test file name should match the name of the directory it is located in (e.g. test_foobar/test_foobar.ino). Tests use a very simple BSTest library, which handles test registration and provides `TEST_CASE`, `CHECK`, `REQUIRE`, and `FAIL` macros, similar to [Catch](https://github.com/catchorg/Catch2).

*Note: we should migrate to Catch framework with a custom runner.*

Here is a simple test case written with BSTest:

```c++
#include <BSTest.h>
#include <test_config.h>

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}


TEST_CASE("this test runs successfully", "[bs]")
{
    CHECK(1 + 1 == 2);
    REQUIRE(2 * 2 == 4);
}
```

BSTest is a header-only library, so necessary static data is injected into the sketch using `BS_ENV_DECLARE();` macro.

`BS_RUN(Serial)` passes control to the test runner, which uses `Serial` stream to communicate with the host. If you need to do any preparation before starting tests, for example connect to an AP, do this before calling `BS_RUN`.

`TEST_CASE` macro defines a test case. First argument is human-readable test name, second contains optional set of tags (identifiers with square brackets). Currently only one tag has special meaning: `[.]` can be used to mark the test case as ignored. Such tests will not be skipped by the test runner (see below).

### Test execution

Once `BS_RUN` is called, BSTest library starts by printing the *menu*, i.e. the list of tests defined in the sketch. For example:

```
>>>>>bs_test_menu_begin
>>>>>bs_test_item id=1 name="this test runs successfully" desc="[bs]"
>>>>>bs_test_menu_end
```

Then it waits for the test index to be sent by the host, followed by newline.

Once the line number is received, the test is executed, and feedback is printed:
```
>>>>>bs_test_start file="arduino-esp8266/tests/device/test_tests/test_tests.ino" line=13 name="this test runs successfully" desc="[bs]"
>>>>>bs_test_end line=0 result=1 checks=2 failed_checks=0
```

Or, in case the test fails:
```
>>>>>bs_test_start file="arduino-esp8266/tests/device/test_tests/test_tests.ino" line=19 name="another test which fails" desc="[bs][fail]"
>>>>>bs_test_check_failure line=22
>>>>>bs_test_check_failure line=24
>>>>>bs_test_end line=0 result=0 checks=4 failed_checks=2
```

BSTest library also contains a Python script which can "talk" to the ESP8266 board and run the tests, tests/device/libraries/BSTest/runner.py. Normally it is not necessary to use this script directly, as the top level Makefile in tests/device/ directory can call it automatically (see below).

### Test configuration

Some tests need to connect to WiFi AP or to the PC running the tests. In the test code, this configuration is read from environment variables (the ones set using C `getenv`/`setenv` functions). There are two ways environment variables can be set.

- Environment variables which apply to all or most of the tests can be defined in `tests/device/test_env.cfg` file. This file is not present in Git by default. Make a copy of `tests/device/test_env.cfg.template` and change the values to suit your environment.

- Environment variables which apply to a specific test can be set dynamically by the `setup` host side helper (see section below). This is done using `setenv` function defined in `mock_decorators`.

Environment variables can also be used to pass some information from the test code to the host side helper. To do that, test code can set an environment variable using `setenv` C function. Then the `teardown` host side helper can obtain the value of that variable using `request_env` function defined in `mock_decorators`.

A SPIFFS filesystem may be generated on the host and uploade before a test by including a file called `make_spiffs.py` in the individual test directory.

### Building and running the tests

Makefile in tests/device/ directory handles compiling, uploading, and executing test cases.

Here are some of the supported targets:

- `virtualenv`: prepares Python virtual environment inside tests/device/libraries/BSTest/virtualenv/. This has to be run once on each computer where tests are to be run. This target will use `pip` to install several Python libraries required by the test runner (see tests/device/libraries/BSTest/requirements.txt).

- `test_xxx/test_xxx.ino`: compiles, uploads, and runs the tests defined in `test_xxx/test_xxx.ino` sketch. Some extra options are available, these can be passed as additional arguments to `make`:
    - `NO_BUILD=1`: don't compile the test.
    - `NO_UPLOAD=1`: don't upload the test.
    - `NO_RUN=1`: don't run the test.
    - `V=1`: enable verbose output from compilation, upload, and test runner.

    For example, `make test_newlib/test_newlib.ino V=1` will compile, upload, and run all tests defined in `test_newlib/test_newlib.ino`.

    For each test sketch, test results are stored in `tests/device/.build/test_xxx.ino/test_result.xml`. This file is an xUnit XML file, and can be read by a variety of tools, such as Jenkins.

- `test_report`: Generate HTML test report from xUnit XML files produced by test runs.

- `all` (or just `make` without a target): Run tests from all the .ino files, and generate HTML test report.

### Host-side helpers

Some tests running on the device need a matching part running on the host. For example, HTTP client test might need a web server running on the host to connect to. TCP server test might need to be connected to by TCP client running on the host. To support such use cases, for each test file, an optional Python test file can be provided. This Python file defines setup and teardown functions which have to be run before and after the test is run on the device. `setup` and `teardown` decorators bind setup/teardown functions to the test with specified name:

```python
from mock_decorators import setup, teardown, setenv, request_env

@setup('WiFiClient test')
def setup_wificlient_test(e):
    # create a TCP server
    # pass environment variable to the test
    setenv(e, 'SERVER_PORT', '10000')
    setenv(e, 'SERVER_IP', repr(server_ip))

@teardown('WiFiClient test')
def teardown_wificlient_test(e):
    # delete TCP server
    # request environment variable from the test, compare to the expected value
    read_bytes = request_env(e, 'READ_BYTES')
    assert(read_bytes == '4096')
```

Corresponding test code might look like this:

```c++

TEST_CASE("WiFiClient test", "[wificlient]")
{
    const char* server_ip = getenv("SERVER_IP");
    int server_port = (int) strtol(getenv("SERVER_PORT"), NULL, 0);

    WiFiClient client;
    REQUIRE(client.connect(server_ip, server_port));

    // read data from server
    // ...

    // Save the result back so that host side helper can read it
    setenv("READ_BYTES", String(read_bytes).c_str(), 1);
}
```


