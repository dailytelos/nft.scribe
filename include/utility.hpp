
//splits a string by token
vector<string> split(string str, string token){
    vector<string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

// parse_json needs debug / needs testing
//parses fields from JSON, trims white space, return characters, and removes quote characters
string parse_json(const string& json_data, const string& field) {

    // Parse the JSON by finding the "name" field
    int16_t name_pos = json_data.find("\"" + field + "\"");
    if (name_pos == string::npos)
        return "";

    // Find the beginning of the value (after ":") for the "name" field
    int16_t value_start = json_data.find(":", name_pos);
    if (value_start == string::npos)
        return "";

    // Find the end of the "name" value (up to the next comma or end of JSON object)
    int16_t value_end = json_data.find_first_of(",}", value_start);
    if (value_end == string::npos)
        return "";

    string result = json_data.substr(value_start + 1, value_end - value_start - 1);

    // Trim leading and trailing whitespaces, newline, return characters, and remove quote characters
    auto wsfront = find_if_not(result.begin(), result.end(), [](int c) {
        return isspace(c) || c == '\n' || c == '\r' || c == '\"' || c == '\'';
    });
    auto wsback = find_if_not(result.rbegin(), result.rend(), [](int c) {
        return isspace(c) || c == '\n' || c == '\r' || c == '\"' || c == '\'';
    }).base();

    result = (wsback <= wsfront ? string() : string(wsfront, wsback));

    // Remove quote characters from inside the string
    result.erase(remove(result.begin(), result.end(), '\"'), result.end());
    result.erase(remove(result.begin(), result.end(), '\''), result.end());

    return result;
}

uint8_t digitstoi(string d) {
    check(d.size() == 2, "digitstoi(d) must be provided 2 digits, using the a,b,c,d,z,1-5 method.");
    string adig = d.substr(0,1);
    string bdig = d.substr(1,1);

    uint8_t ret = 0;

    if(adig == "z") { ret = ret + 0; }
    else if(adig == "1") { ret = ret + 10; }
    else if(adig == "2") { ret = ret + 20; }
    else if(adig == "3") { ret = ret + 30; }
    else if(adig == "4") { ret = ret + 40; }
    else if(adig == "5") { ret = ret + 50; }
    else if(adig == "a") { ret = ret + 60; }
    else if(adig == "b") { ret = ret + 70; }
    else if(adig == "c") { ret = ret + 80; }
    else if(adig == "d") { ret = ret + 90; }
    

    if(bdig == "z") { ret = ret + 0; }
    else if(bdig == "1") { ret = ret + 1; }
    else if(bdig == "2") { ret = ret + 2; }
    else if(bdig == "3") { ret = ret + 3; }
    else if(bdig == "4") { ret = ret + 4; }
    else if(bdig == "5") { ret = ret + 5; }
    else if(bdig == "a") { ret = ret + 6; }
    else if(bdig == "b") { ret = ret + 7; }
    else if(bdig == "c") { ret = ret + 8; }
    else if(bdig == "d") { ret = ret + 9; }

    return ret;
}

string itodigits(uint8_t n) {
    check((n < 100) && (n >= 0), "itodigits(uint8_t n) used improperly. ");
    string sret = to_string(n);

    string adig = sret.substr(0,1);
    string bdig = "";

    if(adig == "6") { adig = "a"; }
    else if(adig == "7") { adig = "b"; }
    else if(adig == "8") { adig = "c"; }
    else if(adig == "9") { adig = "d"; }
    else if(adig == "0") { adig = "z"; }

    if(sret.size() == 2)
    {
        bdig = sret.substr(1,1);

        if(bdig == "6") { bdig = "a"; }
        else if(bdig == "7") { bdig = "b"; }
        else if(bdig == "8") { bdig = "c"; }
        else if(bdig == "9") { bdig = "d"; }
        else if(bdig == "0") { bdig = "z"; }
    } else { adig = "z" + adig; }

    return (adig + bdig);
}

string ndig(string s) {
    //check(s.size() > 2, "digitstoeosio(string s) is too long: " + to_string(s.size()));
    //check(s.size() == 0, "digitstoeosio(string s) is zero length. ");

    return itodigits(stoi(s));
}

string chop(string s) {
    return s.substr(0, s.size()-1);
}

asset stoa(string s) {
    vector<string> data = split(s, " ");
    check(data.size() == 2, "Improper data sent to stoa(s). ");
    string sSC = data[1];
    symbol_code sc = symbol_code(sSC);
    check(sc.to_string().size() >= 1, "Improper data sent to stoa(s). ");
    vector<string> amount = split(data[0], ".");

    string sAmount = "";
    uint8_t precision = 0;

    if(amount.size() == 1) { // precision is 0
        sAmount = amount[0];
    } else if (amount.size() == 2) {  // set precision
        precision = (uint8_t) amount[1].size();  //string length is the precision
        sAmount = amount[0] + amount[1];
    } else { //error
        check(false, "Improper data sent to stoa(s). ");
    }

    int64_t nAmount = stoi(sAmount);

    return asset(nAmount, symbol(sc, precision)); 
}

time_point_sec convert_string_to_time_point(const string& sTime) {
    check((sTime.size() == 19 && (sTime[10] == ' ' || sTime[10] == 'T')), "Invalid datetime string format");

    // Extract year, month, day, etc. from the string
    int year = stoi(sTime.substr(0, 4));
    int month = stoi(sTime.substr(5, 2));
    int day = stoi(sTime.substr(8, 2));
    int hour = stoi(sTime.substr(11, 2));
    int minute = stoi(sTime.substr(14, 2));
    int second = stoi(sTime.substr(17, 2));

    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Adjust for leap years
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        days_in_month[2] = 29;
    }

    long calc_seconds = 0;

    // Calculate days for years since epoch
    for (int y = 1970; y < year; y++) {
        calc_seconds += 31536000; // 365 days in seconds
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
            calc_seconds += 86400; // 1 day in seconds for leap years
        }
    }

    // Calculate days for months since the beginning of the year
    for (int m = 1; m < month; m++) {
        calc_seconds += days_in_month[m] * 86400;
    }

    // Add days, hours, minutes and seconds
    calc_seconds += (day - 1) * 86400;
    calc_seconds += hour * 3600;
    calc_seconds += minute * 60;
    calc_seconds += second;

    time_point_sec result = time_point_sec(calc_seconds);
    return result;
}