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
    if( (memo.substr(0,7) == "deposit") &&  // for depositing the system token for future payment handle
        (to.value == get_self().value) &&
        (contract == getcontract()) &&
        (sUnit == getglobalstr(name("tokensymbol"))) ) {
        
        checkfreeze();

        if(memo.size() == 7) { //deposit into your personal account balance to be used for future payments
            string sMemo = from.to_string() + " deposited " + quant.to_string() + " into the contract. ";

            adddeposit(from, quant, sMemo);
        } else { //deposit into a specific system account
            vector <string> a_memo = split(memo, ":");
            check(a_memo.size() == 2, "Invalid memo, must be formatted as 'deposit:acct.name' or simply 'deposit'. ");

            name system_account = name(a_memo[1]);

            asset prior_balance = getglobalast(system_account);
                    
            check(prior_balance.symbol.code().to_string() != "NULL", "The system account specified does not exist. ");

            asset new_balance = system_asset(prior_balance.amount + quant.amount);
            setglobalast(system_account, new_balance);
        };

    } else if(to.value == get_self().value) {
        // Other Types
        // memo string will be formatted as follows userid@network_id
        // Example:  "ad42.sfx@eth.mainnet"

        checkfreeze();

        vector<string> aSplit = split(memo, "@");
        check(aSplit.size() == 2, "Invalid memo specified, memos should specify a virtual account user like: ad42.sfx@eth.mainnet ");

        name network_id = name(aSplit[1]);
        name userid = name(aSplit[0]);

        vector<string> aSplit2 = split(aSplit[0], ".");
        check(aSplit2.size() == 2, "Invalid memo specified, memos should specify a virtual account user like: ad42.sfx@eth.mainnet ");

        name suffix = name(aSplit2[1]);

        struct_token cToken = gettoken(contract, cUnit);

        check(contract.value == cToken.contract.value, "Mis-match of contracts, unable to send. ");
        check(cToken.token.symbol.code() == quant.symbol.code(), "Mis-match of symbol_code, unable to send. ");
        check(cToken.token.symbol.precision() == quant.symbol.precision(), "Mis-match of precision, unable to send. ");

        cToken.token.amount = quant.amount;

        _nftuser_token_transfer_in(network_id, userid, cToken, memo);

    } else {
        //do nothing
    }
}

private:
