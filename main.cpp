#define LINUX 1
#define WINDOWS 0
#include <iostream>
#include "main.hpp"
#if LINUX == 1
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <X11/XKBlib.h>
#endif

int main()
{
    Personal_type acc;
    writingPi_GPIO_SET();
    init_acc(acc);
    double buff;
    char n;
    while(1)
    {
        title();
        show_menu();
        std::cin >> n;
        switch(n)
        {
            case '1':
            {
                clean_or_safe_screen();
                std::cout << "***** Start programu! *****\n";
                std::cout << "***** CAPS LOCK - ON, STOP PROGRAM! *****\n";
                waiting();
                start_program(buff, acc);
                break;
            }
            case '2':
            {
                func_init_new_led(acc);
                break;
            }

            case '3':
            {
                func_init_new_temp(acc);
                break;
            }

            case '4':
            {
                fun_on_off(acc);
                break;
            }

            case '5':
            {
                init_acc(acc);
                break;
            }
            case '6':
                exit(0);

            default:
                break;
        }
    }
    return 0;
}

void title()
{
    clean_or_safe_screen();
    std::cout <<"***************************************************\n"
              <<"***************************************************\n"
              <<"**-----------------------------------------------**\n"
              <<"**----#########----##-----##--------##------##---**\n"
              <<"**----##-----------##-----##-------###-----##----**\n"
              <<"**----########-----##-----##------##-##---##-----**\n"
              <<"**----##-----------##-----##-----##--##--##------**\n"
              <<"**----##-----------##-----##----##----###--------**\n"
              <<"**----##------------#######----##------##--------**\n"
              <<"**-----------------------------------------------**\n"
              <<"***************************************************\n"
              <<"***************************************************\n";
}       

void show_menu()
{
    using std::cout;
    cout << "MENU GLOWNE: \n";
    cout << "___________________________________________________\n";
    cout << "1. Uruchom program.\n";
    cout << "2. Zmien PIN(wyjsciowy) do GPIO.\n";
    cout << "3. Zmien ustawienia temperatury.\n";
    cout << "4. Wlacz/Wylacz wentylator.\n";
    cout << "5. Przywroc ustawienia domyslne.\n";
    cout << "6. Wyjscie.\n";
    cout << "Wybierz opcje: ";
}

void fun_on_off(const Personal_type & _acc)
{
    #if LINUX == 1
    pinMode(_acc.led, OUTPUT);
    if(digitalRead(_acc.led) == 1)
    {
        digitalWrite(_acc.led, LOW);
        printf("\033c");
        std::cout << "***** Wentrylator zostal wylaczony! *****" << std::endl;
        delay(500);
    }
    else
    {
        digitalWrite(_acc.led, HIGH);
        printf("\033c");
        std::cout << "***** Wentylator zostal wlaczony! *****" << std::endl;
        delay(500);
    }
    #endif
    #if WINDOWS == 1
    std::cout << "fun_on_off()" << std::endl;
    #endif
}

void start_program(double & id_temp, const Personal_type & _acc)
{
    #if LINUX == 1
    pinMode(_acc.led, OUTPUT);
    if(digitalRead(_acc.led) == 1)  //sprawdzenie czy wentylator nie zostal juz wczesniej wlaczony
        digitalWrite(_acc.led, LOW);//wylaczenie wentylatora jesli jest wlaczony
    Display *display;
    Status status;
    unsigned state;
    display = XOpenDisplay(getenv ("DISPLAY"));
    while(state != 1)
    {
        if (!display)
            break;
        if(XkbGetIndicatorState (display, XkbUseCoreKbd, &state) != Success)
            state = 2;
        id_temp = temperatureCPU(id_temp);  //funkcja inicjalizujaca zmienna buff - temperatura z CPU
        printf ("Temperatura twojego CPU wynosi: %6.3f *C!\n", id_temp);
        printf ("\033c");
        if(id_temp > _acc.temp_max) //Wywolanie wlaczenia wentylatora jesli temperatura przekroczy 60*C
        {
            digitalWrite(_acc.led, HIGH);    //wlaczenie wentylatora
            std::cout << "Wlaczenie wiatraka! - Temperatura przekroczyla " << _acc.temp_max << "*C! " << std::endl;
            while(id_temp > _acc.temp_min || state != 1)   //wentylator bedzie wlaczony do osiagniecia temperatury ponizej 45*C
            {
                if (!display)
                    break;
                if(XkbGetIndicatorState (display, XkbUseCoreKbd, &state) != Success)
                    state = 2;
                id_temp = temperatureCPU(id_temp);
                printf ("Temperatura twojego CPU wynosi: %6.3f *C!\n", id_temp);  //wypisywanie temperatury CPU
                printf ("\033c");                //czyszczenie konsoli
                if(id_temp > _acc.limit)    //Jesli temperatura po wlaczeniu wentylatora nie bedzie spadac
                {                                //i osiagnie powyzej 70*C, wywola sie wylaczenie systemu
                    std::cout << "Nie odnotowano spadku temperatury! - Nastapi wylaczenie systemu!\n";
                    system("sudo poweroff");       //wylaczenie systemu
                }
                delay(500);    
            }
            digitalWrite(_acc.led, LOW);         //wylaczenie wentylatora
            std::cout << "Wylaczenie wiatraka! - Temperatura spadla ponizej " << _acc.temp_min << "*C! " << std::endl;
        }
        delay(500);
    }
    #endif
    #if WINDOWS == 1
    std::cout << "start_program()" << std::endl;
    id_temp = temperatureCPU(id_temp);
    std::cout << "id_temp = " << id_temp << std::endl;
    #endif
}

