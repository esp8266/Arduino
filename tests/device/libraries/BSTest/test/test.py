from mock_decorators import setup, teardown, setenv, request_env

@setup('environment variables can be set and read from python')
def setup_envtest(e):
    setenv(e, 'VAR_FROM_PYTHON', '42')


@teardown('environment variables can be set and read from python')
def teardown_envtest(e):
    env_value = request_env(e, 'VAR_FROM_TEST')
    assert(env_value == '24')
