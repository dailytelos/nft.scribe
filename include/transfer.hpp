//transfer.hpp

public:

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

    //frozen state
    uint64_t nfreeze = getglobalint(name("freeze"));

    check(nfreeze != 2, "Transfers are disabled, contract is under maintenance. ");

    //*** Perform checks based on memo *******************************
    if( ((memo.substr(0,7) == "deposit")&&(memo.size() == 7)) &&  // for depositing the system token for future payment handle
        (to.value == get_self().value) &&
        (contract == getcontract()) &&
        (sUnit == getglobalstr(name("tokensymbol"))) ) {
        
        checkfreeze();

        string sMemo = from.to_string() + " deposited " + quant.to_string() + " into the contract. ";

        adddeposit(from, quant, sMemo);

    } else if( ((memo.substr(0,16) == "!sys.deposit-alpha!")&&(memo.size() == 19)) &&  // for depositing to admin side of contract
        (to.value == get_self().value) &&
        (contract == getcontract()) &&
        (sUnit == getglobalstr(name("tokensymbol"))) ) {
        
        checkfreeze();

        string sMemo = from.to_string() + " deposited " + quant.to_string() + " into the contract. ";

        adddeposit(from, quant, sMemo);

    } else if(to.value == get_self().value) {
        // Other Types
        // memo string will be formatted as follows userid@network_id
        // Example:  "ad42.sfx@eth.mainnet"

        checkfreeze();

        string aSplit = split(memo, "@");
        check(aSplit.size() == 2, "Invalid memo specified, memos should specify a virtual account user like: ad42.sfx@eth.mainnet ");

        name network_id = name(aSplit[1]);
        name userid = name(aSplit[0]);

        string aSplit2 = split(aSplit[0], ".");
        check(aSplit2.size() == 2, "Invalid memo specified, memos should specify a virtual account user like: ad42.sfx@eth.mainnet ");

        name suffix = name(aSplit2[1]);

        struct_token cToken = gettoken(contract, cUnit);

        _nftuser_token_transfer_in(network_id, userid, cToken, memo);
    }
}

private:
