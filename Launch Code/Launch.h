//
//  Header.h
//  KRPC
//
//  Created by Quan Bach  on 10/31/17.
//  Copyright © 2017 QB. All rights reserved.
//

#ifndef Launch_h
#define Launch_h

#include <iostream>
#include <cmath>
#include "Mission.h"

class Launch
{
public:
    void launchToTargetAlt(const int _targetAltitude);
    void takeSecondToOrbit(const int _targetAppoapsis, const int _targetPeriapsis);
    void landingFirstStage();
};

//launch to target altitude function definition
 void Launch::launchToTargetAlt(const int _targetAltitude)
{
    krpc::Client conn = krpc::connect("F9-First-Launch", "192.168.0.10",50000,50001);
    
    
    //    //establish connection to KSP
    //    krpc::Client conn = krpc::connect("F9-First-Launch");
    krpc::services::SpaceCenter space_center(&conn);
    
    //assign var vessel to the active vessel on launch pad
    auto vessel = space_center.active_vessel();
    
    
    float turn_start_altitude = 400;
    float turn_end_altitude = 40000;
    
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
    std::cout << "5..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "4..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "3..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "2..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1..." << "Ignition!" << std::endl;
    vessel.control().activate_next_stage();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //Lift Off
    std::cout << "...and LIFT OFF!" << std::endl;
    
    vessel.control().activate_next_stage();

    double turn_angle = 0;
    
    while (apoapsis() < _targetAltitude)
    {
        //Gravity turn
        if (altitude() > turn_start_altitude && altitude() < turn_end_altitude)
        {
            double frac = (altitude() - turn_start_altitude)/ (turn_end_altitude - turn_start_altitude);
            double new_turn_angle = frac * 90.0;
            if (fabs(new_turn_angle - turn_angle) > 0.8)
            {
                turn_angle = new_turn_angle;
                vessel.auto_pilot().target_pitch_and_heading(90.0 - turn_angle, 90.0);
            }
        }
    }
    
    
    
    if (vessel.orbit().apoapsis_altitude() >= _targetAltitude)
    {
        std::cout <<"MECO!" << std::endl;
        vessel.control().set_throttle(0);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //Interstage separation
        std::cout << "Interstage Successful Separation!" << std::endl;
        vessel.control().activate_next_stage();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //Stage 2 separation
        std::cout << "Stage 2 Sucessful Separation!" << std::endl;
        vessel.control().activate_next_stage();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout <<"Stage 2 Inition!" << std::endl;
        vessel.control().set_throttle(1);
        vessel.control().activate_next_stage();
    }
 

//    for (auto i = space_center.vessels().begin(); i == space_center.vessels().end(); i++)
//    {
////        krpc::services::SpaceCenter::Vessel *vessel = space_center.vessels().data();
//        std::cout << i->name() << " ";
//    }
    std::cout << std::endl;
}

