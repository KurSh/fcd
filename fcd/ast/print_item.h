//
// print_item.h
// Copyright (C) 2015 Félix Cloutier.
// All Rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// license. See LICENSE.md for details.
//

#ifndef print_item_hpp
#define print_item_hpp

#include "dumb_allocator.h"
#include "not_null.h"

#include <llvm/Support/raw_ostream.h>

#include <string>
#include <vector>

class PrintableScope;

class PrintableItem
{
public:
	enum Type
	{
		Scope,
		Statement,
	};
	
private:
	DumbAllocator& allocator;
	Type discriminant;
	PrintableScope* parent;
	
protected:
	DumbAllocator& pool() { return allocator; }
	
public:
	PrintableItem(Type type, DumbAllocator& allocator, PrintableScope* parent)
	: allocator(allocator), discriminant(type), parent(parent)
	{
	}
	
	// no destructor on purpose, since this type must be trivially destructible
	
	Type getType() const { return discriminant; }
	PrintableScope* getParent() { return parent; }
	
	virtual void print(llvm::raw_ostream& os, unsigned indent) const = 0;
	void dump() const;
};

class PrintableLine : public PrintableItem
{
	NOT_NULL(const char) line;
	
public:
	static bool classof(const PrintableItem* stmt)
	{
		return stmt->getType() == Statement;
	}
	
	PrintableLine(DumbAllocator& allocator, PrintableScope* parent, NOT_NULL(const char) line)
	: PrintableItem(Statement, allocator, parent), line(pool().copyString(llvm::StringRef(line)))
	{
	}
	
	NOT_NULL(const char) getLine() const { return line; }
	void setLine(NOT_NULL(const char) line) { this->line = pool().copyString(llvm::StringRef(line)); }
	
	virtual void print(llvm::raw_ostream& os, unsigned indent) const override;
};

class PrintableScope : public PrintableItem
{
	const char* prefix;
	const char* suffix;
	PooledDeque<NOT_NULL(PrintableItem)> prepended;
	PooledDeque<NOT_NULL(PrintableItem)> items;
	
public:
	static bool classof(const PrintableItem* stmt)
	{
		return stmt->getType() == Scope;
	}
	
	PrintableScope(DumbAllocator& allocator, PrintableScope* parent)
	: PrintableItem(Scope, allocator, parent), prefix(nullptr), suffix(nullptr), prepended(allocator), items(allocator)
	{
	}
	
	const char* getPrefix() const { return prefix; }
	const char* getSuffix() const { return suffix; }
	void setPrefix(NOT_NULL(const char) prefix) { this->prefix = pool().copyString(llvm::StringRef(prefix)); }
	void setSuffix(NOT_NULL(const char) suffix) { this->suffix = pool().copyString(llvm::StringRef(suffix)); }
	
	PrintableItem* prependItem(NOT_NULL(const char) line);
	PrintableItem* appendItem(NOT_NULL(const char) line);
	PrintableItem* appendItem(NOT_NULL(PrintableItem) statement);
	
	virtual void print(llvm::raw_ostream& os, unsigned indent) const override;
};

#endif /* print_item_hpp */
