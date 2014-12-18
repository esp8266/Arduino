#ifndef TICKER_H
#define TICKER_H

#include <stdint.h>
#include <stddef.h>

extern "C" {
	typedef struct _ETSTIMER_ ETSTimer;
}

class Ticker
{
public:
	Ticker();
	~Ticker();
	typedef void (*callback_t)(void);
	typedef void (*callback_with_arg_t)(void*);

	void attach(callback_t callback, float seconds)
	{
		_attach_ms(reinterpret_cast<callback_with_arg_t>(callback), seconds * 1000, 0);
	}

	void attach_ms(callback_t callback, uint32_t milliseconds)
	{
		_attach_ms(reinterpret_cast<callback_with_arg_t>(callback), milliseconds, 0);
	}

	template<typename TArg>
	void attach(void (*callback)(TArg), float seconds, TArg arg)
	{
		static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach() callback argument size must be <= 4 bytes");
		uint32_t arg32 = static_cast<uint32_t>(arg);
		_attach_ms(reinterpret_cast<callback_with_arg_t>(callback), seconds * 1000, reinterpret_cast<void*>(arg32));
	}

	template<typename TArg>
	void attach_ms(void (*callback)(TArg), uint32_t milliseconds, TArg arg)
	{
		static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach_ms() callback argument size must be <= 4 bytes");
		uint32_t arg32 = static_cast<uint32_t>(arg);
		_attach_ms(reinterpret_cast<callback_with_arg_t>(callback), milliseconds, reinterpret_cast<void*>(arg32));
	}

	void detach();

protected:	
	void _attach_ms(callback_with_arg_t callback, uint32_t milliseconds, void* arg);


protected:
	ETSTimer* _timer;
};


#endif//TICKER_H
