
public:
//test 
[[eosio::on_notify("*::transfer")]] 
void on_transfer(name from, name to, asset quant, std::string memo) {
    
    //check(get_first_receiver() == "eosio.token"_n, "should be eosio.token");
    //check(false, "Transfers are refused.");

    name contract = get_first_receiver();
    int64_t nAmt = quant.amount;
    symbol_code cUnit = quant.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = quant.symbol.precision();

    check(nAmt > 0, "Insufficient funds sent. ");

    //*** Perform checks based on memo *******************************
    if( ((memo.substr(0,6) == "retire")||(memo.size() == 6)) &&
        (to.value == name("carbon.cert").value) &&
        (contract == getcontract()) ) {
        
        checkfreeze();

        //code to retire a certificate by issuer/cert#

         
    } else if(to.value == name("heartfarmsio").value) {
        
        if((memo.substr(0,7) == "deposit") || (memo.substr(0,6) == "refund") || (memo.substr(0,8) == "transfer"))
        {}//ok
        else {
            check(false, "carbon.cert E-t126 (https://cx.bluefieldrenewable.com/- Memo to transfer tokens into carbon.cert must begin with: 'deposit', 'refund', 'transfer'.  Specifying 'retire' in memo will permanently retire token. ");
        }
    }
}

private:
