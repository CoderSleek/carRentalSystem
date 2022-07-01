class Error : public std::exception{ //custom error
    std::string err;
public:
    Error(){
        err = "Error encountered"; //default initialization
    }
    Error(const std::string &e){
        err = "Error ecountered: "; //if param passed error is def init + param
        err.append(e);
        err.push_back('\n');
    }
    const char* what() const throw(){
        return err.c_str();
    }
};

//-------------------------------------------------------------------------
typedef struct veh_inf{ //struct for keeping track of booked veh, part of user class
    char type = '\0';
    int v_id = 0;
}veh_inf; //typedef for easy naming
//-------------------------------------------------------------------------
class User{
private:
    static unsigned int users; //static var for count of total users
    unsigned int user_id;
    std::string user_name, home_address, e_mail;
    char gender, date_of_birth[11];
    unsigned long long int phone;
    veh_inf veh;
    void validate_data();
public:
    void get_data();
    void display();
    unsigned int get_userid();
    const char* get_dob();
    void set_car_status(char, int);
    veh_inf get_car_status();
    void write();
    std::string& get_name();
    friend void read_from_user_file(std::vector<User> &);
    static void set_no_of_users(unsigned int);
    static unsigned int get_no_of_users();

} *current_user = NULL; //global user pointer for storing pointer to logged in user

unsigned int User::users;

void User::get_data(){
    std::cin.ignore();
    std::cout<<"Please enter your name: ";
    getline(std::cin, user_name);
    std::cout<<"Please enter your home address: ";
    getline(std::cin, home_address);
    std::cout<<"Enter your email address: ";
    std::cin>>e_mail;
    std::cin.ignore();
    std::cout<<"Enter your date of birth (in dd/mm/yyyy): ";
    std::cin.getline(date_of_birth, 11);
    std::cout<<"Enter your gender as M or F: ";
    std::cin>>gender;
    std::cout<<"Please enter your current phone number: ";
    std::cin>>phone;

    validate_data();
    user_id = ++users; //function will return before this point if any error
}

void User::validate_data(){
    int phone_length = log10(phone) + 1, d, m, y;
    bool e = false;
    for(int i = e_mail.size()-2; i >= 0; --i){
        if(e_mail[i] == '@'){
            e = true;
            break;
        }
    }
    if(!e) throw Error("Invalid email id");//custom exception throw

    std::stringstream dt(date_of_birth); char ch;
    dt>>d>>ch>>m>>ch>>y; //extract dob in 3 dif vars
    
    if(!(m <= 12 && m > 0)) throw Error("Invalid Month"); //month validity check

    if((m==1) || (m==3) || (m==5) || (m==7) //day validity check for months with 31 days
       || (m==8) || (m==10) || (m==12))
    {
        if(!(d<=31 && d>0)){
            throw Error("Invalid date");
        }
    }
    else if(m==2){   //checking for february
        if(((y%4==0) && (y%100 != 0) || (y%400==0)) //checking for leap
            && !(d<=29 && d>0)) throw Error("Leap year feb has 29 days");
        else if(!(d<=28 && d>0)) throw Error("Feb has 28 days"); //normal february
    }
    else{
        if(d>30 || d<1) throw Error("Invalid date"); //rest of the months
    }
    if(y<1900) throw Error("Invalid Year"); //year check

    SYSTEMTIME date; //get a system defined date struct
    GetLocalTime(&date); //populate struct with values (current time)

    if((date.wYear-y) < 18){ //age check whether greater than 18
        throw Error("Below 18 years");
    }
    else if((date.wYear-y) == 18){
        if(date.wMonth < m){
            throw Error("Below 18 years");
        }
        else if((date.wMonth == m) && (date.wDay < d)){
            throw Error("Below 18 years");
        }
    }
    

    if((!user_name.empty()) && (!home_address.empty()) && (!e_mail.empty())
    && (gender=='M' || gender=='F') && (phone_length == 10)) return;
    else{
        throw Error("Invalid user data, please try again");
    }
}

void User::display(){
    std::cout<<"\nUser name is "<<user_name<<" with contact number "<<phone<<" resident of "
    <<home_address<<" with vehicle booked "<<(veh.v_id == 0 ? "none" : "of id %d", veh.v_id)
    <<" and has email of "<<e_mail;
}

unsigned int User::get_userid(){
    return user_id;
}

const char* User::get_dob(){ //login check
    return date_of_birth;
}

void User::set_car_status(char t, int n){
    veh.type = t;
    veh.v_id = n;
}

veh_inf User::get_car_status(){
    return veh;
}

void User::write(){ //serialize write object
    int x;
    std::ofstream file(userfile, std::ios::binary | std::ios::app);

    file.write((char*)&user_id, sizeof(user_id));
    file.write((char*)&gender, sizeof(gender));
    file.write((char*)date_of_birth, sizeof(date_of_birth));
    file.write((char*)&phone, sizeof(phone));
    file.write((char*)&veh, sizeof(veh));
    
    x = user_name.size()+1;
    file.write((char*)&x, sizeof(x));
    file.write((char*)user_name.c_str(), x);
    x = home_address.size()+1;
    file.write((char*)&x, sizeof(x));
    file.write((char*)home_address.c_str(), x);
    x = e_mail.size()+1;
    file.write((char*)&x, sizeof(x));
    file.write((char*)e_mail.c_str(), x);
}
void User::set_no_of_users(unsigned int n){
    users = n;
}
unsigned int User::get_no_of_users(){
    return users;
}
std::string& User::get_name(){
    return user_name;
}
//-------------------------------------------------------------------------
class Vehicle{ //abstract for deriving others
protected:
    static unsigned int no_of_vehicles;

