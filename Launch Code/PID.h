//
//  PID.h
//  KRPC
//
//  Created by Quan Bach  on 11/2/17.
//  Copyright © 2017 QB. All rights reserved.
//

#ifndef PID_h
#define PID_h

class PID
{
private:
    double _lastInput;
    double _lastSampleTime;
    double _errorSum;
    double _Kp;
    double _Ki;
    double _Kd;
    double _setPoint;
    double _minOutput;
    double _maxOutput;
    double _PTerm;
    double _ITerm;
    double _DTerm;
    double _verticalSpeedGoal;
    double _verticalSpeedCurrent;
    double _altitudeGoal;
    double _altitudeCurrent;
    
public:
    
    double PTerm_Calculation (double _Kp, double _speedGoal, double _currentVerticalSpeed);
    double DTerm_Calculation (double _Kd, double _altitudeGoal, double _altitudeCurrent);
    double Thrott_Percent_Calculation (double _gForce, double _shipMass, double _availableThrust);
    
    
};

#endif /* PID_h */
