#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

#define NUM_OF_SWIMMERS (6)

#if ENABLE_TESTING == 0
static double GetDoubleFromStream(const string &message, double min, double max) {
    double num;
    bool isCorrect = false;
    while (!isCorrect) {
        cout << message << ":";
        isCorrect = (!!(cin >> num)) && num >= min && num <= max;
        if (!isCorrect) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error! Please enter correct number" << endl;
        }
    }
    return num;
}

static string GetStringFromStream(const string &message) {
    std::string str;
    while (true) {
        cout << message << endl;
        while (str.empty()) {
            getline(cin, str);
        }
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    }
    return str;
}
#else

static double GetDoubleFromStream(const string &message, double min, double max) {
    static int count = 0;
    double speeds[NUM_OF_SWIMMERS] = {
            17, 14, 13, 15, 20, 8
    };
    cout << speeds[count] << endl;
    double speed = speeds[count];
    if (++count > NUM_OF_SWIMMERS) {
        count = 0;
    }

    return speed;
}

static string GetStringFromStream(const string &message) {
    static int count = 0;
    string names[NUM_OF_SWIMMERS] = {
            "Swimmer 1", "Swimmer 2", "Swimmer 3", "Swimmer 4", "Swimmer 5", "Swimmer 6"
    };
    cout << names[count] << endl;
    string name = names[count];
    if (++count > NUM_OF_SWIMMERS) {
        count = 0;
    }

    return name;
}

#endif

class ResultsStorage {
    vector<double> items;
    mutex dataAccess;
public:
    void PushItem(double item) {
        dataAccess.lock();
        items.push_back(item);
        dataAccess.unlock();
    }

    const vector<double> &GetStorage() const {
        return items;
    }
};

class Swimmer {
    string name;
    double speed;
    double restOfDistance = 0;
    mutex dataAccess;

public:
    Swimmer(string &inName, double inSpeed) : name(inName), speed(inSpeed) {
        assert(inSpeed > 0);
    }

    static void Swim(Swimmer *swimmer, double distance, ResultsStorage * results) {
        assert(distance > 0);
        assert(nullptr != swimmer);
        assert(nullptr != results);

        double duration = 0;
        double restOfDistance = distance;

        while (restOfDistance > 0) {
            restOfDistance = restOfDistance - swimmer->speed;
            int delay = (restOfDistance > 0) ? 1000 : 1000 - int(fabs(restOfDistance) / swimmer->speed * 1000);

            swimmer->dataAccess.lock();
            swimmer->restOfDistance = (restOfDistance > 0) ? restOfDistance : 0;
            swimmer->dataAccess.unlock();

            duration += delay;
            this_thread::sleep_for(chrono::milliseconds(delay));
        }
        results->PushItem(duration);
    }

    static void ShowProgress(const vector<Swimmer *> &swimmers, double distance) {
        bool isSwimming = true;
        while (isSwimming) {
            isSwimming = false;
            cout << "---------------------" << endl;
            for (auto &swimmer : swimmers) {
                swimmer->dataAccess.lock();
                double tempRest = swimmer->restOfDistance;
                swimmer->dataAccess.unlock();
                if (tempRest > 0) {
                    isSwimming = true;
                }
                cout << "Swimmer " << swimmer->name << " swims " << distance - tempRest << " m" << endl;
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    const string &GetName() {
        return name;
    }
};

int main() {
    vector<Swimmer *> swimmers;
    thread *calls[NUM_OF_SWIMMERS];
    ResultsStorage resultsStorage;

    for (int i = 0; i < NUM_OF_SWIMMERS; ++i) {
        string name = GetStringFromStream(string("Enter swimmer " + to_string(i + 1) + " name:"));
        double speed = GetDoubleFromStream(string("Enter swimmer " + to_string(i + 1) + " speed(0 - 20 m/s)"), 0, 20);
        swimmers.push_back(new Swimmer(name, speed));
    }

    int i = 0;
    for (auto &swimmer : swimmers) {
        calls[i] = new thread(Swimmer::Swim, swimmer, 100, &resultsStorage);
        ++i;
    }

    thread progressThread(Swimmer::ShowProgress, swimmers, 100);

    for (auto &call : calls) {
        call->join();
    }

    progressThread.join();

    i = 0;
    cout << "---------------------" << endl;
    cout << "Results:" << endl;
    auto results = resultsStorage.GetStorage();
    for (auto &swimmer : swimmers) {
        cout << "Swimmer " << swimmer->GetName() << ". Time " << results[i] / 1000 << " s" << endl;
        delete swimmer;
        delete calls[i];
        ++i;
    }

    return 0;
}
