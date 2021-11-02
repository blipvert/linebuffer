#pragma once

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <ctype.h>
#include <assert.h>

class linebuffer
{
public:
	linebuffer(size_t blocksize = 1024) : blocksize_(blocksize)
	{
		if ((buffer_ = (char*)malloc(buffersize_ = blocksize_)) == nullptr)
		{
			throw std::runtime_error("linebuffer: allocation failed");
		}
		readptr_ = buffer_;
	}

	~linebuffer()
	{
		if (buffer_ != nullptr)
			free(buffer_);
	}

	char* getline(FILE* fp)
	{
		reset();

		while (readfrom(fp) && lastchar() != '\n')
		{
			if (avail() < blocksize_ / 2)
				expand();
		}

		if (empty())
		{
			return nullptr;
		}

		chomp();
		return buffer_;
	}

	inline char lastchar()
	{
		return readptr_ > buffer_ ? *(readptr_ - 1) : '\0';
	}

	char chop()
	{
		if (readptr_ > buffer_)
		{
			char c = *--readptr_;
			*readptr_ = '\0';
			return c;
		}
		return '\0';
	}

	void chomp()
	{
		if (lastchar() == '\n')
		{
			chop();
			if (lastchar() == '\r')
				chop();
		}
	}

	void rstrip()
	{
		char c;

		while ((c = lastchar()) && isspace(c))
		{
			chop();
		}
	}

	void lstrip()
	{
		char* cp = buffer_;
		while (isspace(*cp))
			++cp;
		if (cp > buffer_)
		{
			readptr_ = buffer_;
			while (*cp)
			{
				*readptr_++ = *cp++;
			}
			*readptr_ = '\0';
		}
	}

	void strip()
	{
		lstrip();
		rstrip();
	}

	inline bool empty()
	{
		return readptr_ == buffer_;
	}

	inline void reset()
	{
		assert(buffer_ != nullptr);
		*(readptr_ = buffer_) = '\0';
	}

	inline size_t readoffset()
	{
		return readptr_ - buffer_;
	}

	inline size_t avail()
	{
		return buffersize_ - readoffset();
	}

protected:
	void setbuffer(char* newbuffer)
	{
		assert(newbuffer != nullptr);
		readptr_ = newbuffer + readoffset();
		buffer_ = newbuffer;
	}

private:
	void expand()
	{
		size_t newsize = buffersize_ + blocksize_;
		char* newbuffer = (char *)realloc(buffer_, newsize);
		if (newbuffer == nullptr)
		{
			throw std::runtime_error("linebuffer: reallocation failed");
		}
		else
		{
			setbuffer(newbuffer);
			buffersize_ = newsize;
		}
	}

	bool readfrom(FILE* fp)
	{
		if (fgets(readptr_, (int)avail(), fp) != nullptr)
		{
			readptr_ += strlen(readptr_);
			return true;
		}
		return false;
	}

	const size_t blocksize_;
	size_t buffersize_;
	char* buffer_;
	char* readptr_;
};
