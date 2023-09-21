
private:

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


