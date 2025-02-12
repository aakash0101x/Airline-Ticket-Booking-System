#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <ctime>
using namespace std;

class Flight
{
public:
    string flightNumber, departure, destination, time;
    double price;
    int seatsAvailable;

    Flight() {}
    Flight(string fNum, string dep, string dest, string tm, double p, int seats)
    {
        flightNumber = fNum;
        departure = dep;
        destination = dest;
        time = tm;
        price = p;
        seatsAvailable = seats;
    }

    void displayFlight()
    {
        cout << left << setw(10) << flightNumber << setw(15) << departure << setw(15)
             << destination << setw(8) << time << setw(10) << price << setw(10) << seatsAvailable;
        if (seatsAvailable == 0)
        {
            cout << " (Tickets Unavailable)";
        }
        cout << endl;
    }
};

class User
{
protected:
    string username, password;

public:
    virtual void menu() = 0;
    bool login(string user, string pass);
    void signup(string user, string pass);
};

bool User::login(string user, string pass)
{
    ifstream file("users.txt");
    string u, p;
    while (file >> u >> p)
    {
        if (u == user && p == pass)
        {
            return true;
        }
    }
    return false;
}

void User::signup(string user, string pass)
{
    ofstream file("users.txt", ios::app);
    file << user << " " << pass << endl;
    file.close();
}

class Admin : public User
{
public:
    Admin(string user, string pass)
    {
        username = user;
        password = pass;
    }

    void addFlight();
    void viewFlights();
    void menu();
};

class Passenger : public User
{
public:
    Passenger(string user, string pass)
    {
        username = user;
        password = pass;
    }
    void searchFlights();
    void bookTicket();
    void viewBookings();
    void menu();
    void cancelTicket();
};

void saveFlight(Flight flight)
{
    ofstream file("flights.txt", ios::app);
    file << flight.flightNumber << " " << flight.departure << " " << flight.destination << " "
         << flight.time << " " << flight.price << " " << flight.seatsAvailable << endl;
    file.close();
}

vector<Flight> loadFlights()
{
    vector<Flight> flights;
    ifstream file("flights.txt");
    string fNum, dep, dest, tm;
    double price;
    int seats;
    while (file >> fNum >> dep >> dest >> tm >> price >> seats)
    {
        flights.push_back(Flight(fNum, dep, dest, tm, price, seats));
    }
    file.close();
    return flights;
}

void Admin::addFlight()
{
    string fNum, dep, dest, tm;
    double price;
    int seats;
    cout << "Enter Flight Number: ";
    cin >> fNum;
    cout << "Enter Departure Location: ";
    cin >> dep;
    cout << "Enter Destination Location: ";
    cin >> dest;
    cout << "Enter Time of Departure (HH:MM): ";
    cin >> tm;
    cout << "Enter Fare: ";
    cin >> price;
    cout << "Enter Total Seats Available: ";
    cin >> seats;

    Flight newFlight(fNum, dep, dest, tm, price, seats);
    saveFlight(newFlight);
    cout << "Flight Added Successfully!\n";
}

void Admin::viewFlights()
{
    vector<Flight> flights = loadFlights();
    cout << left << setw(10) << "Flight" << setw(15) << "Departure" << setw(15)
         << "Destination" << setw(8) << "Time" << setw(10) << "Price" << setw(10) << "Seats" << endl;
    for (Flight f : flights)
    {
        f.displayFlight();
    }
}

void Admin::menu()
{
    int choice;
    do
    {
        cout << "\nAdmin Menu:\n1. Add Flight\n2. View Flights\n3. Exit\nChoice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            addFlight();
            break;
        case 2:
            viewFlights();
            break;
        case 3:
            return;
        default:
            cout << "Invalid choice!\n";
        }
    } while (choice != 3);
}

void Passenger::searchFlights()
{
    string dep, dest;
    cout << "Enter Departure City: ";
    cin >> dep;
    cout << "Enter Destination City: ";
    cin >> dest;

    vector<Flight> flights = loadFlights();
    bool found = false;

    cout << "\nAvailable Flights:\n";
    cout << left << setw(10) << "Flight" << setw(15) << "Departure" << setw(15)
         << "Destination" << setw(8) << "Time" << setw(10) << "Price" << setw(10) << "Seats" << endl;

    for (Flight f : flights)
    {
        if (f.departure == dep && f.destination == dest)
        {
            f.displayFlight();
            found = true;
        }
    }

    if (!found)
    {
        cout << "No flights found for the given route.\n";
    }
}

void updateFlightsFile(vector<Flight> flights)
{
    ofstream file("flights.txt", ios::trunc);
    for (Flight f : flights)
    {
        file << f.flightNumber << " " << f.departure << " " << f.destination << " "
             << f.time << " " << f.price << " " << f.seatsAvailable << endl;
    }
    file.close();
}

void Passenger::bookTicket()
{
    string fNum;
    vector<Flight> flights = loadFlights();

    cout << "\nAvailable Flights:\n";
    cout << left << setw(10) << "Flight" << setw(15) << "Departure" << setw(15)
         << "Destination" << setw(8) << "Time" << setw(10) << "Price" << setw(10) << "Seats" << endl;
    for (Flight f : flights)
    {
        f.displayFlight();
    }

    cout << "\nEnter Flight Number to Book: ";
    cin >> fNum;

    for (Flight &f : flights)
    {
        if (f.flightNumber == fNum)
        {
            if (f.seatsAvailable > 0)
            {
                f.seatsAvailable--;
                updateFlightsFile(flights);

                time_t now = time(0);
                char *dt = ctime(&now);

                ofstream file("bookings.txt", ios::app);
                file << username << " " << f.flightNumber << " " << f.departure << " "
                     << f.destination << " " << f.time << " " << dt;
                file.close();

                cout << "\n Ticket Booked Successfully!\n";
            }
            else
            {
                cout << "\n No tickets available for this flight!\n";
            }
            return;
        }
    }
    cout << "\n Flight Not Found!\n";
}

void Passenger::viewBookings()
{
    ifstream file("bookings.txt");
    string user, flightNum, dep, dest, time, date;
    bool found = false;

    cout << left << setw(10) << "Flight" << setw(15) << "Departure" << setw(15)
         << "Destination" << setw(8) << "Time" << " Booking Date & Time" << endl;
    cout << "-------------------------------------------------------------\n";

    while (file >> user >> flightNum >> dep >> dest >> time)
    {
        getline(file, date);
        if (user == username)
        {
            cout << left << setw(10) << flightNum << setw(15) << dep << setw(15)
                 << dest << setw(8) << time << date << endl;
            found = true;
        }
    }
    file.close();

    if (!found)
    {
        cout << "No bookings found for your account.\n";
    }
}

void Passenger::menu()
{
    int choice;
    do
    {
        cout << "\nPassenger Menu:\n1. Search Flights\n2. Book Ticket\n3. View Bookings\n4. Cancel Ticket\n\n5. Exit\nChoice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            searchFlights();
            break;
        case 2:
            bookTicket();
            break;
        case 3:
            viewBookings();
            break;
        case 4:
            cancelTicket();
            break;
        case 5:
            return;
        default:
            cout << "Invalid choice!\n";
        }
    } while (choice != 5);
}

void Passenger::cancelTicket()
{
    string fNum;
    vector<Flight> flights = loadFlights();
    vector<string> updatedBookings;
    bool found = false;
    cout << "\nYou will be refunded only 60% of cost";
    cout << "\nIf you still want to proceed, Enter Flight Number : ";
    cin >> fNum;

    ifstream file("bookings.txt");
    string user, flightNum, dep, dest, time, date;

    while (file >> user >> flightNum >> dep >> dest >> time)
    {
        getline(file, date);
        if (user == username && flightNum == fNum)
        {
            found = true;
            for (Flight &f : flights)
            {
                if (f.flightNumber == fNum)
                {
                    double refund = f.price * 0.6;
                    cout << "\n Ticket Cancelled! You will be refunded $" << refund << "\n";
                    f.seatsAvailable++;
                }
            }
        }
        else
        {
            updatedBookings.push_back(user + " " + flightNum + " " + dep + " " + dest + " " + time + " " + date);
        }
    }
    file.close();

    if (!found)
    {
        cout << "\n No booking found for this flight!\n";
        return;
    }

    ofstream outFile("bookings.txt", ios::trunc);
    for (string line : updatedBookings)
    {
        outFile << line << endl;
    }
    outFile.close();

    updateFlightsFile(flights);
}

int main()
{
    int role, option;
    cout << "1. Login\n2. Signup\nChoice: ";
    cin >> option;

    string username, password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    User *user;
    if (option == 2)
    {
        user = new Passenger(username, password);
        user->signup(username, password);
        cout << "Signup Successful!\n";
    }
    else if (option == 1)
    {
        if (user->login(username, password))
        {
            cout << "Login Successful!\n";
        }
        else
        {
            cout << "Invalid Credentials!\n";
            return 0;
        }
    }

    cout << "Select Role:\n1. Admin\n2. Passenger\nChoice: ";
    cin >> role;
    if (role == 1)
    {
        Admin admin(username, password);
        admin.menu();
    }
    else if (role == 2)
    {
        Passenger passenger(username, password);
        passenger.menu();
    }
    else
    {
        cout << "Invalid Choice!\n";
    }
    return 0;
}