double temperatureCPU(double & id_temp)
{
    #if LINUX == 1
    FILE *temperatureFILE;
    temperatureFILE = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temperatureFILE == NULL)
        std::cout << "ERROR!" << std::endl;

    fscanf (temperatureFILE, "%lf", &id_temp);
    id_temp /= 1000;
    fclose (temperatureFILE);
    return id_temp;
    #endif
    #if WINDOWS == 1
    return 1;
    #endif
}

void change_acc_led(Personal_type & _acc)
{
    short f_out{0};
    unsigned short virtual_led;
    while(f_out != 1)
    {
        std::cout << "Podaj nowy PIN (1 - 40): ";
        std::cin >> virtual_led;
        if(std::cin)
        {
            if(virtual_led <= 40 && virtual_led >= 1)
            {
                _acc.led = virtual_led;
                clean_or_safe_screen();
                std::cout << "***** Zapis zakonczony sukcesem! *****\n";
                waiting();
                f_out = 1;
            }
            else
            {
                std::cout << "Podana wartosc wykracza poza mozliwy zasieg!\n";
            }
        }
        else
        {
            std::cout << "Podano niewlasciwy znak! \n";
            std::cin.clear();
            std::cin.ignore();
        }
    }
}

void change_acc_temp(Personal_type & _acc)
{
    short f_out {0};
    unsigned short virtual_limit;
    char virtual_value;
    while(f_out != 1)
    {
        std::cout << "Wybierz wartosc ktora ma zostac zmodyfikwana (1 - 3): ";
        std::cin >> virtual_value;
        switch(virtual_value)
        {
            case '1':
            {
                while(f_out != 1)
                {
                    std::cout << "1. Podaj nowa wartosc: \n" << "Temperature MAX (55*C - 80*C): ";
                    std::cin >> virtual_limit;
                    if(std::cin)
                    {
                        if(virtual_limit <= 100 && virtual_limit >= 55)
                        {
                            _acc.temp_max = virtual_limit;
                            clean_or_safe_screen();
                            std::cout << "***** Zapis zakonczony sukcesem! *****\n";
                            waiting();
                            f_out = 1;
                        }
                        else
                        {
                            clean_or_safe_screen();
                            std::cout << "Podana wartosc wykracza poza wskazany limit!\n";
                        }
                    }else
                    {
                        clean_or_safe_screen();
                        std::cout << "Podano niewlasciwy znak!\n";
                        std::cout << "Obecne ustawienia: \n" 
                        << "1. Temperature MAX: " << _acc.temp_max << "\n" 
                        << "2. Temperature MIN: " << _acc.temp_min << "\n"
                        << "3. Temperature LIMIT: " << _acc.limit << std::endl;
                        std::cin.clear();
                        std::cin.ignore();
                    }
                }
                break;
            }
            case '2':
            {
                while(f_out != 1)
                {
                    std::cout << "1. Podaj nowa wartosc: \n" << "Temperature MIN (1*C - 50*C): ";
                    std::cin >> virtual_limit;
                    if(std::cin)
                    {
                        if(virtual_limit <= 50 && virtual_limit >= 1)
                        {
                            _acc.temp_min = virtual_limit;
                            clean_or_safe_screen();
                            std::cout << "***** Zapis zakonczony sukcesem! *****\n";
                            waiting();
                            f_out = 1;
                        }else
                        {
                            clean_or_safe_screen();
                            std::cout << "Podana wartosc wykracza poza wskazany limit!\n";
                        }
                    }else
                    {
                        clean_or_safe_screen();
                        std::cout << "Podano niewlasciwy znak!\n";
                        std::cout << "Obecne ustawienia: \n" 
                        << "1. Temperature MAX: " << _acc.temp_max << "\n" 
                        << "2. Temperature MIN: " << _acc.temp_min << "\n"
                        << "3. Temperature LIMIT: " << _acc.limit << std::endl;
                        std::cin.clear();
                        std::cin.ignore();
                    }
                }
                break;
            }
            case '3':
            {
                while(f_out != 1)
                {
                    std::cout << "3. Podaj nowa wartosc: \n" << "Temperature LIMIT (60*C - 80*C): ";
                    std::cin >> virtual_limit;
                    if(std::cin)
                    {
                        if(virtual_limit <= 80 && virtual_limit >= 60)
                        {
                            _acc.limit = virtual_limit;
                            clean_or_safe_screen();
                            std::cout << "***** Zapis zakonczony sukcesem! *****\n";
                            waiting();
                            f_out = 1;
                        }
                        else
                        {   
                            clean_or_safe_screen();
                            std::cout << "Podana wartosc wykracza poza wskazany limit!\n";
                        }
                    }else
                    {
                        clean_or_safe_screen();
                        std::cout << "Podano niewlasciwy znak!\n";
                        std::cout << "Obecne ustawienia: \n" 
                        << "1. Temperature MAX: " << _acc.temp_max << "\n" 
                        << "2. Temperature MIN: " << _acc.temp_min << "\n"
                        << "3. Temperature LIMIT: " << _acc.limit << std::endl;
                        std::cin.clear();
                        std::cin.ignore();
                    }
                }
                break;
            }
            default:
            {
                clean_or_safe_screen();
                std::cout << "Podana wartosc wykracza poza mozliwy zasieg!\n";
                waiting();
            }
        }
    }
}

