
env = dict()

def setup(test_name):
	global env
	if not test_name in env:
		env[test_name] = dict()
	func_env = env[test_name]
	def decorator(func):
		def func_wrapper():
			return func(env[test_name])
		func_env['setup'] = func_wrapper
		return func_wrapper
	return decorator



def teardown(test_name):
	global env
	if not test_name in env:
		env[test_name] = dict()
	func_env = env[test_name]
	def decorator(func):
		def func_wrapper():
			return func(env[test_name])
		func_env['teardown'] = func_wrapper
		return func_wrapper
	return decorator
