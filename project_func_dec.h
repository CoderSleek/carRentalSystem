void login(std::vector<User>&);

template<typename T>
void read_from_file(std::vector<T>&, const std::string &);

template<typename T>
void write_to_file(T &, const std::string &);
void create_user(std::vector<User> &);
void create_vehicle(std::vector<Travel> &, std::vector<Transport> &);
void book_vehicle(std::vector<Travel> &, std::vector<Transport> &);
void return_car(std::vector<Travel> &, std::vector<Transport> &);
void homepage(std::vector<User> &);
void login(std::vector<User> &users);
void user_menu(std::vector<Travel> &, std::vector<Transport> &);
void admin_menu(std::vector<User> &, std::vector<Travel> &, std::vector<Transport> &);
void display_all_vehicles(std::vector<Travel> &, std::vector<Transport> &);
void display_all_available_vehicles(std::vector<Travel> &, std::vector<Transport> &);
void display_all_booked_vehicles(std::vector<Travel> &, std::vector<Transport> &);
void check_car_status(std::vector<Travel> &, std::vector<Transport> &);
void check_admin();
void format_output(const char*);
void search_vehicle(std::vector<Travel> &, std::vector<Transport> &);
void modify_vehicle(std::vector<Travel> &, std::vector<Transport> &);
void write_all_user(std::vector<User> &v);

template<typename T>
void write_all_vehicles(std::vector<T> &, const std::string &);
template<typename T>
int binary_search_for_vehicles(std::vector<T> &, int);