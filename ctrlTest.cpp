#include "AdsLib.h"
#include "AdsNotificationOOI.h"
#include "AdsVariable.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static void readStateExample(std::ostream& out, const AdsDevice& route)
{
    const auto state = route.GetState();

    out << "ADS state: " << std::dec << (uint16_t)state.ads << " devState: " << std::dec << (uint16_t)state.device <<
        '\n';
}

static AdsDevice Connect2PLC(){
    // Target IP
    static const AmsNetId remoteNetId{192, 168, 0, 90, 1, 1};
    static const char remoteIpV4[] = "192.168.0.90";

    // uncomment and adjust if automatic AmsNetId deduction is not working as expected
    bhf::ads::SetLocalAddress({192, 168, 0, 3, 1, 1});

    printf("Connecting to PLC...\n");
    AdsDevice route{remoteIpV4, remoteNetId, AMSPORT_R0_PLC_TC3};
    std::cout << "Successfully connected!" << std::endl;
    try{
        readStateExample(std::cout, route);
    } catch (const AdsException& ex) {
        std::cout << "Connection failed. Error code: " << ex.errorCode << "\n";
        std::cout << "AdsException message: " << ex.what() << "\n";
    } catch (const std::runtime_error& ex) {
        std::cout << ex.what() << '\n';
    }
    return route;
}

static void forwardOn(AdsDevice &route){
    static const bool var_value = true;
    AdsVariable<bool> simpleVar {route, "MAIN.Machine.EMMobileBase.VechicleIO.bForward.Force"};
    simpleVar = var_value;
}

static void forwardOff(AdsDevice &route){
    static const bool var_value = false;
    AdsVariable<bool> simpleVar {route, "MAIN.Machine.EMMobileBase.VechicleIO.bForward.Force"};
    simpleVar = var_value;
}

static void setForwarByVel(AdsDevice &route, const double vel){
    AdsVariable<double> AxLeftFront {route, "MAIN.Machine.EMMobileBase.AxLeftFront.ref.NcToPlc.ActVelo"};
    AdsVariable<double> AxRightFront {route, "MAIN.Machine.EMMobileBase.AxRightFront.ref.NcToPlc.ActVelo"};
    AdsVariable<double> AxLeftBack {route, "MAIN.Machine.EMMobileBase.AxLeftBack.ref.NcToPlc.ActVelo"};
    AdsVariable<double> AxRightBack {route, "MAIN.Machine.EMMobileBase.AxRightBack.ref.NcToPlc.ActVelo"};

    AxLeftFront = vel;
    AxRightFront = vel;
    AxLeftBack = vel;
    AxRightBack = vel;

    static const bool var_value = true;
    AdsVariable<bool> enable {route, "MAIN.Machine.EMMobileBase.VechicleIO.bForward.Force"};
    enable = var_value;
}

int main()
{
    auto route = Connect2PLC();

    // Set up keyboard input
    struct termios old_tio{}, new_tio{};
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    int fd = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(fd, F_SETFL, fd | O_NONBLOCK);

    // Listen for keyboard input
    int i = 0;
    while (i < 5)
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0)
        {
            if (c == '8')
            {
                forwardOn(route);
                std::cout << "forward\n";
            }
            else if (c == '5')
            {
                forwardOff(route);
                std::cout << "stop\n";
            }

        }
        usleep(10000);
        i++;
    }

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    fcntl(fd, F_SETFL, fd & ~O_NONBLOCK);

    return 0;
}
