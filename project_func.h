template<typename T>
void read_from_file(std::vector<T> &vec, const std::string &filename){
    std::ifstream file;
    file.open(filename, std::ios::binary | std::ios::in);
    if(!file){ //file status check
        std::cout<<"Read File does not exist\n";
        return;
    }

    T tempobj;
    file.seekg(0);
    while(file.peek() != EOF){
        file.read((char*)&tempobj, sizeof(tempobj));
        vec.push_back(tempobj);
    }
    file.close();   
}

void read_from_user_file(std::vector<User> &users){ //serialized read for user obj (frnd fnc)
    User temp;
    int x;

    std::ifstream file(userfile, std::ios::binary);

    if(!file){
        std::cout<<"Read File does not exist\n";
        return;
    }
    char *p = (char*)malloc(sizeof(char)); //for extracting char array
    while(true){
        file.read((char*)&temp.user_id, sizeof(temp.user_id));
        file.read((char*)&temp.gender, sizeof(temp.gender));
        file.read((char*)&temp.date_of_birth, sizeof(temp.date_of_birth));
        file.read((char*)&temp.phone, sizeof(temp.phone));
        file.read((char*)&temp.veh, sizeof(temp.veh));

        file.read((char*)&x, sizeof(x));
        p = (char*)realloc(p, x*sizeof(char));
        if(p == NULL) throw Error("Read Error, bad mem allocation"); //safety check
        file.read(p, x);
        temp.user_name.assign(p);

        file.read((char*)&x, sizeof(x));
        p = (char*)realloc(p, x);
        if(p == NULL) throw Error("Read Error, bad mem allocation");
        file.read(p, x);
        temp.home_address.assign(p);

        file.read((char*)&x, sizeof(x));
        p = (char*)realloc(p, x);
        if(p == NULL) throw Error("Read Error, bad mem allocation");
        file.read(p, x);
        temp.e_mail.assign(p);

        users.push_back(temp);
        file.get();
        if(file.eof()) break;
        file.seekg(-1, file.cur);
    }
    free(p);
}

template<typename T> //for writing vehicle class obj
void write_to_file(T &obj, const std::string &filename){

    std::ofstream file(filename, std::ios::binary | std::ios::app);
    file.write((char*)&obj, sizeof(obj));
}

void create_user(std::vector<User> &vec){
    try{
        User temp;

        temp.get_data();
        vec.push_back(temp);
        temp.write();
        std::cout<<"\nUser creation is successful!\nYour user id is '"<<temp.get_userid()
        <<"' Please remember your user id for login, Sign in with your id in the next page\n";
    } catch (Error &obj){
        std::cout<<obj.what();
    }
}

void create_vehicle(std::vector<Travel> &v, std::vector<Transport> &v2){
    std::cout<<"Enter 1 for travel type, 2 for transport type\n";
    int choice;
    std::cin>>choice;
    if(choice == 1){
        Travel tempobj;
        tempobj.get_vehicle_data();
        v.push_back(tempobj);
        write_to_file(tempobj, trav_file);
    } else if(choice == 2) {
        Transport tempobj;
        tempobj.get_vehicle_data();
        v2.push_back(tempobj);
        write_to_file(tempobj, tran_file);
    } else {
        std::cout<<"Try Again\n";
    }

    std::cout<<"\nVehicle Creation Successful with vehicle id "<<Vehicle::get_no_of_vehicles();
}

void book_vehicle(std::vector<Travel> &v, std::vector<Transport> &v2){
    int choice, vid;
    veh_inf p = current_user->get_car_status();
    if(p.v_id != 0){
        std::cout<<"\nYou already have a car booked please return it first!";
        return;
    } //check whether user already has car booked
    std::cout<<"Enter the vehicle id to book: ";
    std::cin>>vid;
    for(int i = 0; i < Vehicle::get_no_of_vehicles(); ++i){
        if(v[i].get_vehicle_id() == vid){
            if(v[i].booked() != -1){
                std::cout<<"\nThis vehicle is already booked";
                return;
            }
            char y_n;
            std::cout<<"\nWould you like to book vehicle with id "<<vid<<" which is a "
            <<v[i].get_car_name()<<" , the booking needs to be of minimum 1 day which will cost "
            <<v[i].get_baserate()<<" and each successive day will cost "<<v[i].get_extendrate();
            std::cin.ignore();
            std::cout<<"\nPress Y to confirm or N to deny: ";
            y_n = getchar();
            if(y_n == 'Y' || y_n=='y'){
                current_user->set_car_status('v', v[i].get_vehicle_id());
                v[i].set_booked(current_user->get_userid());
                std::cout<<"\nCongrats your car has been booked!";
                return;
            } else {
                std::cout<<"\nCar not booked";
                return;
            }
        }
        if(v2[i].get_vehicle_id()==vid){
            if(v2[i].booked() != -1){
                std::cout<<"\nThis vehicle is already booked";
                return;
            }
            char y_n;
            std::cout<<"\nWould you like to book vehicle with id "<<vid<<" which is a "
            <<v2[i].get_car_name()<<" , the booking needs to be of minimum 1 day which will cost "
            <<v2[i].get_baserate()<<" and each successive day will cost "<<v2[i].get_extendrate()<<" + the weight limit/10";
            std::cin.ignore();
            std::cout<<"\nPress Y to confirm or N to deny";
            y_n = getchar();
            if(y_n == 'Y' || y_n=='y'){
                current_user->set_car_status('n', v2[i].get_vehicle_id());
                v2[i].set_booked(current_user->get_userid());
                std::cout<<"\nCongrats your car has been booked!";
                return;
            } else {
                 std::cout<<"\nCar not booked";
                return;
            }
        }
    }
    std::cout<<"\nVehicle not found";
}

