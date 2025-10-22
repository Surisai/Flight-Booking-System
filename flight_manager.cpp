#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>

class FlightBooking {
private:
    int id;
    int capacity;
    int reserved;

public:
    FlightBooking(int id = 0, int capacity = 0, int reserved = 0) {
        this->id = id;
        this->capacity = (capacity > 0) ? capacity : 0;
        this->reserved = (reserved > 0) ? reserved : 0;

        // Prevent overbooking beyond 105%
        if ((100 * this->reserved) > (105 * this->capacity)) {
            this->reserved = (105 * this->capacity) / 100;
        }
    }

    int getID() const { return id; }
    int getCapacity() const { return capacity; }
    int getReserved() const { return reserved; }

    double getLoadFactor() const {
        return (capacity > 0) ? (100.0 * reserved / capacity) : 0.0;
    }

    bool reserveSeats(int number_of_seats) {
        if (number_of_seats <= 0 || (100 * (reserved + number_of_seats)) > (105 * capacity))
            return false;
        reserved += number_of_seats;
        return true;
    }

    bool cancelSeats(int number_of_seats) {
        if (number_of_seats <= 0 || reserved < number_of_seats)
            return false;
        reserved -= number_of_seats;
        return true;
    }

    void printStatus() const {
        std::cout << std::setw(10) << id
                  << std::setw(12) << capacity
                  << std::setw(12) << reserved
                  << std::setw(12) << std::fixed << std::setprecision(1)
                  << getLoadFactor() << "%" << std::endl;
    }

    // Save flight info to file
    void saveToFile(std::ofstream& out) const {
        out << id << " " << capacity << " " << reserved << "\n";
    }

    // Load flight info from file
    static FlightBooking loadFromFile(std::ifstream& in) {
        int id, capacity, reserved;
        in >> id >> capacity >> reserved;
        return FlightBooking(id, capacity, reserved);
    }
};

// ----------------------------------------------------------------------
// MANAGER CLASS
// ----------------------------------------------------------------------
class FlightManager {
private:
    std::vector<FlightBooking> flights;

public:
    void createFlight(int id, int capacity) {
        for (const auto& f : flights) {
            if (f.getID() == id) {
                std::cout << "❌ Flight " << id << " already exists.\n";
                return;
            }
        }
        flights.push_back(FlightBooking(id, capacity, 0));
        std::cout << "✅ Flight " << id << " created with capacity " << capacity << ".\n";
    }

    void deleteFlight(int id) {
        for (auto it = flights.begin(); it != flights.end(); ++it) {
            if (it->getID() == id) {
                flights.erase(it);
                std::cout << "🗑️ Flight " << id << " deleted.\n";
                return;
            }
        }
        std::cout << "❌ Flight not found.\n";
    }

    void reserveSeats(int id, int seats) {
        for (auto& f : flights) {
            if (f.getID() == id) {
                if (f.reserveSeats(seats))
                    std::cout << "✅ Reserved " << seats << " seats for flight " << id << ".\n";
                else
                    std::cout << "❌ Cannot reserve (may exceed capacity).\n";
                return;
            }
        }
        std::cout << "❌ Flight not found.\n";
    }

    void cancelSeats(int id, int seats) {
        for (auto& f : flights) {
            if (f.getID() == id) {
                if (f.cancelSeats(seats))
                    std::cout << "✅ Canceled " << seats << " seats for flight " << id << ".\n";
                else
                    std::cout << "❌ Cannot cancel (invalid number).\n";
                return;
            }
        }
        std::cout << "❌ Flight not found.\n";
    }

    void showFlights() const {
        if (flights.empty()) {
            std::cout << "No flights in the system.\n";
            return;
        }
        std::cout << "\n--------------------------------------------------\n";
        std::cout << std::setw(10) << "FlightID"
                  << std::setw(12) << "Capacity"
                  << std::setw(12) << "Reserved"
                  << std::setw(12) << "Load %" << std::endl;
        std::cout << "--------------------------------------------------\n";
        for (const auto& f : flights) {
            f.printStatus();
        }
        std::cout << "--------------------------------------------------\n\n";
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        for (const auto& f : flights) {
            f.saveToFile(out);
        }
        out.close();
        std::cout << "💾 Data saved to " << filename << "\n";
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            std::cout << "⚠️ No saved data found.\n";
            return;
        }
        flights.clear();
        while (!in.eof()) {
            FlightBooking f = FlightBooking::loadFromFile(in);
            if (f.getID() != 0) flights.push_back(f);
        }
        in.close();
        std::cout << "📂 Data loaded from " << filename << "\n";
    }
};

// ----------------------------------------------------------------------
// MAIN PROGRAM
// ----------------------------------------------------------------------
int main() {
    FlightManager manager;
    manager.loadFromFile("flights.txt");

    int choice, id, seats, capacity;
    do {
        std::cout << "\n====== ✈️ Flight Booking System ======\n";
        std::cout << "1. Create Flight\n";
        std::cout << "2. Delete Flight\n";
        std::cout << "3. Reserve Seats\n";
        std::cout << "4. Cancel Seats\n";
        std::cout << "5. Show All Flights\n";
        std::cout << "6. Save and Exit\n";
        std::cout << "------------------------------------\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Enter Flight ID and Capacity: ";
                std::cin >> id >> capacity;
                manager.createFlight(id, capacity);
                break;
            case 2:
                std::cout << "Enter Flight ID to delete: ";
                std::cin >> id;
                manager.deleteFlight(id);
                break;
            case 3:
                std::cout << "Enter Flight ID and number of seats to reserve: ";
                std::cin >> id >> seats;
                manager.reserveSeats(id, seats);
                break;
            case 4:
                std::cout << "Enter Flight ID and number of seats to cancel: ";
                std::cin >> id >> seats;
                manager.cancelSeats(id, seats);
                break;
            case 5:
                manager.showFlights();
                break;
            case 6:
                manager.saveToFile("flights.txt");
                std::cout << "Goodbye!\n";
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 6);

    return 0;
}
