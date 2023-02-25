/* Copyright (c) 2022 Cloud Software Group, Inc.
   This file is subject to the license terms contained in the
   license file that is distributed with this file. */

#ifndef SBDF_916D1C2F_24CD_4d61_BAE6_23A50D2C01CE
#define SBDF_916D1C2F_24CD_4d61_BAE6_23A50D2C01CE

#include "object.h"
#include "valuetypeid.h"

#include <cassert>
#include <cstdint>

#if !_MSC_VER
#define _tempnam tempnam
#endif

namespace sbdf
{
	template <typename t>
	struct id;

	template <> struct id<bool> { enum { result = SBDF_BOOLTYPEID }; };
	template <> struct id<int32_t> { enum { result = SBDF_INTTYPEID }; };
	template <> struct id<int64_t> { enum { result = SBDF_LONGTYPEID }; };
	template <> struct id<float> { enum { result = SBDF_FLOATTYPEID }; };
	template <> struct id<double> { enum { result = SBDF_DOUBLETYPEID }; };

	class obj
	{
		sbdf_object* obj_;

	public:
		obj() : obj_(0)
		{
		}

		template <typename T>
		obj(T const& t) : obj_(0)
		{
			sbdf_valuetype vt = { id<T>::result };
			sbdf_obj_create(vt, &t, 0, &obj_);
		}

		template <typename T, int N>
		obj(T const(&t)[N]) : obj_(0)
		{
			sbdf_valuetype vt = { id<T>::result };
			sbdf_obj_create_arr(vt, N, &t, 0, &obj_);
		}

		template <typename T>
		obj(T const* t, int N) : obj_(0)
		{
			sbdf_valuetype vt = { id<T>::result };
			sbdf_obj_create_arr(vt, N, t, 0, &obj_);
		}

		obj(char const* s) : obj_(0)
		{
			sbdf_valuetype vts = { SBDF_STRINGTYPEID };
			sbdf_obj_create(vts, &s, 0, &obj_);
		}

		template <int N>
		obj(char const* (&s)[N]) : obj_(0)
		{
			sbdf_valuetype vts = { SBDF_STRINGTYPEID };
			sbdf_obj_create_arr(vts, N, s, 0, &obj_);
		}

		obj(obj const& rhs) : obj_(0)
		{
			sbdf_obj_copy(rhs.obj_, &obj_);
		}

		~obj()
		{
			sbdf_obj_destroy(obj_);
			obj_ = 0;
		}

		obj& operator=(obj const& rhs)
		{
			sbdf_object* t = 0;
			sbdf_obj_copy(rhs.obj_, &t);
			if (obj_)
			{
				sbdf_obj_destroy(obj_);
			}
			obj_ = t;
			return *this;
		}

		bool operator==(obj const& rhs) const
		{
			return !!sbdf_obj_eq(obj_, rhs.obj_);
		}

		bool operator!=(obj const& rhs) const
		{
			return !sbdf_obj_eq(obj_, rhs.obj_);
		}

		operator sbdf_object const*() const
		{
			return obj_;
		}

		sbdf_object const* operator->() const
		{
			return obj_;
		}

		sbdf_object* operator->()
		{
			return obj_;
		}

		sbdf_object** obj_ptr()
		{
			if (obj_)
			{
				sbdf_obj_destroy(obj_);
				obj_ = 0;
			}

			return &obj_;
		}

		template <typename T>
		std::vector<T> as_vec() const
		{
			assert(id<T>::result == obj_->type.id); // should have used exceptions
			return std::vector<T>((T*)obj_->data, (T*)obj_->data + obj_->count);
		}
	};
}

template <typename T>
class scoped_ptr
{
	T* ptr_;
	int* ref_;
	void (*dt_)(T*);
public:
	scoped_ptr(T* ptr, void (*dt)(T*)) : ptr_(ptr), ref_(new int(1)), dt_(dt)
	{
	}

	scoped_ptr(scoped_ptr const& rhs) : ptr_(rhs.ptr_), ref_(rhs.ref_), dt_(rhs.dt_)
	{
		++*ref_;
	}

	~scoped_ptr()
	{
		if (--*ref_ == 0)
		{
			dt_(ptr_);
			delete ref_;
		}
	}

	operator T*() const
	{
		return ptr_;
	}

	T* operator->() const
	{
		return ptr_;
	}

	T** obj_ptr()
	{
		if (--*ref_ == 0)
		{
			dt_(ptr_);
			delete ref_;
		}

		ref_ = new int(1);
		ptr_ = 0;
		return &ptr_;
	}
};

template <typename T>
scoped_ptr<T> make_scoped_ptr(T* ptr, void (*dt)(T*))
{
	return scoped_ptr<T>(ptr, dt);
}

template <typename T>
scoped_ptr<T> make_scoped_ptr(void (*dt)(T*))
{
	return scoped_ptr<T>(0, dt);
}

#endif
