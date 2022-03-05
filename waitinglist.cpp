#include <iostream>
#include <string>
#include <algorithm> // sort

using namespace std;

// custom data structures (struct)

// Guest struct -- pointer objects
struct Guest {
    int serial;
    int grpSize;
    string table;
    bool isSeated;
};

// Table struct -- array objects
struct Table {
    int size;
    int currentSize;
    int order;
    string name;
    bool occupied;
    Guest* currentGuest;
};

// function prototypes
void intro(); // opeing exposition
const int getTotal(); // Total number of tables
void setup(Table t[], int n); // tables objects' data entry | parameter explaination
void status(Table t[], int n); // print status
void helpGuest(Table t[], int n); // guest entry
void outro(); // ending exposition

// main function
int main() {

    intro();
    const int total = getTotal(); // taking input for total number of tables

    Table tables[total]; // creating tables object (array) -- fixed amount

    setup(tables, total); // filling out tables objects' data 
    status(tables, total); // displaying current status of tables

    helpGuest(tables, total); // helping guests to get seated -- function chain of command: helpGuest -> waitingList -> merge2Tables -> occupy -> status -> repeat

    outro();

    return 0;
}

// functions

void intro() { // opening expositions
    cout << "\nManager: Good evening! Welcome to the restaurant. You are our receptionist for the night.\nPlease help our guests get a table based on the size of their group.\nYou have the Waiting List algorithm to help you along the way. Best of luck!\n";
}

// getting the number of tables
const int getTotal() {

    int total;
    
    do {
        cout << "\nManager: But first, please decide how many tables you want to set. (You need to have a minimum of 3 and a maximum of 10 tables for our guests)\nYou: ";
        cin >> total;
        if (total < 3 || total > 10) cout << "System: Input not in range! Please enter a valid number!\n";
    } while (total < 3 || total > 10); // input verification

    return total;
}

// setting up the Table objects preliminary data
void setup(Table t[], int n) { // parameter: Table array, total number of tables

    cout << "\nManager: Now you need to decide what are the sizes for each table going to be. (The least is 2 and the most is 8)\n";
   
    for (int i = 0; i < n; i++) {

        do {
            cout << "Table " << i+1 << ": ";
            cin >> t[i].size;
            if (t[i].size < 2 || t[i].size > 8) cout << "System: Input not in range! Please enter a valid number!\n";
        } while (t[i].size < 2 || t[i].size > 8); // input verification
        t[i].currentSize = t[i].size;
        t[i].order = i+1;
        t[i].name = "'Table " + to_string(i+1) + "'";
        t[i].occupied = false;
    }
}

bool comp1(Table a, Table b) { // predicate for sorting in status
    return a.order < b.order;
}

// print status
void status(Table t[], int n) { // parameter: Table array, total number of tables
    sort(t, t+n, comp1);
    cout << "\nSystem: Here is the current status of the tables:\n";
    for (int i = 0; i < n; i++) {
        cout << t[i].name << ": ";
        (t[i].occupied)? cout << "Occupied by customer no. " << t[i].currentGuest->serial << "\n" : 
        cout << "Up to " << t[i].currentSize << " seats available\n";
    }
}

// new guest sitting --> occupying table
void occupy(Table t[], Guest* g, int n) { // parameter: Table array, Guest pointer object, total number of tables
    t[n].occupied = true;
    t[n].currentSize = 0;
    t[n].currentGuest = g;
    g->isSeated = true;
}

// current guest leaving --> emptying table -- currently unused, will be integrated in future versions
void vacant(Table t[], Guest* g, int n) { // parameter: Table array, Guest pointer object, total number of tables
    t[n].occupied = false;
    t[n].currentSize = t[n].size;
    t[n].currentGuest = NULL;
    g->isSeated = false;
    delete g;
}

