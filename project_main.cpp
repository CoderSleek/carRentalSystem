#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<windows.h>
#include<sstream>
#include<cmath>
#include<exception>
#include<string.h>
#include<iomanip>

#define iter(type) std::vector<type>::iterator
#define citer(type) std::vector<type>::const_iterator

#define userfile "userdata.bat"
#define trav_file "travel.bat"
#define tran_file "transport.bat"
const int fs = 40;
bool is_admin = false;

#include "project_class.h"
#include "project_func_dec.h"
#include "project_func.h"

int main(){
    std::vector<User> all_users;
    std::vector<Travel> trav_vec;
    std::vector<Transport> tran_vec;

    try{
        read_from_user_file(all_users);
    } catch(Error &obj){
        obj.what();
    }
    read_from_file<Travel>(trav_vec, trav_file);
    read_from_file<Transport>(tran_vec, tran_file);
    
    User::set_no_of_users(all_users.size());
    Vehicle::set_no_of_vehicles(trav_vec.size() + tran_vec.size());
    int choice;
    while(true){
        system("cls");
        if(current_user || is_admin){
            if(is_admin){
                admin_menu();
                std::cin>>choice;
                switch(choice){
                    case 0: is_admin = false; break;
                    case 1: display_all_vehicles(trav_vec, tran_vec); break;
                    case 2: display_all_available_vehicles(trav_vec, tran_vec); break;
                    case 3: display_all_booked_vehicles(trav_vec, tran_vec); break;
                    case 4: create_vehicle(trav_vec, tran_vec); break;
                    case 5: for(iter(User) it = all_users.begin(); it!=all_users.end(); ++it){
                                it->display();
                            } break;
                    case 6: search_vehicle(trav_vec, tran_vec); break;
                    case 7: modify_vehicle(trav_vec, tran_vec); break;
                    default: std::cout<<"Please enter a valid choice among the given options\n";
                }
            } else {
                user_menu();
                std::cin>>choice;
                switch(choice){
                    case 0: current_user = NULL; break;
                    case 1: book_vehicle(trav_vec, tran_vec); break;
                    case 2: return_car(trav_vec, tran_vec); break;
                    case 3: check_car_status(trav_vec, tran_vec); break;
                    case 4: display_all_available_vehicles(trav_vec, tran_vec); break;
                    default: std::cout<<"Please enter a valid choice among the given options\n";
                }
            }
        }
        else{
            homepage();
            std::cin>>choice;
            switch(choice){
                case 0: goto out;
                case 1: login(all_users); break;
                case 2: create_user(all_users); break;
                case 3: check_admin(); break;
                default: std::cout<<"Choose a correct option\n";
            }
        }
        std::cout<<"\n";
        system("pause");
    }
    out:
    std::cout<<"\nThank you for using our program!\n";
    write_all_user(all_users);
    write_all_vehicles<Travel>(trav_vec, trav_file);
    write_all_vehicles<Transport>(tran_vec, tran_file);
    return 0;
}