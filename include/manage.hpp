//***************************************************
//**** HEART FARMS - Contract Management         ****
//***************************************************
public:
// admin contract management
ACTION sysglobalstr(name &var, string &sval);
ACTION sysglobalint(name &var, uint64_t &nval);
ACTION sysdefaults();
ACTION sysdrawacct(name &acct, name &to, asset &quant, std::string &memo); 
ACTION sysaddrefund(name &user, asset &quant, string &memo); 
ACTION sysdelglobal(name &var); 
ACTION sysfreeze(uint64_t &freeze);

// public contract management
ACTION refund(name &user);

private:

void checkfreeze();
string getglobalstr(name var);
void setglobalstr(name var, string sval);
uint64_t getglobalint(name var);
void setglobalint(name var, uint64_t nval);
void delglobal(name var);
name getcontract();
void addrefund(name &user, asset &quant, string &memo);


TABLE globalvars {
    name          var;
    string        sval;
    uint64_t      nval;

    auto primary_key() const { return var.value; };
};

TABLE refunds {
    name    user;
    asset   quant;
    string  memo;

    auto primary_key() const { return user.value; };
};


typedef multi_index <name("globals"), globalvars> global_index;
typedef multi_index <name("refunds"), refunds> refunds_index;