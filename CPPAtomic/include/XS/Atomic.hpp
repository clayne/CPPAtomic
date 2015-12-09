/*******************************************************************************
 * Copyright (c) 2015, Jean-David Gadina - www.xs-labs.com
 * Distributed under the Boost Software License, Version 1.0.
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

/*!
 * @copyright   (c) 2015 - Jean-David Gadina - www.xs-labs.com
 * @brief       Replacement of std::atomic supporting non trivially-copyable types
 */

#ifndef XS_ATOMIC_H
#define XS_ATOMIC_H

#include <type_traits>
#include <utility>
#include <atomic>
#include <mutex>

namespace XS
{
    template< typename _T_, class _E_ = void >
    class Atomic
    {
        public:
            
            Atomic( void )                     = delete;
            Atomic( _T_ v )                    = delete;
            Atomic( const Atomic< _T_ > & o )  = delete;
            Atomic( const Atomic< _T_ > && o ) = delete;
            
            Atomic< _T_ > & operator =( _T_ v )                    = delete;
            Atomic< _T_ > & operator =( const Atomic< _T_ > & o )  = delete;
            Atomic< _T_ > & operator =( const Atomic< _T_ > && o ) = delete;
    };
    
    template< typename _T_ >
    class Atomic< _T_, typename std::enable_if< std::is_trivially_copyable< _T_ >::value >::type >
    {
        private:
            
            typedef std::atomic< _T_ > _A_;
            
        public:
            
            Atomic( void ): _v{}
            {}
            
            Atomic( _T_ v ): _v{ v }
            {}
            
            Atomic( const Atomic< _T_ > & o ): _v{ o._v.load() }
            {}
            
            Atomic( const Atomic< _T_ > && o ): _v{ std::move( o._v ) }
            {}
            
            ~Atomic( void )
            {}
            
            Atomic< _T_ > & operator =( Atomic< _T_ > o )
            {
                this->_v = o._v;
                
                return *( this );
            }
            
            Atomic< _T_ > & operator =( _T_ v )
            {
                this->_v = v;
                
                return *( this );
            }
            
            operator _T_ ( void ) const
            {
                return this->_v;
            }
            
            _T_ * operator ->( void ) const
            {
                return this->_v;
            }
            
            Atomic< _T_ > & operator ++ ( void )
            {
                return *( this );
            }
            
            Atomic< _T_ > operator ++ ( int )
            {
                return *( this );
            }
            
            Atomic< _T_ > & operator -- ( void )
            {
                return *( this );
            }
            
            Atomic< _T_ > operator -- ( int )
            {
                return *( this );
            }
            
            /*
            Atomic< _T_ > & operator +=
            Atomic< _T_ > & operator -=
            Atomic< _T_ > & operator &=
            Atomic< _T_ > & operator |=
            Atomic< _T_ > & operator ^=
            */
            
            bool IsLockFree( void )
            {
                return this->_v.is_lock_free();
            }
            
            friend void swap( Atomic< _T_ > & o1, Atomic< _T_ > & o2 )
            {
                using std::swap;
                
                swap( o1._v, o2._v );
            }
            
        private:
            
            _A_ _v;
    };
    
    template< typename _T_ >
    class Atomic< _T_, typename std::enable_if< !std::is_trivially_copyable< _T_ >::value >::type >
    {
        private:
            
            typedef std::recursive_mutex    _M_;
            typedef std::lock_guard< _M_ >  _L_;
            
        public:
            
            Atomic( void ): _v{}
            {}
            
            Atomic( _T_ v ): _v{ v }
            {}
            
            Atomic( const Atomic< _T_ > & o ): Atomic< _T_ >( o, _L_( o._rmtx ) )
            {}
            
            Atomic( const Atomic< _T_ > && o ): Atomic< _T_ >( o, _L_( o._rmtx ) )
            {}
            
            ~Atomic( void )
            {}
            
            Atomic< _T_ > & operator =( Atomic< _T_ > o )
            {
                std::lock( this->_rmtx, o._rmtx );
                
                _L_ l1( this->_rmtx, std::adopt_lock );
                _L_ l2( o._rmtx,     std::adopt_lock );
                
                this->_v = o._v;
                
                return *( this );
            }
            
            Atomic< _T_ > & operator =( _T_ v )
            {
                _L_ l( this->_rmtx );
                
                this->_v = v;
                
                return *( this );
            }
            
            operator _T_ ( void ) const
            {
                _L_ l( this->_rmtx );
                
                return this->_v;
            }
            
            _T_ * operator ->( void ) const
            {
                _L_ l( this->_rmtx );
                
                return this->_v;
            }
            
            Atomic< _T_ > & operator ++ ( void )
            {
                return *( this );
            }
            
            Atomic< _T_ > operator ++ ( int )
            {
                return *( this );
            }
            
            Atomic< _T_ > & operator -- ( void )
            {
                return *( this );
            }
            
            Atomic< _T_ > operator -- ( int )
            {
                return *( this );
            }
            
            /*
            Atomic< _T_ > & operator +=
            Atomic< _T_ > & operator -=
            Atomic< _T_ > & operator &=
            Atomic< _T_ > & operator |=
            Atomic< _T_ > & operator ^=
            */
            
            bool IsLockFree( void )
            {
                return false;
            }
            
            friend void swap( Atomic< _T_ > & o1, Atomic< _T_ > & o2 )
            {
                std::lock( o1._rmtx, o2._rmtx );
                
                _L_ l1( o1._rmtx, std::adopt_lock );
                _L_ l2( o2._rmtx, std::adopt_lock );
                
                using std::swap;
                
                swap( o1._v, o2._v );
            }
            
        private:
            
            Atomic( const Atomic< _T_ > & o, _L_ & l ): _v{ o._v }
            {
                ( void )l;
            }
            
            Atomic( const Atomic< _T_ > && o, _L_ & l ): _v{ std::move( o._v ) }
            {
                ( void )l;
            }
            
            _T_         _v;
            mutable _M_ _rmtx;
    };
}

#endif /* XS_ATOMIC_H */
