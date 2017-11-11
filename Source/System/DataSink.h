#ifndef SYSTEM_DATASINK_H_
#define SYSTEM_DATASINK_H_

#include <stdio.h>

#include <string>

class DataSink
{
   public:
	virtual ~DataSink();
	virtual size_t Write(const void* p, size_t len) = 0;
	virtual void Flush() = 0;
};

class FileSink : public DataSink
{
   public:
	FileSink();
	~FileSink();

	bool Open(const std::string& filename, const char* mode);

	virtual size_t Write(const void* p, size_t len);
	virtual void Flush();

   private:
	FILE* Handle;
};

#endif  // SYSTEM_DATASINK_H_
