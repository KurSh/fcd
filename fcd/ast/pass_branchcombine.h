//
// pass_branchcombine.h
// Copyright (C) 2015 Félix Cloutier.
// All Rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// license. See LICENSE.md for details.
//


#ifndef fcd__ast_pass_branchcombine_h
#define fcd__ast_pass_branchcombine_h

#include "pass.h"

class AstBranchCombine final : public AstFunctionPass
{
protected:
	virtual void doRun(FunctionNode& fn) override;
	
public:
	virtual const char* getName() const override;
};

#endif /* fcd__ast_pass_branchcombine_h */
