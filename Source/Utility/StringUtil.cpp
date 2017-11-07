#include "stdafx.h"
#include "StringUtil.h"

void Split(ConstStringRef str, char split_char, std::vector<ConstStringRef>* pieces)
{
	ConstStringRef cur = str;

	for (const char* p = str.Begin; p < str.End; ++p)
	{
		if (*p == split_char)
		{
			cur.End = p;
			pieces->push_back(cur);
			cur.Begin = p + 1;
		}
	}

	cur.End = str.End;
	if (cur.Begin <= cur.End)
	{
		pieces->push_back(cur);
	}
}

void SplitAt(ConstStringRef str, char split_char, ConstStringRef* left, ConstStringRef* right)
{
	for (const char* p = str.Begin; p < str.End; ++p)
	{
		if (*p == split_char)
		{
			left->Begin = str.Begin;
			left->End = p;
			right->Begin = p + 1;
			right->End = str.End;
			return;
		}
	}

	// Split char wasn't found - just return the whole string as 'left'.
	*left = str;
	*right = ConstStringRef();
}

