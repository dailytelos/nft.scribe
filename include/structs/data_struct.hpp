
struct struct_data {

    carboncert::strctheader header;
    std::string data;
    std::string token;

    struct_data() {};

    struct_data(carboncert::strctheader i_header, std::string i_data, std::string i_token) {
        check(i_data.size() <= 12000, "Data supplied to struct_data exceeds 12,000 characters. ");

        header = i_header;
        data   = i_data;
        token  = i_token;
    };

    //for internal string splitting
    vector<string> _split(string str, string token){
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

    //for internal chopping
    string _chop(string s) {
        return s.substr(0, s.size()-1);
    }

    //for internal conversion of string to asset
    asset _stoa(string s) {
        vector<string> data = _split(s, " ");
        check(data.size() == 2, "Improper data sent to stoa(s). ");
        string sSC = data[1];
        symbol_code sc = symbol_code(sSC);
        check(sc.to_string().size() >= 1, "Improper data sent to stoa(s). ");
        vector<string> amount = _split(data[0], ".");

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

    // is_data_valid(vector<std::string> asVars)
    //  Used to validate ada according to asVars supplied
    //
    //  asVars - A list of variables, a vector of strings like: {"var_1","var_2","data_var_1"}
    //
    //  returns - true if all variables are found inside data
    //            false if error / not all found
    void is_data_valid(vector<std::string> asVars) {
        vector<std::string> asData  = _split(data, token);

        check((asData.size() % 2) == 0, "Invalid data supplied to is_data_valid. ");

        int nFound = 0;

        for(int i=0; i < asVars.size(); i++) {
            for(int i2=0; i2 < asData.size(); i2 = i2 + 2) {
                if(asData[i2] == asVars[i]) { nFound++; break; } //found
            }
        }

        check(asVars.size() == nFound, "Validation of data string failed. ");
    }

    // string get_var(std::string sVarName)
    //  returns a string from data, finding it using sVarName
    //
    // sVarName - string, name of variable to return
    //
    // returns - string on success, NULL on error
    string get_var(std::string sVarName) {
        vector<std::string> asData  = _split(data, token);

        check((asData.size() % 2) == 0, "Invalid data for get_var. ");

        for(int i2=0; i2 < asData.size(); i2 = i2 + 2) {
            if(asData[i2] == sVarName) { return asData[i2+1]; } //found
        }

        return NULL;
    }

    // set_var(std::string sVarName, std::string sValue)
    //  Sets sValue for sVarName in structure data
    //
    //  sVarName - string, name of variable
    //  sValue   - string, value to set for variable
    //
    //  returns - void
    void set_var(std::string sVarName, std::string sValue) {
        vector<std::string> asData  = _split(data, token);

        check((asData.size() % 2) == 0, "Invalid data for set_var. ");

        bool bFound = false;

        for(int i2=0; i2 < asData.size(); i2 = i2 + 2) {
            if(asData[i2] == sVarName) {
                asData[i2+1] = sValue;
                bFound = true;
                break;
            } //found
        }

        if(bFound) {  // rebuild data using new sValue
            string sNew = "";
            for(int i=0; i < asData.size(); i = i + 2) {
                sNew = sNew + asData[i] + token + asData[i+1] + token;
            }
            sNew = _chop(sNew); //chop off last token
            data = sNew; //set sNew as value for data of this structure
        }
    }

    int64_t get_var_as_int(std::string sVarName) {
        return (int64_t) stoi(get_var(sVarName));
    }

    time_point_sec get_var_as_time(std::string sVarName) {
        return time_point_sec(time_point::from_iso_string(get_var(sVarName)));
    }

    asset get_var_as_asset(std::string sVarName) {
        return _stoa(get_var(sVarName));
    }

    EOSLIB_SERIALIZE(struct_data, (header)(data)(token));
};

