//
//  main.cpp
//  F9FirstLaunch
//
//  Created by Quan Bach  on 9/25/17.
//  Copyright © 2017 QB. All rights reserved.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <cmath>

int main()
{
    krpc::Client conn = krpc::connect("F9-First-Launch", "127.0.0.1",50000,50001);
    
    
    
//    //establish connection to KSP
//    krpc::Client conn = krpc::connect("F9-First-Launch");
    krpc::services::SpaceCenter space_center(&conn);
    
    //assign var vessel to the active vessel on launch pad
    auto vessel = space_center.active_vessel();
    
    double targetPitch = 90;
    
    //SET UP AUTO-PILOT
    //pitch to target pitch
    vessel.auto_pilot().set_target_pitch(targetPitch);
    vessel.auto_pilot().set_target_heading(90);
    //activate auto pilot
    vessel.auto_pilot().engage();
    
    //set throttle power to 100%
    vessel.control().set_throttle(1);
    
    
    //enbale SAS and set to stability assist
//    vessel.auto_pilot().set_sas(true);
//    vessel.auto_pilot().set_sas_mode(krpc::services::SpaceCenter::SASMode::stability_assist);
    //disbale RCS
    vessel.control().set_rcs(false);
    
    
    //set reference frame
    auto srf_frame = vessel.orbit().body().reference_frame();
    
    //Set up stream
    auto ut = space_center.ut_stream();
    auto altitude = vessel.flight().mean_altitude_stream();
    auto apoapsis = vessel.orbit().apoapsis_altitude_stream();
    auto speed    = vessel.flight(srf_frame).speed_stream();
    
    
    //START COUNTING DOWN
    std::cout << "10..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "9..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "8..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "7..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "6..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "5..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "4..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "3..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "2..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    //Start Ignition
    std::cout << "Ignition!" << std::endl;
    vessel.control().activate_next_stage();
    std::this_thread::sleep_for(std::chrono::seconds(15));
    
    //Lift Off
    std::cout << "...and LIFT OFF!" << std::endl;
    vessel.control().activate_next_stage();
    
    
    
    

    while (vessel.flight(srf_frame).speed() <= 500)
    {
        auto speed = vessel.flight(srf_frame).speed_stream();

        if  (speed() == 100)
            vessel.auto_pilot().target_pitch_and_heading(80, 90);
        
        else if  (speed() == 200)
            vessel.auto_pilot().target_pitch_and_heading(70, 90);

        else if  (speed() == 300)
            vessel.auto_pilot().target_pitch_and_heading(60, 90);

        else if  (speed() == 400)
            vessel.auto_pilot().target_pitch_and_heading(50, 90);
        
        else if  (speed() == 500)
            vessel.auto_pilot().target_pitch_and_heading(45, 90);
    }
    
    
    while (vessel.orbit().apoapsis_altitude() < 100000)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    
    
    if (vessel.orbit().apoapsis_altitude() >= 100000)
            {
                std::cout <<"MECO!" << std::endl;
                vessel.control().set_throttle(0);
                std::this_thread::sleep_for(std::chrono::seconds(5));
                //Interstage separation
                std::cout << "Interstage Successful Separation!" << std::endl;
                vessel.control().activate_next_stage();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                //Stage 2 separation
                std::cout << "Stage 2 Sucessful Separation!" << std::endl;
                vessel.control().activate_next_stage();
            }
/* ************** TEST LANDING CODE DISBLED FOR THIS TESTING VERSION ********************/
//    while (vessel.orbit().apoapsis_altitude() < 66000)
//    {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        std::cout << "Apoapsis: " << vessel.orbit().apoapsis_altitude() << std::endl;
//    }

    
//    //MECO at target apoapsis
//    if (vessel.orbit().apoapsis_altitude() >= 66000)
//    {
//        std::cout <<"MECO!" << std::endl;
//        vessel.control().set_throttle(0);
//        std::this_thread::sleep_for(std::chrono::seconds(5));
//        //Interstage separation
//        std::cout << "Interstage Successful Separation!" << std::endl;
//        vessel.control().activate_next_stage();
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        //Stage 2 separation
//        std::cout << "Stage 2 Sucessful Separation!" << std::endl;
//        vessel.control().activate_next_stage();
//    }
}
