//
// expression_user.cpp
// Copyright (C) 2015 Félix Cloutier.
// All Rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// license. See LICENSE.md for details.
//

#include "expression_user.h"
#include "print.h"

using namespace llvm;
using namespace std;

namespace
{
	template<typename TAction>
	void iterateUseArrays(ExpressionUser* user, const ExpressionUseAllocInfo& allocatedAndUsed, TAction&& action)
	{
		auto arrayEnd = reinterpret_cast<ExpressionUse*>(user);
		unsigned allocated = allocatedAndUsed.allocated;
		unsigned used = allocatedAndUsed.used;
		auto arrayBegin = arrayEnd - allocated;
		while (arrayEnd != nullptr && action(arrayEnd - used, arrayEnd))
		{
			auto nextHead = &reinterpret_cast<ExpressionUseArrayHead*>(arrayBegin)[-1];
			used = nextHead->allocInfo.used;
			arrayBegin = nextHead->array;
			arrayEnd = arrayBegin == nullptr ? nullptr : arrayBegin + nextHead->allocInfo.allocated;
		}
	}
	
	template<typename TAction>
	void iterateUseArrays(const ExpressionUser* user, const ExpressionUseAllocInfo& allocatedAndUsed, TAction&& action)
	{
		auto arrayEnd = reinterpret_cast<const ExpressionUse*>(user);
		unsigned allocated = allocatedAndUsed.allocated;
		unsigned used = allocatedAndUsed.used;
		auto arrayBegin = arrayEnd - allocated;
		while (arrayEnd != nullptr && action(arrayEnd - used, arrayEnd))
		{
			auto nextHead = &reinterpret_cast<const ExpressionUseArrayHead*>(arrayBegin)[-1];
			used = nextHead->allocInfo.used;
			arrayBegin = nextHead->array;
			arrayEnd = arrayBegin == nullptr ? nullptr : arrayBegin + nextHead->allocInfo.allocated;
		}
	}
}

void ExpressionUser::anchor()
{
}

ExpressionUse& ExpressionUser::getOperandUse(unsigned int index)
{
	ExpressionUse* result = nullptr;
	iterateUseArrays(this, allocInfo, [&](ExpressionUse* begin, ExpressionUse* end)
	{
		ptrdiff_t count = end - begin;
		if (count >= index)
		{
			result = end - index - 1;
			return false;
		}
		else
		{
			index -= count;
			return true;
		}
	});
	
	return *result;
}

unsigned ExpressionUser::operands_size() const
{
	unsigned count = 0;
	iterateUseArrays(this, allocInfo, [&](const ExpressionUse* begin, const ExpressionUse* end)
	{
		count += end - begin;
		return true;
	});
	return count;
}

void ExpressionUser::print(raw_ostream& os) const
{
	// This doesn't really need the AstContext used to create the statements.
	// However, I'd say that it's bad practice to create a whole new AstContext
	// just to use StatementPrintVisitor. I'd be unhappy to see that kind of code
	// outside of debug code.
	DumbAllocator pool;
	AstContext context(pool);
	StatementPrintVisitor::print(context, os, *this, false);
}

void ExpressionUser::dump() const
{
	print(errs());
}
