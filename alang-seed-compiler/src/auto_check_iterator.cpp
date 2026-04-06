#include "auto_check_iterator.h"


namespace alang
{
	Iterator::Iterator(size_t idx, size_t size) : index(idx), totalSize(size) {}

	Iterator Iterator::operator++(int)
	{
		if (index + 1 > totalSize)
		{
			throw std::out_of_range("Attempted to advance iterator past the end of the range at index " + std::to_string(index) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		Iterator temp = *this;
		index++;
		return temp;
	}

	void Iterator::operator+=(size_t increment)
	{
		if (index + increment > totalSize)
		{
			throw std::out_of_range("Attempted to advance iterator by " + std::to_string(increment) + " from index " + std::to_string(index) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		index += increment;
	}

	void Iterator::operator+=(int increment)
	{
		if (index + static_cast<size_t>(increment) > totalSize)
		{
			throw std::out_of_range("Attempted to advance iterator by " + std::to_string(increment) + " from index " + std::to_string(index) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		index += increment;
	}

	void Iterator::operator-=(size_t decrement)
	{
		if (index - decrement < 0)
		{
			index = 0;
		}
		else
		{
			index -= decrement;
		}
	}

	void Iterator::operator-=(int decrement)
	{
		if (index - static_cast<size_t>(decrement) < 0)
		{
			index = 0;
		}
		else
		{
			index -= decrement;
		}
	}

	Iterator Iterator::operator+(size_t increment) const
	{
		if (index + increment > totalSize)
		{
			throw std::out_of_range("Attempted to advance iterator by " + std::to_string(increment) + " from index " + std::to_string(index) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		return Iterator(index + increment, totalSize);
	}

	Iterator Iterator::operator+(int increment) const
	{
		if (index + static_cast<size_t>(increment) > totalSize) 
		{
			throw std::out_of_range("Attempted to advance iterator by " + std::to_string(increment) + " from index " + std::to_string(index) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		return Iterator(index + increment, totalSize);
	}

	Iterator Iterator::operator-(size_t decrement) const
	{
		if (index - decrement < 0)
		{
			return Iterator(0, totalSize);
		}
		return Iterator(index - decrement, totalSize);
	}

	Iterator Iterator::operator-(int decrement) const
	{
		if (index - static_cast<size_t>(decrement) < 0)
		{
			return Iterator(0, totalSize);
		}
		return Iterator(index - decrement, totalSize);
	}

	Iterator& Iterator::operator=(size_t newIndex)
	{
		if (newIndex > totalSize)
		{
			throw std::out_of_range("Attempted to set iterator to index " + std::to_string(newIndex) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		index = newIndex;
		return *this;
	}

	Iterator& Iterator::operator=(int newIndex)
	{
		if (newIndex > totalSize)
		{
			throw std::out_of_range("Attempted to set iterator to index " + std::to_string(newIndex) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		index = newIndex;
		return *this;
	}

	bool Iterator::operator>(size_t other) const
	{
		return index > other;
	}

	bool Iterator::operator>=(size_t other) const
	{
		return index >= other;
	}

	bool Iterator::operator<=(size_t other) const
	{
		return index <= other;
	}

	Iterator::operator int() const
	{
		return static_cast<int>(index);
	}

	Iterator::operator size_t() const
	{
		return index;
	}

	size_t& Iterator::as_index()
	{
		return index;
	}

	bool Iterator::operator<(size_t other) const
	{
		return index < other;
	}


	Iterator::Iterator(const Iterator& other) noexcept
	{
		index = other.index;
	}

	Iterator::Iterator(Iterator&& other) noexcept
	{
		index = other.index;
	}

	Iterator& Iterator::operator++()
	{
		if (index + 1 > totalSize)
		{
			throw std::out_of_range("Attempted to advance iterator past the end of the range at index " + std::to_string(index) + ", which exceeds total size of " + std::to_string(totalSize) + ".");
		}
		++index;
		return *this;
	}

	void Iterator::expect_here(std::string_view msg)
	{
		errorMessage = msg;
	}



}