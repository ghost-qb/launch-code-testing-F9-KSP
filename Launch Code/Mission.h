//
//  Mission.h
//  KRPC
//
//  Created by Quan Bach  on 11/3/17.
//  Copyright © 2017 QB. All rights reserved.
//

#ifndef Mission_h
#define Mission_h

#include <chrono>
#include <thread>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <krpc/services/krpc.hpp>

class Mission
{
public:
    void TestLaunch(krpc::Client & _conn);
    
};


void Mission::TestLaunch(krpc::Client & _conn)
{
    
    krpc::services::SpaceCenter space_center(&_conn);
    auto vessel = space_center.active_vessel();

    for (int i = 0; i < 4; i++)
    {
        space_center.set_physics_warp_factor(3);
        std::this_thread::sleep_for(std::chrono::seconds(15));
        space_center.set_physics_warp_factor(0);
        vessel.control().activate_next_stage();
    }

}
#endif /* Mission_h */
