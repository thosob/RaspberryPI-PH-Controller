/* 
 * File:   Ph-Controller.cpp
 * Author: tsobieroy
 * Description: Driver file for the ph Controller 
 * Created on 12. Oktober 2016, 09:28
 */
#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <cmath>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>   
#include <fstream>
#include <sstream>


using namespace std;


float ph9 = 0.0;
float ph7 = 0.0;
float ph4 = 0.0;
float temperature_corrected_ph9 = 0.0;
float temperature_corrected_ph7 = 0.0;
float temperature_corrected_ph4 = 0.0;

//One temperature value for one probe
string temperature_Path;
string probe_Path = "/ph_probes/";

/**
 * @brief Gives back based on the temperature value the change in ph throught temp
 * @param ph4
 * @param temperature_C
 */
float adjustpH4Value( float temperature_C ){
  
        if( temperature_C <= 10.0 ) return 0.0;
        if( temperature_C > 10.0 & temperature_C <= 15.0 ) return 0.0;
        if( temperature_C > 15.0 & temperature_C <= 20.0 ) return 0.0;
        if( temperature_C > 20.0 & temperature_C <= 25.0 ) return 0.0;
        if( temperature_C > 25.0 & temperature_C <= 30.0 ) return 0.01;
        if( temperature_C > 30.0 & temperature_C <= 35.0 ) return 0.02;
        if( temperature_C > 35.0 & temperature_C <= 40.0 ) return 0.03;
        if( temperature_C > 40.0 & temperature_C <= 50.0 ) return 0.05;
        if( temperature_C > 50.0 ) return 0.08;
        
    return 0.0;
}

/**
 * @brief Gives back based on the temperature value the change in ph throught temp
 * @param ph7
 * @param temperature_C
 */
float adjustpH7Value(float temperature_C){
    
    if(temperature_C < 0.0) return 0.13;
    if( temperature_C > 0.0 & temperature_C <= 5.0) return 0.13;
    if( temperature_C > 5.0 & temperature_C <= 10.0) return 0.1;        
    if( temperature_C > 10.0 & temperature_C <= 15.0) return 0.07;
    if( temperature_C > 15.0 & temperature_C <= 20.0) return 0.04;
    if( temperature_C > 20.0 & temperature_C <= 25.0 ) return 0.03;
    if( temperature_C > 25.0 & temperature_C <= 30.0 ) return 0.01;
    if( temperature_C > 30.0 & temperature_C <= 35.0 ) return 0.0;
    if( temperature_C > 35.0 & temperature_C <= 40.0 ) return -0.01;
    if( temperature_C > 40.0 & temperature_C <= 50.0 ) return -0.02;
    if( temperature_C > 50.0 ) return -0.02;    
    return 0.0;
}

/**
 * @brief Gives back based on the temperature value the change in ph throught temp
 * @param ph9
 * @param temperature_C
 */
float adjustpH9Value(float temperature_C){
     
        if(temperature_C < 0.0) return 0.46;
        if(temperature_C > 0.0 & temperature_C <= 5.0)  return 0.46;
        if(temperature_C > 5.0 & temperature_C <= 10.0) return 0.39;
        if(temperature_C > 10.0 & temperature_C <= 15.0) return 0.33;
        if(temperature_C > 15.0 & temperature_C <= 20.0) return 0.27;
        if(temperature_C > 20.0 & temperature_C <= 25.0) return 0.22;
        if(temperature_C > 25.0 & temperature_C <= 30.0) return 0.18;
        if(temperature_C > 30.0 & temperature_C <= 35.0) return 0.14;
        if(temperature_C > 35.0 & temperature_C <= 40.0) return 0.11;
        if(temperature_C > 40.0 & temperature_C <= 50.0) return 0.07;
        if(temperature_C > 50.0) return 0.01;
        return 0.0;
}
/**
 * @brief corrects the temperature 
 * @param temperature_C
 * @return 
 */
float makeTemperatureCorrection(float temperature_C){
   temperature_corrected_ph9 = adjustpH9Value(temperature_C);
   temperature_corrected_ph7 = adjustpH7Value(temperature_C);
   temperature_corrected_ph4 = adjustpH4Value(temperature_C);
    
}

/**
 * @brief mV of the probe
 * @param file_Descriptor
 * @return Voltage
 */
float get_Probe_mV(int ph_Probe_Address, int i2c_Port){
    //Loading phAdress    
    int raw = wiringPiI2CReadReg16(ph_Probe_Address, i2c_Port);
    raw = raw >> 8 | ((raw << 8) &0xffff);
    //std::cout << raw << endl;
    //3.3 equals the voltage
    //Design Decision: 3.3V implementation   
    //4096 - 12bit in total 
    if(raw > 0){
        return (((float) raw / 4096) * 3.3) * 1000;
    }
    else{
        return -1;
    }
}
/**
 * @brief converts celsius to Kelvin
 * @param temperature_C
 * @return Kelvin
 */
float convertCelsiusToKelvin(float temperature_C)
{
    return temperature_C+273.15;
}
/**
 * @brief converts Kelvin to degree celsius
 * @param temperature_K
 * @return 
 */
float convertKelvinToCelsius(float temperature_K)
{
    return temperature_K-273.15;
}


/**
 * @brief calculates the nernst equation
 * @param mean_measurements the mean of all mV measurements
 * @return result -> should be a valid ph_Value
 */
float calculateNernstEquation(float mean_measurements, float temperature_K){
    
    //float E = mean_measurements;
    //Ione Valence
    //float n = -0.00065907;
    //Gas constant
    //float R = 8.314459;
    //Faraday constant
    //float F = 96485.336;
    //Electrode zero
    //float E_o = ph7;
    //Temperature in K
    //float T = 293.15;
    //Slope of nernst equation should be close to -0,059V, which is standart
    //float slope_Nernst = 2.3*((n*F)/(R*T)); //*(mean_measurements - E_o);
    
    //Defining Variables for temperature adjusting
    float ph4_temperature_controlled = ph4*(1+adjustpH4Value(ph4));
    float ph7_temperature_controlled = ph7*(1+adjustpH7Value(ph7));
    float ph9_temperature_controlled = ph9*(1+adjustpH9Value(ph9));
    
    
    // return E/slope_Nernst;
    //n = -0.059/2.3*(R*T)/F; Calculate n in case their is an adjustment error
    
    if(mean_measurements < ph9_temperature_controlled){
        return 9 + log10(ph9_temperature_controlled - mean_measurements); 
        //old formular: return ((9-7)/(ph9-ph7))*(mean_measurements - ph7) + 9; New one is more precise
    }
    if(mean_measurements >= ph9_temperature_controlled && mean_measurements < ph7_temperature_controlled){
        
        return ((9-7)/(ph9_temperature_controlled-ph7_temperature_controlled))*(mean_measurements - ph7_temperature_controlled)+7;
    }
    if(mean_measurements >= ph7_temperature_controlled && mean_measurements < ph4_temperature_controlled){
        
        return ((7-4)/(ph7_temperature_controlled-ph4_temperature_controlled))*(mean_measurements - ph4_temperature_controlled)+4;
    }
    if(mean_measurements >= ph4_temperature_controlled){
        
        return 4 - log10(mean_measurements - ph4_temperature_controlled );
    }
    //slope_Nernst*(mean_measurements - E_o); 
    return 0;          
}

/**
 * @brief gets the mean out of a number of measurements
 * @param measurement_size
 * @param ph_probe_Address
 * @return mean > 0 if enough data was collected, else -1 
 */
float getMeanMeasurements(int measurement_size,int ph_Probe_Address, int i2c_Port){
    float measurements[measurement_size]; 
    float mean_measurements = 0.0;
    float mV = 0.0;
    int valid_data = 0;
    
    for(int i = 0; i < measurement_size; i++){            
        //Gets mV from Probe    
        mV = get_Probe_mV(ph_Probe_Address, i2c_Port);
        //if Probe Value is valid
        if(mV > 0){
            //add to measurements array
            measurements[valid_data] = mV;
            valid_data += 1;                
        }                        
    }
    //if one or more values are valid
    if(measurements[0] != 0){
            for(int i = 0; i < valid_data; i++){
                mean_measurements += measurements[i] / valid_data;
            }
            /* Debug: */
            // std::cout << "Mean: " << mean_measurements << endl 
            //        << "Valid Data Collected:" << valid_data << endl;
        return mean_measurements;
    }    
    else{
        //if no valid data was found, return -1 for mean
        return -1;
    }
}

/**
 * @brief sends out post statement to specified server
 * @param argc
 * @param argv
 * @return 
 */
void writeXmlFile(string ph_Value, string temperature, string ph_Probe_Address, string measurement_mV, string deviceName  ){    
     cout << "<SymbioFilter>" << endl <<
             "<DeviceName>" << deviceName << "</DeviceName>" << endl <<
             "<Ph>" << ph_Value << "</Ph>" << endl <<
             "<ProbeAddress>" << ph_Probe_Address << "</ProbeAddress>" << endl <<
             "<Temperature_C>"<< temperature << "</Temperature_C>" << endl <<
             "<Voltage_mV>"<< measurement_mV << "</Voltage_mV>" << endl <<
             "</SymbioFilter>" << endl;
}

/**
 * @brief converts float to String
 * @param f float
 * @return string
 */
string floatToString(float f){

    std::ostringstream ss;
    ss << f;
    std::string s(ss.str());
    return s;
}

string intToString(int i){
    ostringstream convert;
    convert << i;
    return convert.str();
}

float getTemperatureCelsius(int i){        
    string line;
    string filename;
    string tmp;    
    
    //Define Filename
    filename = temperature_Path;    
    //Open File
    std::ifstream in(filename);   

    //search for t=
    while(getline(in, line)){
        tmp = "";
        if(strstr(line.c_str(), "t=")){
            tmp = strstr(line.c_str(), "t=");
            tmp.erase(0,2);
            if(tmp.length() > 1){
                in.close();
                return strtof(tmp.c_str(),NULL)/1000;
            }
        }
    }
    in.close();
    
    return -1000;
}

/**
 * @brief Main Function to work on server
 * @param argc argument count
 * @param argv argument - only one argument should be supplied: Address of probe
 * @return 
 */
int main(int argc, char *argv[]) {
    
    if(argc != 6){
        std::cout  << "Error: You did not provide required arguments." << endl
                << "Usage: Aqualight-PhController TemperatureFile PhProbeI2CAddress ph4(in mV) ph7(in mV) ph9(in mV) Filedescriptor(0 on default)" 
                << endl
                << "PhProbeI2CAddress has to be provided by integer number of port. E.g. Port:0x4d becomes 77."
                << endl
                << "The filedescriptor from the ADS1015 or whatever multiplexer: if there's just one probe simply 0"
                << endl        
                << "Syntax for port is Port:FD e.g. 77:0"
                << endl        
                << "To achieve that simply convert hex to decimal in multiplying 4*16 and add d=13 to it."
                << endl
                << "PH Values normally come from database and are created by ph-calibrator"                
                << endl;
                        
        return 0;
    }
    //Initialize variables
    temperature_Path = argv[1];
    string str = argv[2];    
    int pos = str.find_first_of(':');
    string i2c_FD = str.substr(0, pos);    
    string ph_Address = str.substr(pos+1);    
    
    ph4 = stof(argv[3]);
    ph7 = stof(argv[4]);
    ph9 = stof(argv[5]);
    
    
    uint i2c_Address = stoi(i2c_FD); //0x4d;
    uint i2c_Port = stoi(ph_Address);
    //stoi(ph_Address);
    int ph_Probe_Address = i2c_Address;
    float mean;     

    float temperature_C = getTemperatureCelsius(0);
    float temperature_K = convertCelsiusToKelvin(temperature_C);
    
    string ph_Value;
    string measurement_mV;
    string ph_Probe_Address_String;
    string deviceName = "Symbiofilter";    
    
    //All temperature files
    //All Ph probes            
    if(i2c_Address >= 0){                        
        //Setting Up Gpio
        wiringPiSetupGpio();
        //Access ph-Probe
        ph_Probe_Address = wiringPiI2CSetup(i2c_Address);     
        ph_Probe_Address_String = intToString(ph_Probe_Address);
                      
        //Calculate Mean Value
        //mean = getMeanMeasurements(65535, ph_Probe_Address,i2c_Port);
        //Debug
       
        mean = getMeanMeasurements(5, ph_Probe_Address,3);
        if(mean > 0){
            measurement_mV = floatToString(mean);
            ph_Value = floatToString(calculateNernstEquation( mean , temperature_K));
            //give all data to Console                              
            writeXmlFile( ph_Value, floatToString(temperature_C), ph_Probe_Address_String, measurement_mV, deviceName  );
        }        
        //return success        
        return 0;
    }
    else{
        std::cout << -1 << endl;
        //return fail
        return 1;
    }
}