void Launch::takeSecondToOrbit(const int _targetAppoapsis, const int _targetPeriapsis)
{
    Mission currentMission;

    
    krpc::Client conn = krpc::connect("Control of Second Stage", "192.168.0.10",50000,50001);

    krpc::services::SpaceCenter space_center(&conn);
    
    //assign var vessel to the active vessel
    auto vessel = space_center.active_vessel();
    
    //set reference frame
    auto srf_frame = vessel.orbit().body().reference_frame();

    
    //Set up stream
    auto ut = space_center.ut_stream();
    auto altitude = vessel.flight().mean_altitude_stream();
    auto apoapsis = vessel.orbit().apoapsis_altitude_stream();
    auto periapsis = vessel.orbit().period_stream();
    auto speed    = vessel.flight(srf_frame).speed_stream();

    
    //SET UP AUTO-PILOT
    //activate auto pilot
    vessel.auto_pilot().disengage();

    //enbale RCS
    vessel.control().set_rcs(true);
    //enable SAS and set to prograde
    vessel.control().set_sas(true);
    vessel.control().set_sas_mode(krpc::services::SpaceCenter::SASMode::prograde);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    bool fairingNotDeployed = true;
    
    while (apoapsis() < _targetAppoapsis)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
            if (altitude() > 60000 && fairingNotDeployed)
            {
                fairingNotDeployed = false;
//                vessel.control().toggle_action_group(4);
                vessel.control().activate_next_stage();
            }
            
            if (apoapsis() < _targetAppoapsis - 10000)
            {
                space_center.set_physics_warp_factor(3);
            }
            else
            {
                space_center.set_physics_warp_factor(0);
            }
            
            if (apoapsis() > _targetAppoapsis)
                {
                    vessel.control().set_throttle(0);
                    std::cout <<"SECO!" << std::endl;
                }
        }
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    std::cout << "Approaching target apoapsis" << std::endl;
    
    
    while (altitude() < _targetAppoapsis)
    {
        if (altitude() < _targetAppoapsis - 500)
        {
            space_center.set_physics_warp_factor(3);
        }
        else
        {
            space_center.set_physics_warp_factor(0);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    //disable SAS and set to prograde
    vessel.control().set_sas(false);
    vessel.auto_pilot().engage();
    vessel.auto_pilot().target_pitch_and_heading(0, 90.0);
    vessel.control().set_throttle(1);

    
    while (periapsis() < _targetPeriapsis && apoapsis() < 130000)
        std::this_thread::sleep_for(std::chrono::milliseconds (500));

    vessel.control().set_throttle(0);
    std::cout << "Second Stage in orbit: Affermative!" << std::endl;
    vessel.auto_pilot().disengage();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    vessel.control().set_sas(true);
    vessel.control().set_sas_mode(krpc::services::SpaceCenter::SASMode::retrograde);


    std::this_thread::sleep_for(std::chrono::seconds(30));
    
    currentMission.TestLaunch(conn);
    
//    for (int i = 0; i < 4; i++)
//    {
//        space_center.set_physics_warp_factor(3);
//        std::this_thread::sleep_for(std::chrono::seconds(15));
//        space_center.set_physics_warp_factor(0);
//        vessel.control().activate_next_stage();
//    }
    
    std::cout << "All satellites deployed..." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "Begin decending burn..." << std::endl;
    vessel.control().set_throttle(1);
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Mission Accomplished..." << std::endl;
    std::cout << "Switching to First Stage..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

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

}

void Launch::landingFirstStage()
{
    krpc::Client conn = krpc::connect("Landing First Stage", "192.168.0.10",50000,50001);
    
    krpc::services::SpaceCenter space_center(&conn);
    
    //assign var vessel to the active vessel
    auto vessel = space_center.active_vessel();
    
    //set reference frame
    auto srf_frame = vessel.orbit().body().reference_frame();
    
    
    //Set up stream
    auto ut = space_center.ut_stream();
    auto altitude = vessel.flight().mean_altitude_stream();
    auto apoapsis = vessel.orbit().apoapsis_altitude_stream();
    auto periapsis = vessel.orbit().period_stream();
    auto speed    = vessel.flight(srf_frame).speed_stream();
    
    //enbale RCS
    vessel.control().set_rcs(true);
    
    //engage auto pilot and set heading
    vessel.auto_pilot().engage();
    vessel.auto_pilot().target_pitch_and_heading(0, 270);

    std::this_thread::sleep_for(std::chrono::seconds(20));
    
    vessel.auto_pilot().disengage();
    //enable SAS and set to prograde
    vessel.control().set_sas(true);
    vessel.control().set_sas_mode(krpc::services::SpaceCenter::SASMode::retrograde);
    
    
    std::this_thread::sleep_for(std::chrono::seconds(10));

    //shut down 5 engines
    vessel.control().toggle_action_group(3);
    
    bool finsNotDeployed = true;
    
    while (altitude() < 60000)
    {
        if (altitude() < 61000 && finsNotDeployed)
        {
            vessel.control().set_brakes(true);
            finsNotDeployed = false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
    }
    
    krpc::services::SpaceCenter::Vessel ASDS;
    
    for (int i = 0; i < space_center.vessels().size(); i++)
    {
        if (space_center.vessels()[i].name() == "Of Course I Still Love You ASDS")
            ASDS = space_center.vessels()[i];
    }
    space_center.set_target_vessel(ASDS);
    
    vessel.control().set_sas_mode(krpc::services::SpaceCenter::SASMode::anti_target);

}




#endif /* Header_h */