void init_acc(Personal_type & _acc)
{
    _acc.led = LED_DEF;
    _acc.temp_max = TEMP_MAX_DEF;
    _acc.temp_min = TEMP_MIN_DEF;
    _acc.limit = LIMIT_DEF;
    clean_or_safe_screen();
    std::cout << "***** Zainicjowano wartościami domyslnymi *****" << std::endl;
    waiting();
}

void func_init_new_led(Personal_type & _acc)
{
    clean_or_safe_screen();
    char choice;
    short f_out{0};
    while(f_out != 1)
    {
        std::cout << "Obecnie ustawiony PIN(wyjsciowy): " << _acc.led << std::endl;
        std::cout << "Czy chcesz go zmienic? (T/N): ";
        std::cin >> choice;
        switch(choice)
        {
            case 'T':
            case 't':
            {
                change_acc_led(_acc);
                f_out = 1;
                break;
            }
            case 'N':
            case 'n':
            {
                clean_or_safe_screen();
                std::cout << "***** Zapis anulowany! *****" << std::endl;
                waiting();
                f_out = 1;
                break;
            }
            default:
            {
                clean_or_safe_screen();
                std::cout << "Wprowadzono nieprawidlowy znak!" << std::endl;
                waiting();
            }
        }
    }
}

void func_init_new_temp(Personal_type & _acc)
{
    clean_or_safe_screen();
    short f_out{0};
    char choice;
    while(f_out != 1)
    {
        std::cout << "Obecne ustawienia: \n" 
        << "1. Temperature MAX: " << _acc.temp_max << "\n" 
        << "2. Temperature MIN: " << _acc.temp_min << "\n"
        << "3. Temperature LIMIT: " << _acc.limit << std::endl; 
        std::cout << "Czy chcesz zmienic jedna z wartosci? (T/N): ";
        std::cin >> choice;
        switch(choice)
        {
            case 'T':
            case 't':
            {
                change_acc_temp(_acc);
                f_out = 1;
                break;
            }
            case 'N':
            case 'n':
            {
                clean_or_safe_screen();
                std::cout << "***** Zapis anulowany! *****" << std::endl;
                waiting();
                f_out = 1;
                break;
            }
            default:
            {
                clean_or_safe_screen();
                std::cout << "Wprowadzono niewlasciwa wartosc!" << std::endl;
                waiting();
            }
        }
    }
}

void writingPi_GPIO_SET()
{
    #if LINUX == 1
    wiringPiSetupGpio();
    wiringPiSetup(); 
    #endif
    #if WINDOWS == 1
    std::cout << "wiringPiSetupGpio()" << std::endl;
    std::cout << "wiringPiSetup()" << std::endl;
    #endif
}

void waiting(void)
{
    #if LINUX == 1
    delay(500);
    #endif
    #if WINDOWS == 1
    std::cout << "czekam! 500ms !" << std::endl;
    #endif
}
void clean_or_safe_screen (void)
{
    #if LINUX == 1
    printf("\033c");
    #endif
    #if WINDOWS == 1
    printf("Czyszczenie ekranu\n");
    #endif
}