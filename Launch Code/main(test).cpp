//
//  main.cpp
//  F9FirstLaunch
//
//  Created by Quan Bach  on 9/25/17.
//  Copyright © 2017 QB. All rights reserved.
//

#include "Launch.h"

int main()
{
    Launch myLaunch;
    
    myLaunch.launchToTargetAlt(70000);
    myLaunch.takeSecondToOrbit(125000, 125000);
    myLaunch.landingFirstStage();
    return 0;
}
