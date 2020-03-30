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
#include <krpc/services/krpc.hpp>
#include <cmath>

int main()
{
    krpc::Client conn = krpc::connect("F9-First-Launch", "192.168.0.10",50000,50001);
    
    
    
    //    //establish connection to KSP
    //    krpc::Client conn = krpc::connect("F9-First-Launch");
    krpc::services::SpaceCenter space_center(&conn);
    
    //assign var vessel to the active vessel on launch pad
    auto vessel = space_center.active_vessel();
    
    
    float turn_start_altitude = 250;
    float turn_end_altitude = 40000;
    float target_altitude = 80000;
    
    //set reference frame
    auto srf_frame = vessel.orbit().body().reference_frame();
    
    
    //Set up stream
    auto ut = space_center.ut_stream();
    auto altitude = vessel.flight().mean_altitude_stream();
    auto apoapsis = vessel.orbit().apoapsis_altitude_stream();
    auto periapsis = vessel.orbit().period_stream();
    auto speed    = vessel.flight(srf_frame).speed_stream();
    
    
    //    double targetPitch = 90;
    
    //SET UP AUTO-PILOT
    //activate auto pilot
    vessel.auto_pilot().engage();
    
    //pitch to target pitch
    vessel.auto_pilot().target_pitch_and_heading(90, 90);
    
    //set throttle power to 100%
    vessel.control().set_throttle(1);
    
    
    //enbale SAS and set to stability assist
    //    vessel.auto_pilot().set_sas(true);
    //    vessel.auto_pilot().set_sas_mode(krpc::services::SpaceCenter::SASMode::stability_assist);
    
    vessel.control().set_sas(false);
    //disbale RCS
    vessel.control().set_rcs(false);
    
    
    
    
    
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
    std::cout << "5..." << "Ignition!"<< std::endl;
    vessel.control().activate_next_stage();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "4..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "3..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "2..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //Lift Off
    std::cout << "...and LIFT OFF!" << std::endl;
    
    
    vessel.control().activate_next_stage();
    
    
    
    
    //Main ascent loop
    double turn_angle = 0;
    
    while (apoapsis() < target_altitude)
    {
        //Gravity turn
        if (altitude() > turn_start_altitude && altitude() < turn_end_altitude)
        {
            double frac = (altitude() - turn_start_altitude)/ (turn_end_altitude - turn_start_altitude);
            double new_turn_angle = frac * 90.0;
            if (fabs(new_turn_angle - turn_angle) > 0.5)
            {
                turn_angle = new_turn_angle;
                vessel.auto_pilot().target_pitch_and_heading(90.0 - turn_angle, 90.0);
            }
        }
    }
    
    
    
    if (vessel.orbit().apoapsis_altitude() >= 70000)
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
    
    //enbale RCS
    vessel.control().set_rcs(true);
    //enable SAS and set to prograde
    vessel.auto_pilot().set_sas(true);
    vessel.auto_pilot().set_sas_mode(krpc::services::SpaceCenter::SASMode::prograde);
    
    vessel.control().activate_next_stage();
    vessel.control().set_throttle(1);
    
    
    while (apoapsis() < 110000)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        if (apoapsis() > 110000)
        {
            vessel.control().set_throttle(0);
            std::cout <<"SECO!" << std::endl;
        }
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    std::cout << "Approaching target apoapsis" << std::endl;
    
    
    while (altitude() < 110000)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    
    
    //disable SAS and set to prograde
    vessel.auto_pilot().set_sas(false);
    vessel.auto_pilot().target_pitch_and_heading(0, 90.0);
    vessel.control().set_throttle(1);
    
    while (periapsis() < 90000 && apoapsis() < 120000)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    
    vessel.control().set_throttle(0);
    std::cout << "Second Stage in orbit: Affermative!" << std::endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    vessel.control().activate_next_stage();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    vessel.control().activate_next_stage();
    std::cout << "Dragon V2 in orbit: Affermative! - Mission Accomplished" << std::endl;
    
    vessel.auto_pilot().disengage();
    
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    vessel.auto_pilot().engage();
    std::cout << "Beginning decend burn" << std::endl;
    
    vessel.auto_pilot().set_sas(true);
    vessel.auto_pilot().set_sas_mode(krpc::services::SpaceCenter::SASMode::retrograde);
    
    
    std::this_thread::sleep_for(std::chrono::seconds(60));
    
    std::cout << "Beginning decend burn" << std::endl;
    
    vessel.control().set_throttle(1);
    
    
    
    
}