    unsigned int vehicle_id;
    char vehicle_name[50], vehicle_type[30];
    short int vehicle_number;
    char color[11];
    double base_rate, extended_rate;
    int user_booked_id;
public:
    Vehicle(){
        user_booked_id = -1;
    }
    virtual void get_vehicle_data();
    virtual void display()=0; //pvf
    virtual void calculate_amount_due()=0;
    static unsigned int get_no_of_vehicles();
    static void set_no_of_vehicles(unsigned int);
    unsigned int get_vehicle_id();
    int booked();
    void set_booked(int);
    const char* get_car_name();
    double get_baserate();
    double get_extendrate();
    virtual void modify() = 0;
};
unsigned int Vehicle::no_of_vehicles;

void Vehicle::get_vehicle_data(){
    std::cin.ignore();
    std::cout<<"Please enter the company name and model name of the vehicle: ";
    std::cin.getline(vehicle_name, 50);
    std::cout<<"Enter the type of vehicle: ";
    std::cin.getline(vehicle_type, 30);
    std::cout<<"Please enter color of the vehicle(single word): ";
    std::cin>>color;
    std::cout<<"Please enter the number in vehicle's number plate: ";
    std::cin>>vehicle_number;
    std::cout<<"Enter the base rate and the extended rate of the car: ";
    std::cin>>base_rate>>extended_rate;
}
int Vehicle::booked(){return user_booked_id;}

unsigned int Vehicle::get_vehicle_id(){return vehicle_id;}

unsigned int Vehicle::get_no_of_vehicles(){return no_of_vehicles;}

void Vehicle::set_no_of_vehicles(unsigned int n){
    no_of_vehicles = n;
}
const char* Vehicle::get_car_name(){return vehicle_name;}

double Vehicle::get_baserate(){return base_rate;}

double Vehicle::get_extendrate(){return extended_rate;}

void Vehicle::set_booked(int n){
    user_booked_id = n;
}
//-------------------------------------------------------------------------
class Travel : public Vehicle{
private:
    int passenger_capacity;
public:
    Travel() : Vehicle(){}; //parent constructor call
    void get_vehicle_data(); //input data
    void calculate_amount_due();
    void display();
    void modify();
};

void Travel::get_vehicle_data(){
    Vehicle::get_vehicle_data(); //calling same function in parent
    std::cout<<"Enter the passenger capacity of the vehicle: ";
    std::cin>>passenger_capacity;
    vehicle_id = ++no_of_vehicles;
}

void Travel::calculate_amount_due(){
    int no_of_days;
    reinput:
    std::cout<<"\nEnter the number of days car was on rent: ";
    std::cin>>no_of_days;
    if(no_of_days < 1){
        std::cout<<"\nIncorrect date entered, please try again";
        goto reinput;
    }

    double cost = base_rate + (extended_rate * --no_of_days);
    std::cout<<"\nThe vehicle rent stands at total of "<<cost;
}

void Travel::display(){
    std::cout<<"\nVehicle name "<<vehicle_name<<" with id of "<<vehicle_id<<" of travel type "<<vehicle_type<<" with number "
    <<vehicle_number<<" and color "<<color<<", passenger capacity of "<<passenger_capacity;
    std::cout<<"\nhas a base and extended rate of "<<base_rate<<", "<<extended_rate
    <<" and is currently booked by ";
    if(user_booked_id ==-1){
        std::cout<<"no user\n";
    } else {
        std::cout<<"user of id "<<user_booked_id<<std::endl;
    }
}
void Travel::modify(){
    Vehicle::get_vehicle_data();
    std::cout<<"Enter the passenger capacity of the vehicle: ";
    std::cin>>passenger_capacity;
}
//-------------------------------------------------------------------------
class Transport : public Vehicle{
private:
    int weight_limit;
public:
    Transport():Vehicle(){}; //parent ctor call
    void get_vehicle_data();
    void calculate_amount_due();
    void display();
    void modify();
};

void Transport::get_vehicle_data(){
    Vehicle::get_vehicle_data(); //calling same function in parent
    std::cout<<"Enter the maximum weight capacity of the vehicle: ";
    std::cin>>weight_limit;
    vehicle_id = ++no_of_vehicles;
}

void Transport::calculate_amount_due(){
    int no_of_days;
    reinput2:
    std::cout<<"\nEnter the number of days car was on rent: ";
    std::cin>>no_of_days;
    if(no_of_days < 1){
        std::cout<<"\nIncorrect date entered, please try again";
        goto reinput2;
    }

    double cost = base_rate + ((extended_rate + weight_limit/10) * --no_of_days);
    std::cout<<"\nThe vehicle rent stands at total of "<<cost;
}

void Transport::display(){
    std::cout<<"\nVehicle name "<<vehicle_name<<" with id of "<<vehicle_id<<" of transport type "<<vehicle_type<<" with number "
    <<vehicle_number<<" and color "<<color<<", having a maximum weight capacity of "<<weight_limit;
    std::cout<<"\nhas a base and extended rate of "<<base_rate<<", "<<extended_rate
    <<" and is currently booked by ";
    if(user_booked_id ==-1){
        std::cout<<"no user\n";
    } else {
        std::cout<<"by user of id "<<user_booked_id<<std::endl;
    }
}
void Transport::modify(){
    Vehicle::get_vehicle_data();
    std::cout<<"Enter the weight capacity of the vehicle: ";
    std::cin>>weight_limit;
}