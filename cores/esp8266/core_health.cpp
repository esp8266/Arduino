
#include <stdint.h>
#include <core_health_setter.h>

static uint32_t health = 0;

int core_condition_is_fatal (void)
{
    return health > ((((decltype(health))1) << core_warn_last) - 1);
}

int core_condition_has (core_condition_e disease)
{
    decltype(health) bits = ((decltype(health))1) << disease;
    int badmood = !!(health & bits);
    health &= ~bits;
    return badmood;
}

void core_condition_got (core_condition_e disease)
{
    health |= ((decltype(health))1) << disease;
}
