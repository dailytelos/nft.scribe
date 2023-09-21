//manage.hpp
//***************************************************
//**** manage.hpp - Contract Management         ****
//***************************************************
public:
// admin contract management
ACTION sysglobalstr(name &var, string &sval);
ACTION sysglobalint(name &var, uint64_t &nval);
ACTION sysglobalast(name &var, asset &aval);
ACTION sysdefaults();
ACTION sysdrawacct(name &acct, name &to, asset &quant, std::string &memo); 
ACTION sysdeposit(name &user, asset &quant, string &memo); 
ACTION sysdelglobal(name &var); 
ACTION sysfreeze(uint64_t &freeze);

// public contract management
ACTION draw(name &user);

private:

void checkfreeze();
string getglobalstr(name var);
void setglobalstr(name var, string sval);
uint64_t getglobalint(name var);
void setglobalint(name var, uint64_t nval);
asset getglobalast(name var);
void setglobalast(name var, asset aval);
void delglobal(name var);
name getcontract();
void adddeposit(const name &user, const asset &quant, const string &memo);
void subdeposit(const name &user, const asset &quant);
void deldeposit(const name &user);
asset getdepamt(const name &user);
string getdepmemo(const name &user);
asset system_asset(int64_t amount);

TABLE globalvars {
    name          var;
    string        sval;
    uint64_t      nval;
    asset         aval;

    auto primary_key() const { return var.value; };
};

TABLE deposits {
    name    user;
    asset   quant;
    string  memo;

    auto primary_key() const { return user.value; };
};


typedef multi_index <name("globals"), globalvars> global_index;
typedef multi_index <name("deposits"), deposits> deposits_index;