// merge tables algorithm
bool merge2Tables(Table t[], Guest* g, int n, int s) { // parameter: Table array, Guest pointer object, total number of tables, customer group size
    int a[2]; // array for merging more than 2 tables -- update needed
    int i = 0;
    while (t[i].occupied == true) i++; // ignoring the occupied tables -- comparing will start from the first index with currentSize value > 0 since the array is sorted
    int j = i+1;
    while (i < n) {
        while (j < n) {
            if (s <= (t[i].currentSize + t[j].currentSize)) {
                //break;
                a[0] = i;
                a[1] = j;
                //cout << "i: " << i << " j: " << j << endl; // console log
                goto exitNestedLoop; // exiting the nested loop if solution is found
            }
            //cout << "i: " << i << " j: " << j << endl; // console log
            j++;
        }
        //cout << "i: " << i << " j: " << j << endl; // console log
        i++;
        j = i+1; // resetting j to be the next index of i
   }
   exitNestedLoop: // exiting from nested loops and continuing from here
   if (i < n && j < n) {
        cout << "\nYou: Please go to " << t[a[0]].name << " and " << t[a[1]].name << ". We will merge them for you. Enjoy your meal!" << endl;
        occupy(t, g, i);
        occupy(t, g, j);
        return true;
   }
   else return false;
}

bool comp2(Table a, Table b) { // predicate for sorting in waitingList
    return a.currentSize < b.currentSize;
}

// assigning table based on availibility and group size -- main algorithm!!!
void waitingList(Table t[], Guest* g, int n, int s) { // parameter: Table array, Guest pointer object, total number of tables
    sort(t, t+n, comp2); // sorting tables in ascending order
    //sort(t, t+n, [] (Table a, Table b) -> bool { return (a.currentSize < b.currentSize);}); // using lambda function (not compatible with all compilers)
    int i = 0;
    while (i < n) {
        if (s <= t[i].currentSize) break;
        i++;
    }

    if (i < n) {
        occupy(t, g, i);
        cout << "\nYou: Please go to " << t[i].name << ". Enjoy your meal!" << endl;
    }
    else {
        if (!merge2Tables(t, g, n, s)) cout << "\nYou: Unfortunately, we don't have any tables available for you at the moment. Please visit us another time!\n";
        delete g;
    }
}

// customer entry + waitingList algorithm implementation
void helpGuest(Table t[], int n) { // parameter: Table array, Guest pointer object, total number of tables

    cout << "\nManager: Now that all the tables are set, the guests will start arriving soon. Please take care of them!\n";
    int c;
    int i = 1; // for current customer ID
    
    do { // first customer
        cout << "\nManager: Is there a customer coming? (enter 1 for yes, 0 for no)\nYou: ";
        cin >> c;
        if (c != 1 && c != 0) cout << "Please enter a valid choice!\n";
    } while (c != 1 && c != 0); // input verification
    
    do { // customer entry loop
        if (c == 1) {
            Guest* newGuest = new Guest; // creating an empty customer object
            newGuest->serial = i; // assigning customer ID
            int grp;
            do {
                cout << "\nYou: Hi, welcome to our restaurant! May I know how many people are in your group?\nCustomer: ";
                cin >> grp;
                if (grp <= 0) cout << "Please enter a valid group size!\n";
            } while (grp <= 0); // input verification
            newGuest->grpSize = grp;
            waitingList(t, newGuest, n, grp); // assigning table algorithm
        }
        else cout << "\nYou: Thank you, have a good night!\n";

        i++; // incrementation --> customer serial number/ID
        status(t, n); // printing status after each new entry
        do { // more customers y/n
            cout << "\nManager: Is there another customer coming? (enter 1 for yes, 0 for no) | (System: Saying no will terminate the program)\nYou: ";
            cin >> c;
            if (c != 1 && c != 0) cout << "Please enter a valid choice!\n";
        } while (c != 1 && c != 0); // input verification

        if (c == 0) cout << "\nYou: We don't have any more guests headed this way!\n";

    } while (c != 0); // no more new customer
}

void outro() { // ending expositions
    cout << "Manager: Alright then, we are done for the night. See you tomorrow!\n" << endl;
}


/*

To do:
- Simplify the program to improve general readability
- Update merging algorithm to support more than 2 tables
- Add an option for the customers to place order
- New data structure idea(s): menu, review, rating, comments
- New algorithm idea(s): estimated wait time, customer review/rating

Identified Bugs:
- Status printing wrong serial number (frequency: rare)

*/