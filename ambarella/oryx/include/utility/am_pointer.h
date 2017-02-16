/*******************************************************************************
 * am_pointer.h
 *
 * History:
 *   2014-12-23 - [ypchang] created file
 *
 * Copyright (C) 2008-2014, Ambarella Co, Ltd.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella.
 *
 ******************************************************************************/
#ifndef ORYX_INCLUDE_UTILITY_AM_POINTER_H_
#define ORYX_INCLUDE_UTILITY_AM_POINTER_H_

/*! @file am_pointer.h
 *  @brief This file defines a class template to manage various class pointers.
 */
#include "am_log.h"

/*! @addtogroup HelperClass
 *  @{
 */
/*! @class AMPointer
 *  @brief A smart pointer class template type used to manage class pointer.
 *
 *  This class template manages class pointer, makes it possible to assign
 *  a class pointer between threads and functions without caring about calling
 *  delete or destroy of the managed class.
 *  To use this class template to manage your class pointer, your class must
 *  meet the following requirement:
 *  - Provides inc_ref() method;
 *  .
 *  - Provides release() method;
 *  .
 *  For more information, please refer to AMIVideoReader and AMVideoReader to
 *  learn how to implement such class that can be managed by AMPointer.
 */
template<class ClassType> class AMPointer
{
  public:
    /*!
     * Constructor.
     */
    AMPointer() :
      m_raw(nullptr)
    {}

    /*!
     * Constructor.
     * @param objPtr a const AMPointer reference.
     */
    AMPointer(const AMPointer &objPtr)
    {
      m_raw = objPtr.m_raw;
      if (m_raw) {
        m_raw->inc_ref();
      }
    }

    /*!
     * Constructor.
     * @param ptr ClassType pointer.
     */
    AMPointer(ClassType *ptr) :
      m_raw(ptr)
    {
      if (m_raw) {
        m_raw->inc_ref();
      }
    }

    /*!
     * Destructor
     */
    virtual ~AMPointer()
    {
      if (m_raw) {
        m_raw->release();
        m_raw = nullptr;
      }
      DEBUG("~AMPointer");
    }

  public:
    /*!
     * == operator overload function.
     * @param objPtr const AMPointer object reference
     * @return true if two AMPointer objects are equal, otherwise return false.
     */
    bool operator==(const AMPointer &objPtr)
    {
      return ((this == &objPtr) || (m_raw == objPtr.m_raw));
    }

    /*!
     * != operator overload function.
     * @param objPtr const AMPointer object reference
     * @return true if two AMPointer objects are different,
     *         otherwise return false.
     */
    bool operator!=(const AMPointer &objPtr)
    {
      return !((*this) == objPtr);
    }

    /*!
     * == operator overload function.
     * @param objPtr ClassType pointer
     * @return true if two ClassType pointers are equal, otherwise return false.
     */
    bool operator==(ClassType *ptr)
    {
      return (m_raw == ptr);
    }

    /*!
     * != operator overload function.
     * @param objPtr ClassType pointer
     * @return true if two ClassType pointers are different,
     *         otherwise return false.
     */
    bool operator!=(ClassType *ptr)
    {
      return (m_raw != ptr);
    }

    /*!
     * ! operator overload function.
     * @return true if the underlying pointer is NULL, otherwise return false.
     */
    bool operator!()
    {
      return (nullptr == m_raw);
    }

    /*!
     * = operator overload function.
     * @param objPtr const AMPointer object reference
     * @return an reference to this AMPointer object.
     */
    AMPointer& operator=(const AMPointer &objPtr)
    {
      ClassType *raw = objPtr.m_raw;
      if (raw) {
        raw->inc_ref();
      }
      if (m_raw) {
        m_raw->release();
      }
      m_raw = raw;

      return (*this);
    }

    /*!
     * = operator overload function
     * @param raw ClassType pointer
     * @return an reference to this AMPointer object.
     */
    AMPointer& operator=(ClassType *raw)
    {
      if (raw) {
        raw->inc_ref();
      }
      if (m_raw) {
        m_raw->release();
      }
      m_raw = raw;

      return (*this);
    }

    /*!
     * * operator overload function.
     * @return ClassType object reference.
     */
    ClassType& operator*() const
    {
      return (*m_raw);
    }

    /*!
     * (ClassType*) overload function.
     * Convert current AMPointer type to ClassType*.
     */
    operator ClassType*() const
    {
      return (m_raw);
    }

    /*!
     * -> operator overload function.
     * @return ClassType pointer.
     */
    ClassType* operator->() const
    {
      return m_raw;
    }

    /*!
     * ** operator overload function.
     * @return The address of ClassType pointer managed by AMPointer.
     */
    ClassType** operator&()
    {
      return (m_raw ? &m_raw : nullptr);
    }

    /*!
     * Return the ClassType pointer managed by this class.
     * @return ClassType*
     */
    ClassType* raw()
    {
      return m_raw;
    }

    /*! Detach the managed ClassType pointer from AMPointer.
     * @return ClassType pointer.
     */
    ClassType* detach()
    {
      ClassType *raw = m_raw;
      m_raw = nullptr;

      return raw;
    }

    /*! Attach the ClassType pointer to AMPointer,
     *  make it managed by this class.
     * @param raw ClassType pointer.
     */
    void attach(ClassType *raw)
    {
      if (m_raw) {
        m_raw->release();
      }
      m_raw = raw;
    }

  protected:
    ClassType *m_raw;
};
/*!
 * @}
 */

template<class Type, class CastType = Type>
class AMSmartPtr: public AMPointer<CastType>
{
  public:
    AMSmartPtr(): AMPointer<CastType>()
    {}

    AMSmartPtr(Type *raw): AMPointer<CastType>(raw)
    {}

    Type* detach()
    {
      return static_cast<Type*>(AMPointer<CastType>::detach());
    }

    Type* operator->() const
    {
      return static_cast<Type*>(AMPointer<CastType>::m_raw);
    }

    Type* ptr() const
    {
      return static_cast<Type*>(AMPointer<CastType>::m_raw);
    }
};

// Use this template when creating STL containers of SmartPtrs.
// e.g. use vector<AMSmartVctPtr<object> > instead of
//          vector<AMSmartPtr<object> >
// This is because STL uses operator& which does the wrong thing

template<class T> class AMSmartVctPtr: public AMSmartPtr<T>
{
  public:
    AMSmartVctPtr(): AMSmartPtr<T>(){}
    AMSmartVctPtr(T *ptr): AMSmartPtr<T>(ptr){}
    AMSmartVctPtr(AMSmartPtr<T> &t): AMSmartPtr<T>(t){}
    AMSmartVctPtr* operator&(){return this;}
};

#endif /* ORYX_INCLUDE_UTILITY_AM_POINTER_H_ */
