#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <ctime>


using namespace std;
struct Date{
	string content;
};
struct Record {
    Date date;
    int minutes;
    string category;
    string description;
};
typedef vector<Record> Records;
Records records;
void enterToContinue(){
	cout<< "Press ENTER to continue..." <<endl;
    cin.ignore(10, '\n');
	cin.get(); 
}
void error(string msg){
	cout << "\033[0;38;5;196mERROR: " << msg << "\033[0m\n";
}

void print(Records records){
	if(records.size()){
		int i = 1;
		// Show the records stored in the vector
		for (const auto& record : records) {
			if(i < 10)
				cout << " ";
			cout << "\033[0;38;5;240m" << i 
				<< " \033[0;38;5;22m" << record.date.content 
				<< " \033[0;38;5;130m" << record.minutes 
				<< " \033[0;38;5;31m" << record.category 
				<< " \033[0;38;5;23m" << record.description
				<< "\033[39m" 
				<< endl;
			i++;
		}
	}else error("No records found.");
}
/**
 * Returns 1 if date1 > date2.
 * Returns -1 if date1 < date2.
 * Returns 0 if date1 == date2.
 **/
int compare_dates(Date date1, Date date2) {
    struct tm tm1 = {0}, tm2 = {0};
    strptime(date1.content.c_str(), "%d-%m-%Y", &tm1);
    strptime(date2.content.c_str(), "%d-%m-%Y", &tm2);

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 > time2) return 1;
    else if (time1 < time2) return -1;
    else return 0;
}
/**
 * Filter records and returns only the records that its date is in the interval [from, to].
 **/
Records getFromTo(Records records, Date from, Date to){
	Records result;
	Records::iterator it = records.begin();
	int fromCond, toCond;
	while(it != records.end()){
		fromCond = compare_dates((*it).date, from);
		if(fromCond == 1 || fromCond == 0){
			toCond = compare_dates((*it).date, to);
			if(toCond == -1 || toCond == 0){
				result.push_back(*it);//add this record to filtered result
			}
		}
		++it;
	}

	return result;
}
Records load(string path){
	Records records;
	const auto& file = path;
	std::ifstream log_file(file);

	if (!log_file.is_open()) {
		string msg = "Could not open file: " + file;
		std::cerr << "Could not open file: " << file << std::endl;
		error(msg);

	}else{
		// Read each line of the file
		std::string line;
		while (std::getline(log_file, line)) {
			std::istringstream iss(line);

			// Read the data for each record
			std::string date, category, description;
			int minutes;
			if (!(iss >> date >> minutes >> std::quoted(category))) {
				std::cerr << "Error while reading line: " << line << std::endl;
				continue;
			}
			std::getline(iss >> std::ws, description);

			Date preparedDate;
			preparedDate.content = date;
			// Store the record in the vector
			records.push_back({preparedDate, minutes, category, description});
		}
		log_file.close();
	}
	
	return records;
}
void print_bar(int size, bool last, string c) {
    cout << "┃";
    for(int i = 0; i < size; i++) {
        cout << c;
    }
    if (last) {
        cout << "┃";
    } else {
        cout << "┫";
    }
    cout << endl;
}

void plot_bars(const unordered_map<string, double>& data, const unordered_map<string, int>& mins, int max_bar_size) {
    int colors[] = {31, 32, 33, 34, 35, 36, 91, 92, 93, 94, 95, 96};
    int num_colors = sizeof(colors) / sizeof(int);
    int current_color = 0;
    for (const auto& [category, value] : data) {
        cout << "\033[1m" << category << "\033[0m ("
			<< (int) (value * 100) << "%, " << mins.at(category) / 60 << " hours):\n";
        int bar_size = max_bar_size * value;
		string block = "\033[" + to_string(colors[current_color]) + "m█\033[0m";
        print_bar(bar_size, true, block);
        current_color = (current_color + 1) % num_colors;
    }
}

void classify_records(const vector<Record>& records) {
    unordered_map<string, int> total_minutes_by_category;
    int total_minutes = 0;

    // Calculate total minutes by category
    for (const auto& record : records) {
        total_minutes_by_category[record.category] += record.minutes;
        total_minutes += record.minutes;
    }

    // Calculate percentage of minutes by category
    unordered_map<string, double> percentage_minutes_by_category;
    for (const auto& [category, minutes] : total_minutes_by_category) {
        percentage_minutes_by_category[category] = (double) minutes / total_minutes;
    }

    // Find the largest bar size to normalize the bars
    int max_bar_size = 50;
    for (const auto& [category, value] : percentage_minutes_by_category) {
        max_bar_size = max(max_bar_size, (int) (value * max_bar_size));
    }

    // Plot the bars
    plot_bars(percentage_minutes_by_category, total_minutes_by_category, max_bar_size);
}/**
 * ==============================================================
 * ================== CHAT-GPT leaving chat :( ==================
 * ==============================================================
 **/
int totalMinutes(Records records){
	int total = 0;
    for (const auto& record : records) total += record.minutes;
	return total;
}

void menu(){
	char response;
	do{
		cout << "======================== MENU ========================" 
			<< "\n[1] Load records from..."
			<< "\n[2] Print records."
			<< "\n[3] Show total time."
			<< "\n[4] Show bar graphic."
			<< "\n[0] Exit." << "\n";
		cin >> response;
		if(response == '0'){
			cout << "\nGood bye! :)" << "\n";
			continue;
		}

		switch(response){
			case '1': {
				string file;
				file = "utn/time2023.log";
				cout << "File to open: ";
				//cin >> file;
				records = load(file);
				break;
			}
			case'2': {
				print(records);
				break;
			}
			case '3': {
				cout << "Total time: " << totalMinutes(records) << " minutes." << "\n";
				break;
			}
			case '4': {
				classify_records(records);
				break;
			}
			default: continue;
		}
		enterToContinue();
	}while(response != '0');
}
int main() {
	menu();
	return 0;
}

