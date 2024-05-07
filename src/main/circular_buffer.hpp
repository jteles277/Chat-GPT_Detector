/*
 * circular_buffer.hpp
 * 
 * Description:
 *   This header file defines the circular_buffer class, implementing a circular buffer in C++.
 * 
 * Adapted from:
 *   https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/circular_buffer/circular_buffer.hpp
 * 
 * Modifications:
 *   - Created a constructor to initialize the buffer with a specified capacity.
 *   - Introduced a Get method to retrieve an element at a given index.
 *   - Renamed the original Get method to Pop for clarity.
 */

#ifndef CIRCULAR_BUFFER_HPP_
#define CIRCULAR_BUFFER_HPP_

#include <mutex>
#include <optional>

using namespace std;

template<class T>
class circular_buffer
{
  	public:
		explicit circular_buffer(size_t capacity) : capacity_(capacity), buf_(new T[capacity]), head_(0), tail_(0), full_(false) {}
	
		~circular_buffer()
		{
			delete[] buf_;
		}
	
		void put(T item) noexcept
		{
			lock_guard<recursive_mutex> lock(mutex_);
	
			buf_[head_] = item;
	
			if(full_)
			{
				tail_ = (tail_ + 1) % capacity_;
			}
	
			head_ = (head_ + 1) % capacity_;
	
			full_ = head_ == tail_;
		}

		optional<T> get(size_t index) noexcept
		{
			lock_guard<recursive_mutex> lock(mutex_);
	
			if(index >= size())
			{
				return nullopt;
			}
	
			// Read data and advance the tail (we now have a free space)
			auto val = buf_[(tail_ + index) % capacity_];
	
			return val;
		}
	
		optional<T> pop() noexcept
		{
			lock_guard<recursive_mutex> lock(mutex_);
	
			if(empty())
			{
				return nullopt;
			}
	
			// Read data and advance the tail (we now have a free space)
			auto val = buf_[tail_];
			full_ = false;
			tail_ = (tail_ + 1) % capacity_;
	
			return val;
		}
	
		void reset() noexcept
		{
			lock_guard<recursive_mutex> lock(mutex_);
			head_ = tail_;
			full_ = false;
		}
	
		bool empty() const noexcept
		{
			// Can have a race condition in a multi-threaded application
			lock_guard<recursive_mutex> lock(mutex_);
			// if head and tail are equal, we are empty
			return (!full_ && (head_ == tail_));
		}
	
		bool full() const noexcept
		{
			// If tail is ahead the head by 1, we are full
			return full_;
		}
	
		size_t capacity() const noexcept
		{
			return capacity_;
		}
	
		size_t size() const noexcept
		{
			// A lock is needed in size ot prevent a race condition, because head_, tail_, and full_
			// can be updated between executing lines within this function in a multi-threaded
			// application
			lock_guard<recursive_mutex> lock(mutex_);
	
			size_t size = capacity_;
	
			if(!full_)
			{
				if(head_ >= tail_)
				{
					size = head_ - tail_;
				}
				else
				{
					size = capacity_ + head_ - tail_;
				}
			}
	
			return size;
		}

		operator string() const noexcept
		{
			lock_guard<recursive_mutex> lock(mutex_);

			string str;
			str.reserve(size());

			for (size_t i = 0; i < size(); i++)
			{
				str.push_back(buf_[(tail_ + i) % capacity_]);
			}
			return str;
		}

  	private:
		mutable recursive_mutex mutex_;
		size_t capacity_;
		mutable T* buf_;
		mutable size_t head_;
		mutable size_t tail_;
		mutable bool full_;
};

#endif // CIRCULAR_BUFFER_HPP_