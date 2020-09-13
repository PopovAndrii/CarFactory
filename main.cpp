#include <iostream>
#include <windows.h>
#include <map>

CRITICAL_SECTION criticalSection;

struct CarInfo {
    BOOL status = true;
};

DWORD WINAPI ThreadProduce(LPVOID arg);
DWORD WINAPI ThreadSell(LPVOID arg);
DWORD WINAPI ThreadShow(LPVOID arg);

class CarFactory {
public:
    int Produce() {
        HANDLE h = CreateThread(0,0,ThreadProduce, &m_cars,0,0);
        if (h == INVALID_HANDLE_VALUE) {
            return -1;
        }
        return 0;
    }
    int SellCar(std::string carSell) {
        m_cars[carSell].status = false;
        
        HANDLE h = CreateThread(0, 0, ThreadSell, &m_cars, 0, 0);
        if (h == INVALID_HANDLE_VALUE) {
            return -1;
        }
        return 0;
    }
    int Show() { 
        HANDLE h = CreateThread(0, 0, ThreadShow, &m_cars, 0, 0);
        if (h == INVALID_HANDLE_VALUE) {
            return -1;
        }
        return 0;
    }

private:
    std::map<std::string, CarInfo> m_cars;
};

int main()
{
    CarFactory* car = new CarFactory();

    InitializeCriticalSection(&criticalSection);
    car->Produce();
    Sleep(500);
    car->Show();
    
    Sleep(500);
    car->SellCar("x4");
    Sleep(500);

    car->Show();
    Sleep(500);
    DeleteCriticalSection(&criticalSection);

}

DWORD WINAPI ThreadProduce(LPVOID arg) {
    std::cout << "ThreadProduce\n";
    EnterCriticalSection(&criticalSection);

    std::map<std::string, CarInfo>& cars = *static_cast<std::map<std::string, CarInfo>*>(arg);
    cars.insert(std::make_pair("x5", CarInfo({})));
    cars.insert(std::make_pair("x6", CarInfo({})));
    cars.insert(std::make_pair("x4", CarInfo({})));
    cars.insert(std::make_pair("x7", CarInfo({})));
    
    LeaveCriticalSection(&criticalSection);
    return 0;
}

DWORD WINAPI ThreadSell(LPVOID arg) {
    std::cout << "ThreadSell Del-";
    std::map<std::string, CarInfo>& cars = *static_cast<std::map<std::string, CarInfo>*>(arg);
    
    EnterCriticalSection(&criticalSection);
    
    std::map<std::string, CarInfo>::iterator it = cars.begin();
    while(it != cars.end()) {
        if (!it->second.status) {
            std::cout << it->first << std::endl;
              cars.erase(it++);
        }
        else {
            ++it;
        }
    }

    LeaveCriticalSection(&criticalSection);
    return 0;
}

DWORD WINAPI ThreadShow(LPVOID arg) {
    std::cout << "ThreadShow\n";
    EnterCriticalSection(&criticalSection);
            
    std::map<std::string, CarInfo>& cars = *static_cast<std::map<std::string, CarInfo>*>(arg);
        
    std::map<std::string, CarInfo>::iterator it = cars.begin();

    for (; it != cars.end(); ++it) {
        std::cout << it->first << std::endl;
    }   

    LeaveCriticalSection(&criticalSection);
    return 0;
}