void return_car(std::vector<Travel> &v, std::vector<Transport> &v2){
    veh_inf temp = current_user->get_car_status();
    if(temp.v_id == 0){
        std::cout<<"\nYou don't have a car booked!";
        return;
    }
    if(temp.type == 'v'){
        for (int i = 0; i < v.size(); i++){
            if(v[i].get_vehicle_id() == temp.v_id){
                v[i].calculate_amount_due();
                v[i].set_booked(-1);
                break;
            }
        }
    } else {
        for (int i = 0; i < v2.size(); i++){
            if(v2[i].get_vehicle_id() == temp.v_id){
                v2[i].calculate_amount_due();
                v2[i].set_booked(-1);
                break;
            }
        }
    }
    current_user->set_car_status('\0', 0);
}

void check_car_status(std::vector<Travel> &v, std::vector<Transport> &v2){
    veh_inf temp = current_user->get_car_status();
    if(!temp.v_id){
        std::cout<<"\nYou dont have a car booked";
        return;
    }
    if(temp.type == 'v'){
        for(iter(Travel) it = v.begin(); it != v.end(); ++it){
            if(temp.v_id == it->get_vehicle_id()){
                std::cout<<("\nYou have %s booked",it->get_car_name());
                it->calculate_amount_due();
                break;
            }
        }
    } else {
        for(iter(Transport) it = v2.begin(); it != v2.end(); ++it){
            if(temp.v_id == it->get_vehicle_id()){
                std::cout<<"\nYou have %s booked",it->get_car_name();
                it->calculate_amount_due();
                break;
            }
        }
    }
}

void search_vehicle(std::vector<Travel> &v, std::vector<Transport> &v2){
    int num;
    std::cout<<"\nEnter the Id of the vehicle to search: ";
    std::cin>>num;

    if(num < 1 || num > Vehicle::get_no_of_vehicles()){
        std::cout<<"\nWrong Id, try again";
        return;
    }
    
    for(iter(Travel) it = v.begin(); it != v.end(); ++it){
        if(it->get_vehicle_id() == num){
            it->display();
            return;
        }
    }
    for(iter(Transport) it = v2.begin(); it != v2.end(); ++it){
        if(it->get_vehicle_id() == num){
            it->display();
            return;
        }
    }
}

void modify_vehicle(std::vector<Travel> &v, std::vector<Transport> &v2){
    int num;
    std::cout<<"\nEnter the Id of the vehicle to search: ";
    std::cin>>num;

    if(num < 1 || num > Vehicle::get_no_of_vehicles()){
        std::cout<<"\nWrong Id, try again";
        return;
    }
    
    for(int i = 0; i < v.size(); ++i){
        if(v[i].get_vehicle_id() == num){
            v[i].modify();
            std::cout<<"\nSuccessfully Modified";
            return;
        }
    }
    for(int i = 0; i < v2.size(); ++i){
        if(v2[i].get_vehicle_id() == num){
            v2[i].modify();
            return;
        }
    }
}
void write_all_user(std::vector<User> &v){
    std::ofstream file(userfile);
    if(!file){
        std::cout<<"\nFile read error, changes will not be saved";
        return;
    }
    file.seekp(0);
    for(iter(User) it = v.begin(); it != v.end(); ++it){
        it->write();
    }
}
template<typename T>
void write_all_vehicles(std::vector<T> &v, const std::string &filename){
    std::ofstream file(filename);
    if(!file){
        std::cout<<"\nFile read error, changes will not be saved";
        return;
    }
    file.seekp(0);
    for(int i = 0; i < v.size(); ++i){
        file.write((char*)&v[i], sizeof(T));
    }
}
void login(std::vector<User> &users){
    int id;
    User *p;
    char dt[11];

    std::cout<<"\nPlease enter your user id: ";
    std::cin>>id;
    if(id < 1 || id > User::get_no_of_users()){
        std::cout<<"Enter a valid id\n";
        return;
    }
    p = &(users[id-1]); //accessing index of vector at which the object is located
    std::cin.ignore();
    std::cout<<"Please enter your date of Birth(dd/mm/yyyy): ";
    std::cin.getline(dt, 11);
    if(!strcmp(dt, p->get_dob())){
        std::cout<<"Login Successful\n";
        current_user = p;
    } else {
        std::cout<<"User id and password do not match, try again!\n";   
    }
}

