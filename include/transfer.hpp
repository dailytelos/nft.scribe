
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
        (to.value == get_self().value) &&
        (contract == getcontract()) &&
        (sUnit == getglobalstr(name("tokensymbol"))) ) {
        
        checkfreeze();

        string sMemo = from.to_string() + " deposited " + quant.to_string() + " for future retirement. ";

        //code to retire a certificate by issuer/cert#
        adddeposit(from, quant, sMemo);
         
    } else if(to.value == get_self().value) {
        
        if((memo.substr(0,7) == "deposit") || (memo.substr(0,6) == "refund") || (memo.substr(0,8) == "transfer"))
        {}//ok
        else {
            check(false, "E-t126 - Memo to transfer tokens into " + get_self().to_string() + " must begin with: 'deposit', 'refund', 'transfer'.  Specifying 'retire' in memo will deposit token for 'retire' action. ");
        }
    }
}

private:
