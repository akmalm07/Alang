#pragma once

#include <stdexcept>
#include <string>

namespace alang
{
	struct Iterator
	{
	public:
		Iterator(size_t idx, size_t size);

		Iterator(const Iterator& other) noexcept;
		Iterator(Iterator&& other) noexcept;

		Iterator& operator++(); // prefix increment

		void expect_here(std::string_view msg);

		// postfix increment
		Iterator operator++(int);

		void operator+=(size_t increment);
		void operator+=(int increment);

		void operator-=(size_t decrement);
		void operator-=(int decrement);

		Iterator operator+(size_t increment) const;
		Iterator operator+(int increment) const;

		Iterator operator-(size_t decrement) const;
		Iterator operator-(int decrement) const;


		// assignment from size_t
		Iterator& operator=(size_t newIndex);
		Iterator& operator=(int newIndex);

		operator int() const;
		operator size_t() const;
		size_t& as_index();// allow binding to size_t&
		
		bool operator<(size_t other) const;
		bool operator>(size_t other) const;
		bool operator>=(size_t other) const;
		bool operator<=(size_t other) const;

	private:
		size_t index;
		size_t totalSize;
		std::string errorMessage;
	};
}