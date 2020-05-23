//==================================================================
/// DSmartPtr.h
///
/// Created by Davide Pasca - 2009/10/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DSMARTPTR_H
#define DSMARTPTR_H

#include "DContainers.h"
#include "DStr.h"

#if 1

# include <memory>

# define uptr std::unique_ptr

template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args) {
   static_assert(std::extent<T>::value == 0,
       "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

   typedef typename std::remove_extent<T>::type U;
   return std::unique_ptr<T>(new U[sizeof...(Args)]{std::forward<Args>(args)...});
}

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
   return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}

#else
//==================================================================
template <class _T>
class uptr
{
    _T	*mPtr;

public:
    uptr( _T *ptr=0 )
    {
        mPtr = ptr;
    }

    uptr &operator=( _T *ptr )
    {
        if ( mPtr && mPtr != ptr )
            DSAFE_DELETE( mPtr );

        mPtr = ptr;

        return *this;
    }

    ~uptr()
    {
        if ( mPtr )
            delete mPtr;
    }

    bool	is_null() const			{ return mPtr == NULL;	}

    const _T *operator->() const	{ return mPtr; }
          _T *operator->()			{ return mPtr; }

    const _T &operator*() const		{ return *mPtr; }
          _T &operator*()			{ return *mPtr; }

    const _T *get() const			{ return mPtr;	}
          _T *get()					{ return mPtr;	}

          _T *release()				{ _T *pRet = mPtr; mPtr = NULL; return pRet; }
};
#endif

//==================================================================
template <class _T>
class VecOwn
{
    DVec<_T>	mVec;

public:
    ~VecOwn()
    {
        clear_free();
    }

    size_t size() const { return mVec.size(); }

    void resize( size_t newSize )
    {
        mVec.resize( newSize );
    }

    void reserve( size_t reserveSize )
    {
        mVec.reserve( reserveSize );
    }
    
    void clear()
    {
        for (size_t i=0; i < size(); ++i)
        {
            DSAFE_DELETE( (*this)[i] );
        }

        mVec.clear();
    }

    void clear_free()
    {
        for (size_t i=0; i < size(); ++i)
        {
            DSAFE_DELETE( (*this)[i] );
        }

        mVec = DVec<_T>();
    }

    void erase( size_t idx )
    {
        DSAFE_DELETE( (*this)[idx] );

        mVec.erase( mVec.begin() + idx );
    }

    void push_back( const _T &val )
    {
        mVec.push_back( val );
    }

    void pop_back()
    {
        DSAFE_DELETE( (*this)[mVec.size()-1] );

        mVec.erase( mVec.begin() + (mVec.size()-1) );
    }

    const	_T &back() const	{ return mVec.back(); }
            _T &back()			{ return mVec.back(); }

    const _T &operator[]( size_t idx ) const { return mVec[ idx ]; }
          _T &operator[]( size_t idx )		 { return mVec[ idx ]; }
};

//==================================================================
template <class _T>
void Dclear_free( VecOwn<_T> &v ) {
    v = VecOwn<_T>();
}

//==================================================================
template <class _T>
class VecOwnNamed
{
    VecOwn<_T>	mVec;
    DVec<DStr>	mNames;

public:
    size_t size() const									{ return mVec.size(); }
    void resize( size_t newSize )						{ mVec.resize( newSize ); mNames.resize( newSize ); }
    void reserve( size_t reserveSize )                  { mVec.reserve( reserveSize ); mNames.reserve( reserveSize ); }
    void clear()										{ mVec.clear(); mNames.clear();  }
    void clear_free()									{ mVec.clear_free(); Dclear_free( mNames ); }
    void erase( size_t idx )							{ mVec.erase( idx ); mNames.erase( mNames.begin() + idx ); }
    void push_back( const _T &val, const char *pName )	{ mVec.push_back( val ); mNames.push_back( pName ); }

    const	_T &back() const	{ return mVec.back(); }
            _T &back()			{ return mVec.back(); }

    const _T &operator[]( const char *pSrcName ) const	{ return mVec[ FindByName( pSrcName ) ]; }
          _T &operator[]( const char *pSrcName )		{ return mVec[ FindByName( pSrcName ) ]; }

    const _T &operator[]( size_t idx ) const { return mVec[ idx ]; }
          _T &operator[]( size_t idx )		 { return mVec[ idx ]; }

    size_t FindByName( const char *pSrcName ) const
    {
        for (size_t i=0; i < mNames.size(); ++i)
            if ( 0 == strcmp( mNames[i].c_str(), pSrcName ) )
                return i;

        return DNPOS;
    }
};

//==================================================================
class RCBaseNoDel
{
    int		mCount;
    
public:
    RCBaseNoDel() :
        mCount(0)
    {
    }
    
    virtual ~RCBaseNoDel()
    {
        DASSERT( mCount == 0 );
    }

    void AddRef()
    {
        mCount += 1;
    }

    int SubRef()
    {
        DASSERT( mCount >= 1 );
        mCount -= 1;
        int retVal = mCount;

        if ( retVal == 0 )
            OnZeroCount();

        return retVal;

    }

    virtual void OnZeroCount()
    {
    }

    int GetRef() const
    {
        return mCount;
    }

private:
    RCBaseNoDel( const RCBaseNoDel &from ) :	mCount(0)
    {
    }
    
    RCBaseNoDel &operator=( const RCBaseNoDel &from )
    {
        mCount = 0;
        return *(RCBaseNoDel *)0;
    }
};

//==================================================================
/// RefCount
//==================================================================
template <class _T>
class RCSha
{
    _T	*mPtr;

public:
    RCSha()						: mPtr(NULL)	{ }
    RCSha( _T *ptr )			: mPtr(NULL)	{ borrow( ptr ); }
    RCSha( const RCSha &from )	: mPtr(NULL)	{ borrow( from.get() ); }

    virtual ~RCSha()
    {
        if ( mPtr )
            mPtr->SubRef();
    }

    RCSha &operator=( const RCSha &from )	{	borrow( from.get() );	return *this; }
    RCSha &operator=( _T *ptr )				{	borrow( ptr );			return *this; }
    RCSha &operator=( const _T *ptr )		{	borrow( ptr );			return *this; }

private:
    void borrow( _T *ptr )
    {
        if ( ptr == mPtr )
            return;

        if ( ptr )
            ptr->AddRef();

        if ( mPtr )
            mPtr->SubRef();

        mPtr = ptr;
    }

    void borrow( const _T *ptr )
    {
        if ( ptr == mPtr )
            return;

        if ( ptr )
            ((_T *)ptr)->AddRef();

        if ( mPtr )
            mPtr->SubRef();

        mPtr = (_T *)ptr;
    }

public:
    const _T *get() const
    {
        DASSERT( !mPtr || mPtr->GetRef() > 0 );

        return mPtr;
    }

    _T *get()
    {
        DASSERT( !mPtr || mPtr->GetRef() > 0 );

        return mPtr;
    }

    const _T *operator->() const	{ return mPtr; }
          _T *operator->()			{ return mPtr; }

    const _T &operator*() const		{ return *mPtr; }
          _T &operator*()			{ return *mPtr; }

};

#endif
