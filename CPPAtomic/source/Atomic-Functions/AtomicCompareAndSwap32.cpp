/*******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Jean-David Gadina - www.xs-labs.com / www.digidna.net
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @copyright   (c) 2015 - Jean-David Gadina - www.xs-labs.com / www.digidna.net
 * @brief       ...
 */

#include <XS/Atomic-Functions.hpp>

#if defined( _WIN32 )

#include <Windows.h>
#include <Winnt.h>

#elif defined( __APPLE__ )

#include <libkern/OSAtomic.h>

#endif

namespace XS
{
    bool AtomicCompareAndSwap32( int32_t oldValue, int32_t newValue, volatile int32_t * value )
    {
        #if defined( _WIN32 )
        
        return ( InterlockedCompareExchange( static_cast< volatile LONG * >( value ), newValue, oldValue ) == oldValue ) ? true : false;
        
        #elif defined( __APPLE__ )
        
        return ( OSAtomicCompareAndSwap32( static_cast< int32_t >( oldValue ), static_cast< int32_t >( newValue ), static_cast< volatile int32_t * >( value ) ) ) ? true : false;
        
        #elif __has_builtin( __sync_bool_compare_and_swap )
        
        return __sync_bool_compare_and_swap( value, oldValue, newValue );
        
        #else
        
        #error "XS::Atomic::CompareAndSwap32 is not implemented for the current platform"
        
        #endif
    }
}
