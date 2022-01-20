#ifndef BSTEST_H
#define BSTEST_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <functional>
#include <stdexcept>
#include "BSProtocol.h"

#if defined(ARDUINO)
#include "BSArduino.h"
#else
#include "BSStdio.h"
#endif

#ifndef BS_LINE_BUF_SIZE
#define BS_LINE_BUF_SIZE 80
#endif

namespace bs
{
typedef void (*test_case_func_t)();

class TestCase
{
public:
    TestCase(TestCase* prev, test_case_func_t func, const char* file, size_t line, const char* name, const char* desc) :
        m_func(func), m_file(file), m_line(line), m_name(name), m_desc(desc)
    {
        if (prev)
        {
            prev->m_next = this;
        }
    }

    void run() const
    {
        (*m_func)();
    }

    TestCase* next() const
    {
        return m_next;
    }

    const char* file() const
    {
        return m_file;
    }

    size_t line() const
    {
        return m_line;
    }

    const char* name() const
    {
        return m_name;
    }

    const char* desc() const
    {
        return (m_desc) ? m_desc : "";
    }

protected:
    TestCase*        m_next = nullptr;
    test_case_func_t m_func;
    const char*      m_file;
    size_t           m_line;
    const char*      m_name;
    const char*      m_desc;
};

struct Registry
{
    void add(test_case_func_t func, const char* file, size_t line, const char* name, const char* desc)
    {
        TestCase* tc = new TestCase(m_last, func, file, line, name, desc);
        if (!m_first)
        {
            m_first = tc;
        }
        m_last = tc;
    }
    TestCase* m_first = nullptr;
    TestCase* m_last  = nullptr;
};

struct Env
{
    std::function<void(void)>   m_check_pass;
    std::function<void(size_t)> m_check_fail;
    std::function<void(size_t)> m_fail;
    Registry                    m_registry;
};

extern Env g_env;

template <typename IO>
class Runner
{
    typedef Runner<IO> Tself;

public:
    Runner(IO& io) :
        m_io(io)
    {
        g_env.m_check_pass = std::bind(&Tself::check_pass, this);
        g_env.m_check_fail = std::bind(&Tself::check_fail, this, std::placeholders::_1);
        g_env.m_fail       = std::bind(&Tself::fail, this, std::placeholders::_1);
    }

    ~Runner()
    {
        g_env.m_check_pass = 0;
        g_env.m_check_fail = 0;
        g_env.m_fail       = 0;
    }

    void run()
    {
        do
        {
        } while (do_menu());
    }

    void check_pass()
    {
        ++m_check_pass_count;
    }

    void check_fail(size_t line)
    {
        ++m_check_fail_count;
        protocol::output_check_failure(m_io, line);
    }

    void fail(size_t line)
    {
        protocol::output_test_end(m_io, false, m_check_pass_count + m_check_fail_count, m_check_fail_count, line);
        bs::fatal();
    }

protected:
    bool do_menu()
    {
        protocol::output_menu_begin(m_io);
        int id = 1;
        for (TestCase* tc = g_env.m_registry.m_first; tc; tc = tc->next(), ++id)
        {
            protocol::output_menu_item(m_io, id, tc->name(), tc->desc());
        }
        protocol::output_menu_end(m_io);
        while (true)
        {
            int  id;
            char line_buf[BS_LINE_BUF_SIZE];
            if (!protocol::input_handle(m_io, line_buf, sizeof(line_buf), id))
            {
                continue;
            }
            if (id < 0)
            {
                return true;
            }
            TestCase* tc = g_env.m_registry.m_first;
            for (int i = 0; i != id - 1 && tc; ++i, tc = tc->next())
                ;
            if (!tc)
            {
                bs::fatal();
            }
            m_check_pass_count = 0;
            m_check_fail_count = 0;
            protocol::output_test_start(m_io, tc->file(), tc->line(), tc->name(), tc->desc());
            tc->run();
            bool success = m_check_fail_count == 0;
            protocol::output_test_end(m_io, success, m_check_pass_count + m_check_fail_count, m_check_fail_count);
        }
    }

protected:
    IO&    m_io;
    size_t m_check_pass_count;
    size_t m_check_fail_count;
};

class AutoReg
{
public:
    AutoReg(test_case_func_t func, const char* file, size_t line, const char* name, const char* desc = nullptr)
    {
        g_env.m_registry.add(func, file, line, name, desc);
    }
};

inline void check(bool condition, size_t line)
{
    if (!condition)
    {
        g_env.m_check_fail(line);
    }
    else
    {
        g_env.m_check_pass();
    }
}

inline void require(bool condition, size_t line)
{
    if (!condition)
    {
        g_env.m_check_fail(line);
        g_env.m_fail(line);
    }
    else
    {
        g_env.m_check_pass();
    }
}

}  // ::bs

#define BS_NAME_LINE2(name, line) name##line
#define BS_NAME_LINE(name, line) BS_NAME_LINE2(name, line)
#define BS_UNIQUE_NAME(name) BS_NAME_LINE(name, __LINE__)

#define TEST_CASE(...)                                                                                             \
    static void BS_UNIQUE_NAME(TEST_FUNC__)();                                                                     \
    namespace                                                                                                      \
    {                                                                                                              \
        bs::AutoReg BS_UNIQUE_NAME(test_autoreg__)(&BS_UNIQUE_NAME(TEST_FUNC__), __FILE__, __LINE__, __VA_ARGS__); \
    }                                                                                                              \
    static void BS_UNIQUE_NAME(TEST_FUNC__)()

#define CHECK(condition) bs::check((condition), __LINE__)
#define REQUIRE(condition) bs::require((condition), __LINE__)
#define FAIL() bs::g_env.m_fail(__LINE__)

#define BS_ENV_DECLARE() \
    namespace bs         \
    {                    \
        Env g_env;       \
    }
#define BS_RUN(...)                                                  \
    do                                                               \
    {                                                                \
        bs::IOHelper             helper = bs::IOHelper(__VA_ARGS__); \
        bs::Runner<bs::IOHelper> runner(helper);                     \
        runner.run();                                                \
    } while (0);

#endif  //BSTEST_H
