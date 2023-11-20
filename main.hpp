#ifndef MAIN_HPP
#define MAIN_HPP

struct Personal_type
{
    short led;
    short temp_max;
    short temp_min;
    short limit;
};

const short LED_DEF = 12;     //stala zawierajac pin 12 w zmiennej led (pin 12 - ten pin obsluguje wentylator)
const short TEMP_MAX_DEF = 60;//stala zawierajaca gorna wartosc temperatury przy ktorej ma zostac wlaczony wentylator
const short TEMP_MIN_DEF = 45;//stala zawierajaca minimalna wartosc temperatury przy ktorej ma zostac wylaczony wentylator
const short LIMIT_DEF = 70;   //stala zawierajaca gorny prog wartosci temperatury przy ktorej powinno dojsc do wylaczenia systemu

double temperatureCPU (double & id_temp);   //funkcja wyswitlajaca aktualna temperature rapsberrypi
void start_program(double & id_temp, const Personal_type & _acc); //funkcja wykonujaca: 1. Pomiary temperatury CPU; 2. Wykonujaca wlaczenie/wylaczenie wiatraka po uzyskaniu odpowiedniego progu temperatury
void fun_on_off(const Personal_type & _acc);//wlaczenie/wylaczenie wiatraka
void show_menu();//wyswietlenie menu
void title();//wyswietlenie loga
void change_acc_led(Personal_type & _acc);//funkcja pozwalajaca zmiane PINu na wlasny
void change_acc_temp(Personal_type & _acc);//funkcja pozwalajaca na zmiane wartosci temperatur 
void init_acc(Personal_type & _acc);       //inicjalizowanie struktury Personal_type domyslnymi wartosciami
void func_init_new_led(Personal_type & _acc); //funkcja sterujaca zmiana PINu
void func_init_new_temp(Personal_type & _acc);//funkcja sterujaca zmiana wartosci temp
void writingPi_GPIO_SET(); //Wczytanie ustawien z GPIO
void waiting(void);
void clean_or_safe_screen (void);
#endif