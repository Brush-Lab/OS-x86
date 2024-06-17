#ifndef _TEMPLATE_MEMORY_HPP
#define _TEMPLATE_MEMORY_HPP

#include <stdint.h>

namespace Template {
    namespace Memory {
        template<typename T,uint32_t addr> 
        T &addr_to_ref_cast = static_cast<T&>(*reinterpret_cast<T*>(addr));

        template<typename T,uint32_t *linkersymbol>
        T &linker_to_ref_cast = static_cast<T&>(*reinterpret_cast<T*>(linkersymbol));
    }
}

#endif