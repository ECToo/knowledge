/*
 *  xcode.cp
 *  xcode
 *
 *  Created by nightz on 9/14/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "xcode.h"
#include "xcodePriv.h"

void xcode::HelloWorld(const char * s)
{
	 xcodePriv *theObj = new xcodePriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void xcodePriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