void homepage(){
    format_output("Homepage");
    std::cout<<"\nPlease Enter your choice\n";
    std::cout<<"1 for User Login\n";
    std::cout<<"2 for Creating New User Id\n";
    std::cout<<"3 for Admin Access\n";
    std::cout<<"0 to exit this program\n";
    for(int i = 0; i < fs; ++i) std::cout<<'-';
    std::cout<<'\n';
}

void user_menu(){
    format_output("User Menu");
    std::cout<<"\n1 for Booking a car\n";
    std::cout<<"2 for Returning your car\n";
    std::cout<<"3 for Checking your booking or amount due\n";
    std::cout<<"4 for listing out available vehicles for rent\n";
    std::cout<<"0 for logging out and exiting this menu\n";
    for(int i = 0; i < fs; ++i) std::cout<<'-';
    std::cout<<'\n';
}

void admin_menu(){
    format_output("Admin Menu");
    std::cout<<"\n1 for Displaying all vehicles\n";
    std::cout<<"2 for Displaying all available vehicles\n";
    std::cout<<"3 for Displaying all booked vehicles\n";
    std::cout<<"4 for Adding a new car\n";
    std::cout<<"5 for Displaying all existing user\n";
    std::cout<<"6 for Searching for an existing vehicle\n";
    std::cout<<"7 for modifying an existing vehicle\n";
    std::cout<<"0 for exiting out of admin privelages\n";
    for(int i = 0; i < fs; ++i) std::cout<<'-';
    std::cout<<'\n';
}

void format_output(const char* output_string){
    std::cout<<std::setw(50)<<std::right;
    for(int i = 0; i < 30; ++i) std::cout<<'-';
    std::cout<<'\n'<<std::setw(50)<<std::right<<'|';
    std::cout<<std::setw(19)<<std::right<<output_string<<std::setw(10)<<'|'<<'\n';
    std::cout<<std::setw(50)<<std::right;
    for(int i = 0; i < 30; ++i) std::cout<<'-';
}

void display_all_vehicles(std::vector<Travel> &v1, std::vector<Transport> &v2){
    for(iter(Travel) it = v1.begin(); it != v1.end(); ++it){
        it->display();
    }
    for(iter(Transport) it = v2.begin(); it != v2.end(); ++it){
        it->display();
    }
}

void display_all_available_vehicles(std::vector<Travel> &v1, std::vector<Transport> &v2){
    for(iter(Travel) it = v1.begin(); it != v1.end(); ++it){
        if(it->booked() == -1)
            it->display();
    }
    for(iter(Transport) it = v2.begin(); it != v2.end(); ++it){
        if(it->booked() == -1)
            it->display();
    }
}

void display_all_booked_vehicles(std::vector<Travel> &v1, std::vector<Transport> &v2){
    for(iter(Travel) it = v1.begin(); it != v1.end(); ++it){
        if(it->booked() != -1)
            it->display();
    }
    for(iter(Transport) it = v2.begin(); it != v2.end(); ++it){
        if(it->booked() != -1)
            it->display();
    }
}

void check_admin(){
    char pass[15];
    static int tries = 1;
    if(tries > 3){
        std::cout<<"\nYou are not allowed to access admin privelage due to multiple wrong password attempts";
        return;
    }
    std::cin.ignore();
    std::cout<<"\nPlease enter the password for adminstrator access: ";
    std::cin.getline(pass, 15);

    if(!strcmp(pass, "administrator")){
        std::cout<<"\nWelcome, administrator login was successful!";
        is_admin = true;
    } else {
        std::cout<<"\nWrong password, admin access is only for privelaged users, You have "
        <<3 - tries++<<" more tries remaining";
    }
}